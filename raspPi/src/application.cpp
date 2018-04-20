#include "application.hpp"

#include <chrono>
#include <thread>
#include "modules/SystemModule.hpp"
#include "modules/TelnetModule.hpp"
#include "modules/DiscoveryModule.hpp"
#include "modules/XmlViewModule.hpp"
#include "modules/InsectModule.hpp"
#include "modules/ServoModule.hpp"
#include "modules/ControllerModule.hpp"
#include "modules/CommandLineModule.hpp"

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

void Application::RunLoop()
{    
    SystemModule* pSystemModule = new SystemModule();
    std::unique_ptr<SystemModule> sysModule(pSystemModule);
    modules.push_back(std::move(sysModule));
    modules.push_back(std::make_unique<DiscoveryModule>());
    modules.push_back(std::make_unique<TelnetModule>());
    modules.push_back(std::make_unique<XmlViewModule>("/"));
    modules.push_back(std::make_unique<InsectModule>());
    modules.push_back(std::make_unique<ServoModule>());
    modules.push_back(std::make_unique<ControllerModule>());
    modules.push_back(std::make_unique<CommandLineModule>());
    
    std::cout << std::endl << "************************" << std::endl;
    std::cout << "Initialize all modules" << std::endl;
    // Let all modules create its nodes.
    for (auto &pModul : modules)
    {
        pModul->CreateNodes(root);
    }
    // Update node tree with setting loaded from configuration file.
    pSystemModule->LoadConfig();
    // Link any mirrors.
    root.LinkAllMirrors();
    // Let all modules lookup mirrors and other required nodes.
    for (auto &pModul : modules)
    {
        pModul->LookupNodes();
    }

    typedef chrono::steady_clock ClockType;
    std::cout << "Starting module loop" << std::endl;    
    auto timeStamp = ClockType::now();
    auto prevTimestamp = timeStamp;
    auto elapsedAccum = std::chrono::seconds::zero();
    auto desiredLoopTime = std::chrono::microseconds(long(1000000.0f / pSystemModule->GetLoopFrequency()));
    auto nextTimeStamp = timeStamp + desiredLoopTime;    
    const auto invalidTime = decltype(timeStamp)::max();
    vector<decltype(timeStamp)> moduleTimeout(modules.size(), invalidTime);
     
    while (true)
    {
        desiredLoopTime = std::chrono::microseconds(long(1000000.0f / pSystemModule->GetLoopFrequency()));
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

        // Execute all modules.
        for (auto &pModule : modules)
        {
            pModule->SetElapsedUs(elapsed_us);
            pModule->Execute();
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
        for (auto &pModule : modules)
        {
            pModule->Publish();
        }

        // Clear all changes.
        root.ClearAllChanges();
    }
}