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