#include "SystemModule.hpp"
#include <fstream>
#include <string>
using namespace std;

static const char* pConfigFileName = "ROBINsect.xml";

SystemModule::SystemModule()
{
    SetTimeout(500);
}

SystemModule::~SystemModule()
{
}

void SystemModule::Execute()
{

    const int filterLen = 10;
    if (elapsed_us > 0)
        loopFreqAvg_Hz = ((filterLen - 1.0f) * loopFreqAvg_Hz + 1000000.0f / elapsed_us) / filterLen;

    loopCount++;


}

void SystemModule::CreateNodes(BaseNode& rootNode)
{
    // Add system nodes to node tree.
    pSystemNode = rootNode.FindOrCreateChild("SystemInfo");
    
    pShutdownNode = pSystemNode->FindOrCreateChild<BoolNode>("ShutdownSystem", false);
    pLoopFreqNode = pSystemNode->FindOrCreateChild<FloatNode>("LoopFrequency");
    pLoopFreqNode->Set(60.0f);
    pLoopFreqNode->SetAttribute(unitAttrib.GetID(), "Hz");
    pActualLoopFreqNode = pSystemNode->FindOrCreateChild<FloatNode>("ActualLoopFrequency");
    pActualLoopFreqNode->SetAttribute(unitAttrib.GetID(), "Hz");

    pLoopCountNode = pSystemNode->FindOrCreateChild<Int64Node>("LoopCounter");

    pSaveConfig = pSystemNode->FindOrCreateChild<BoolNode>("SaveConfiguration");
    pLoadConfig = pSystemNode->FindOrCreateChild<BoolNode>("LoadConfiguration");
}

void SystemModule::OnTimer()
{
    pActualLoopFreqNode->Set(loopFreqAvg_Hz);
    pLoopCountNode->Set(loopCount);

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

    if (pShutdownNode->Get() != shutingdown)
    {
        Shutdown();
    }
    
}

void SystemModule::LookupNodes()
{

}

void SystemModule::LoadConfig()
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

        xmlConverter.UpdateTreeFromXml(pSystemNode->GetRoot(), xmlStr);
    }
}

void SystemModule::SaveConfig()
{
    string xmlStr;
    xmlConverter.ConvertToXml(pSystemNode->GetRoot(), xmlStr, FlagType::persist);
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

void SystemModule::Shutdown()
{
    std::string cmd = "sudo shutdown"; // Shutdown commmand.
    if (!pShutdownNode->Get())
    {
        if (shutingdown)
        {
            cmd += " -c"; // Cancel the shutdown request.
            std::system(cmd.c_str());
            shutingdown = false;
        }
    }
    else
    {
        if (!shutingdown)
        {
            //cmd += " -P"; // Power of now 
            std::system(cmd.c_str()); // Shutdown the OS
            shutingdown = true;
        }
    }
}
