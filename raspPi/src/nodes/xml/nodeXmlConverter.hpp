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
    void ConvertToXml(const BaseNode* pRoot, std::string& xml, FlagType flagMask = FlagType::none, bool onlyChangedNodes = false);
    void UpdateTreeFromXml(BaseNode* pRoot, std::string& xml);
private:
    rapidxml::xml_document<> doc;
    void AddNodeAttributes(rapidxml::xml_node<> *pXmlNode, const BaseNode* pNode);
    rapidxml::xml_node<> *AddChilds(const BaseNode* pParentNode, FlagType flagMask, bool onlyChangedNodes);

    void UpdateNodeAttributes(rapidxml::xml_node<> *pXmlNode, BaseNode* pNode);
};
