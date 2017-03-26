#include "application.hpp"

#include <string>
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
    while (true)
    {
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

        // ToDo - Publish any changes.
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