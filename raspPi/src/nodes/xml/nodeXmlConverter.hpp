#pragma once
#include <string>
#include <memory>
#include "../Nodes.hpp"
#include "rapidxml-1.13/rapidxml.hpp"

class NodeXmlConverter
{
public: 
    NodeXmlConverter() {}
    ~NodeXmlConverter() {}
    void ConvertToXml(const BaseNode* pRoot, std::string& xml, FlagType flagMask = FlagType::none, bool onlyChanges = false);
    void UpdateTreeFromXml(BaseNode* pRoot, std::string& xml);
    bool GetResolveMirrors(){ return resolveMirrors; }
    void SetResolveMirrors(bool value){ resolveMirrors = value;}
private:
    bool resolveMirrors = false;
    rapidxml::xml_document<> doc;
    void AddNodeAttributes(rapidxml::xml_node<> *pXmlNode, const BaseNode* pNode, bool onlyChanges);
    rapidxml::xml_node<> *AddChilds(const BaseNode* pParentNode, FlagType flagMask, bool onlyChanges);

    void UpdateNodeAttributes(const rapidxml::xml_node<> *pXmlNode, BaseNode* pNode);
    void UpdateChilds(const rapidxml::xml_node<> *pXmlParentNode, BaseNode* pParentNode);
};
