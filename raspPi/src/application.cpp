#include "application.hpp"

#include <string>
#include <chrono>
#include <thread>
#include "modules/TelnetModule.hpp"
#include "modules/InsectModule.hpp"

using namespace std;

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
    // ToDo load config xml from file.
    xmlConverter.UpdateTreeFromXml(&root, xmlStr);
}

void Application::SaveConfig()
{
    string xmlStr;
    xmlConverter.ConvertToXml(&root, xmlStr);
    // ToDo Save to file...
}

void Application::RunLoop()
{
    std::vector<std::unique_ptr<Module>> modules;
    modules.push_back(std::make_unique<TelnetModule>());
    modules.push_back(std::make_unique<InsectModule>());

    // Add system nodes to node tree.
    pSystemNode = root.AddChild("SystemInfo");
    
    pLoopFreqNode = pSystemNode->AddChild<FloatNode>("LoopFrequency", 60.0f);
    pLoopFreqNode->SetAttribute(unitAttrib.GetID(), "Hz");

    pActualLoopFreqNode = pSystemNode->AddChild<FloatNode>("ActualLoopFrequency", 60.0f);
    pActualLoopFreqNode->SetAttribute(unitAttrib.GetID(), "Hz");

    pLoopCountNode = pSystemNode->AddChild<Int64Node>("LoopCounter");

    std::cout << std::endl << "************************" << std::endl;
    std::cout << "Initialize all modules" << std::endl;
    // Execute all modules.
    for (auto &pModul : modules)
    {
        pModul->Init(root);
    }

    NodeXmlConverter xmlConv;
    std::string test;
    xmlConv.ConvertToXml(&root, test);

    std::cout << "Starting module loop" << std::endl;
    int64_t loopCount = 0;
    auto prevTimestamp = chrono::high_resolution_clock::now();
    auto prevUpdate = prevTimestamp;
    auto elapsedAccum = std::chrono::seconds::zero();
    auto desiredLoopTime = std::chrono::microseconds(long(1000000.0f / pLoopFreqNode->Get()));
    auto nextTimeStamp = chrono::high_resolution_clock::now() + desiredLoopTime;
    float loopFreqAvg_Hz = 0;
    const auto invalidTime = chrono::time_point<chrono::high_resolution_clock>::max();
    vector<chrono::time_point<chrono::high_resolution_clock>> moduleTimeout(modules.size(), invalidTime);
    
    while (true)
    {
        auto timeStamp = chrono::high_resolution_clock::now();
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

        if (minTime_point < nextTimeStamp && minTime_point > timeStamp) // A timeout will occur before next execute start.
        {
            //this_thread::sleep_for(minTime_point - timeStamp);
            continue;
        }

        if (nextTimeStamp > timeStamp)
        {
            this_thread::sleep_for(nextTimeStamp - timeStamp);
            continue;
        }
        auto elapsed = timeStamp - prevTimestamp;
        nextTimeStamp = timeStamp + desiredLoopTime;
        prevTimestamp = timeStamp;

        const int filterLen = 10;
        loopFreqAvg_Hz = ((filterLen - 1.0f) * loopFreqAvg_Hz + 1000000.0f / chrono::duration<float, micro>(elapsed).count()) / filterLen;

        loopCount++;
        // Update nodes
        if (chrono::duration<float, milli>(timeStamp - prevUpdate).count() > 500.0f)
        {
            prevUpdate = timeStamp;
            pActualLoopFreqNode->Set(loopFreqAvg_Hz);
            pLoopCountNode->Set(loopCount);
            desiredLoopTime = std::chrono::microseconds(long(1000000.0f / pLoopFreqNode->Get()));
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