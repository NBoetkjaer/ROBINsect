#include "application.hpp"

#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include "modules/TelnetModule.hpp"
#include "modules/InsectModule.hpp"
#include "modules/ServoModule.hpp"
#include "modules/ControllerModule.hpp"

using namespace std;

static const char* pConfigFileName = "ROBINsect.xml";

Application::Application():
    root("root")
{
#ifdef WIN32
    Socket::InitLib(2, 2);
#endif
}

Application::~Application()
{
#ifdef WIN32
    Socket::ExitLib();
#endif
}

void Application::LoadConfig()
{
    string xmlStr;
    // Load config xml from file.
    ifstream configFile(pConfigFileName, ios::in | ios::ate);
    if (configFile.is_open())
    {
        streampos fsize = configFile.tellg();
        
        xmlStr.resize((size_t)fsize);
        configFile.seekg(0, ios::beg);
        configFile.read(&xmlStr[0], fsize);
        configFile.close();

        xmlConverter.UpdateTreeFromXml(&root, xmlStr);
    }
}

void Application::SaveConfig()
{
    string xmlStr;
    xmlConverter.ConvertToXml(&root, xmlStr/*, FlagType::persist*/);
    if (xmlStr.size() < 5) 
        return;
    // Save to file.
    ofstream configFile(pConfigFileName);
    if (configFile.is_open())
    {
        configFile << xmlStr;
        configFile.close();
    }
}

