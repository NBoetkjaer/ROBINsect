#include "nodeXmlConverter.hpp"
#include "../NodeFactory.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"
#include <iostream>

using namespace std;
using namespace rapidxml;

const char* pTypeAttribute = "type";

void NodeXmlConverter::AddNodeAttributes(xml_node<> *pXNode, const BaseNode* pNode)
{
    string attribVal;
    // Add type attribute.
    xml_attribute<> *attr = doc.allocate_attribute(pTypeAttribute, NodeFactory::NodeTypeName(pNode).c_str());
    pXNode->append_attribute(attr);
    // Add all used attributes.
    for (attribID_t i = 0; i < Attribute::GetNumAttributes(); ++i)
    {
        if (pNode->IsAttributeUsed(i))
        {
            if (pNode->GetAttribute(i, attribVal))
            {
                char *value = doc.allocate_string(attribVal.c_str());// Allocate string and copy name into it
                xml_attribute<> *attr = doc.allocate_attribute(Attribute::GetAttributeName(i).c_str(), value);
                pXNode->append_attribute(attr);
            }
            else
            {
                // Error unable to retrieve attribute value.
            }
        }
    }
}

void NodeXmlConverter::AddNodeChilds(xml_node<> *pXNode, const BaseNode* pParentNode)
{
    const auto &children = pParentNode->GetChilds();
    for (const auto & childNode: children)
    {
        xml_node<> *pXChildNode = doc.allocate_node(node_element, childNode->GetName().c_str());
        pXNode->append_node(pXChildNode);
        // Append all attribute.
        AddNodeAttributes(pXChildNode, childNode.get());
        // Recursive call to add child childs
        AddNodeChilds(pXChildNode, childNode.get());
    }
}

void NodeXmlConverter::ConvertToXml(const BaseNode* pRoot, std::string& xmlString)
{
    doc.clear();
    xml_node<> *pXNode = doc.allocate_node(node_element, pRoot->GetName().c_str());
    doc.append_node(pXNode);
    AddNodeAttributes(pXNode, pRoot);
    AddNodeChilds(pXNode, pRoot);
    xmlString.clear();
    print(std::back_inserter(xmlString), doc, 0);
}

void NodeXmlConverter::UpdataeTreeFromXml(BaseNode* pRoot, std::string& xml)
{

}
