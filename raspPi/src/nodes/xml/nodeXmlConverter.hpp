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
    void ConvertToXml(const BaseNode* pRoot, std::string& xml);
    void UpdataeTreeFromXml(BaseNode* pRoot, std::string& xml);
private:
    rapidxml::xml_document<> doc;
    void AddNodeAttributes(rapidxml::xml_node<> *pXNode, const BaseNode* pNode);
    void AddNodeChilds(rapidxml::xml_node<> *pXNode, const BaseNode* pParentNode);
};