void Application::RunLoop()
{
    modules.push_back(std::make_unique<TelnetModule>());
    modules.push_back(std::make_unique<InsectModule>());
    modules.push_back(std::make_unique<ServoModule>());
    modules.push_back(std::make_unique<ControllerModule>());  

    // Add system nodes to node tree.
    pSystemNode = root.FindOrCreateChild("SystemInfo");
    
    pLoopFreqNode = pSystemNode->FindOrCreateChild<FloatNode>("LoopFrequency");
    pLoopFreqNode->Set(60.0f);
    pLoopFreqNode->SetAttribute(unitAttrib.GetID(), "Hz");

    pActualLoopFreqNode = pSystemNode->FindOrCreateChild<FloatNode>("ActualLoopFrequency");
    pActualLoopFreqNode->SetAttribute(unitAttrib.GetID(), "Hz");

    pLoopCountNode = pSystemNode->FindOrCreateChild<Int64Node>("LoopCounter");

    pSaveConfig = pSystemNode->FindOrCreateChild<BoolNode>("SaveConfiguration");
    pLoadConfig = pSystemNode->FindOrCreateChild<BoolNode>("LoadConfiguration");

    std::cout << std::endl << "************************" << std::endl;
    std::cout << "Initialize all modules" << std::endl;
    // Let all modules create its nodes.
    for (auto &pModul : modules)
    {
        pModul->CreateNodes(root);
    }
    // Update node tree with setting loaded from configuration file.
    LoadConfig();
    // Link any mirrors.
    root.LinkAllMirrors();
    // Let all modules lookup mirrors and other required nodes.
    for (auto &pModul : modules)
    {
        pModul->LookupNodes();
    }
//// Test
//    MirrorNode* pTestNode = root.FindOrCreateChild<MirrorNode>("MirrorTest", "/Insect/BodyPosition");
//    MirrorNode* pTestNode2 = pTestNode->FindOrCreateChild<MirrorNode>("MirrorTest2", "/SystemInfo/LoopCounter");
//
//    pTestNode->LinkMirror();
//    pTestNode2->LinkMirror();
//
//    Pos3D_32f_Node* pBodyPos = pTestNode->GetMirrorSource<Pos3D_32f_Node>();
//    FloatNode* pLoop = root.FindNode<FloatNode>("/MirrorTest/MirrorTest2", false, false);
//    pBodyPos = root.FindNode<Pos3D_32f_Node>("/MirrorTest", false, true);
//    pBodyPos->SetPosition(1, 2, 3);
//
//
    NodeXmlConverter xmlConv;
//    std::string test;
//    xmlConv.ConvertToXml(&root, test);
//// Test end
    typedef chrono::steady_clock ClockType;
    std::cout << "Starting module loop" << std::endl;
    int64_t loopCount = 0;
    auto timeStamp = ClockType::now();
    auto prevTimestamp = timeStamp;
    auto prevUpdate = timeStamp;
    auto elapsedAccum = std::chrono::seconds::zero();
    auto desiredLoopTime = std::chrono::microseconds(long(1000000.0f / pLoopFreqNode->Get()));
    auto nextTimeStamp = timeStamp + desiredLoopTime;
    float loopFreqAvg_Hz = 0;
    const auto invalidTime = decltype(timeStamp)::max();
    vector<decltype(timeStamp)> moduleTimeout(modules.size(), invalidTime);
     
    while (true)
    {
        timeStamp = ClockType::now();
        auto minTime_point = nextTimeStamp;
        // Call the periodic timeout for the relevant modules.
        for(size_t idx = 0; idx < modules.size(); ++idx)
        {
            auto& pModule = modules.at(idx);
            auto& mtime = moduleTimeout.at(idx);
            if (mtime != invalidTime)
            {
                if (pModule->GetTimeout() < 0) // Turn off timer
                {
                    mtime = invalidTime;
                    continue;
                }
                if (timeStamp > mtime) // Times up!
                {
                    pModule->OnTimer(); // Call timer function.
                    mtime += chrono::milliseconds(pModule->GetTimeout()); // Schedule next timeout.
                }
                minTime_point = std::min(mtime, minTime_point);
            }
            else
            {
                if (pModule->GetTimeout() > 0) // Turn on timer
                {
                    mtime = timeStamp + chrono::milliseconds(pModule->GetTimeout());
                    minTime_point = std::min(mtime, minTime_point);
                }
            }
        }
        auto delay = minTime_point - timeStamp;
        if (delay > std::chrono::seconds::zero()) { this_thread::sleep_for(delay); }
        if(minTime_point == nextTimeStamp) // Next action is loop execute
        {
            timeStamp = ClockType::now();
        }
        else // Next action is a timeout.
        {
            continue;
        }

        auto  elapsed_us = chrono::duration<float, micro>(timeStamp - prevTimestamp).count();
        nextTimeStamp +=  desiredLoopTime;
        prevTimestamp = timeStamp;

        const int filterLen = 10;
        if (elapsed_us > 0)
            loopFreqAvg_Hz = ((filterLen - 1.0f) * loopFreqAvg_Hz + 1000000.0f / elapsed_us) / filterLen;

        loopCount++;
        // Update nodes
        if (chrono::duration<float, milli>(timeStamp - prevUpdate).count() > 500.0f)
        {
            prevUpdate = timeStamp;
            pActualLoopFreqNode->Set(loopFreqAvg_Hz);
            pLoopCountNode->Set(loopCount);
            desiredLoopTime = std::chrono::microseconds(long(1000000.0f / pLoopFreqNode->Get()));
        }

        if (pSaveConfig->Get())
        {   // Save configuration and reset save flag.
            SaveConfig();
            pSaveConfig->Set(false);
        }

        if (pLoadConfig->Get())
        {   // Load configuration and reset load flag.
            LoadConfig();
            pLoadConfig->Set(false);
        }


        // Execute all modules.
        for (auto &pModul : modules)
        {
            pModul->Execute();
        }

        // Continue to notify all nodes until the tree is stabilized.
        const int maxIterations = 8 * sizeof(size_t) - 1;
        int iterations = 0;
        while (root.AnyRecentChanges() && iterations++ < maxIterations)
        {
            root.PushChangeHistory();
            root.Notify(true);
        }
        if (iterations >= maxIterations)
        {
            // ToDo Error.
        }

        // Publish all modules.
        for (auto &pModul : modules)
        {
            pModul->Publish();
        }
        std::string test;
        xmlConv.ConvertToXml(&root, test, FlagType::none, true);
        if (test.size() > 5)
        {
            cout << test << endl;
        }
        test.clear();
        // Clear all changes.
        root.ClearAllChanges();
    }
}