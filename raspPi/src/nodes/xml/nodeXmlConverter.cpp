#include "nodeXmlConverter.hpp"
#include "../NodeFactory.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"
#include <iostream>

using namespace std;
using namespace rapidxml;

void NodeXmlConverter::AddNodeAttributes(xml_node<> *pXmlNode, const BaseNode* pNode)
{
    bool onlyChanges = true;
    string attribVal;
    if (!onlyChanges)
    {
        // Add type attribute.
        xml_attribute<> *attr = doc.allocate_attribute(typeAttrib.GetName().c_str(), NodeFactory::NodeTypeName(pNode).c_str());
        pXmlNode->append_attribute(attr);
    }
    // Add all used attributes.
    for (attribID_t i = 0; i < Attribute::GetNumAttributes(); ++i)
    {
        if (onlyChanges && !pNode->IsAttributeChanged(i))
        {
            continue;
        }
        if (pNode->IsAttributeUsed(i))
        {
            if (pNode->GetAttribute(i, attribVal))
            {
                char *value = doc.allocate_string(attribVal.c_str());// Allocate string and copy name into it
                xml_attribute<> *attr = doc.allocate_attribute(Attribute::GetAttributeName(i).c_str(), value);
                pXmlNode->append_attribute(attr);
            }
            else
            {
                // Error unable to retrieve attribute value.
            }
        }
    }
}

xml_node<> *NodeXmlConverter::AddChilds(const BaseNode* pParentNode, FlagType flagMask, bool onlyChangedNodes)
{
    const auto &children = pParentNode->GetChilds();
    xml_node<> *pXmlParent = nullptr;
    for (const auto & childNode : children)
    {
        if (onlyChangedNodes && !childNode->AnyChanges())
        {
            continue;
        }
        xml_node<> *pXmlChild = AddChilds(childNode.get(), flagMask, onlyChangedNodes);
        if (pXmlChild != nullptr)
        {
            if (pXmlParent == nullptr)
            {   // Create the childs parent node.
                pXmlParent = doc.allocate_node(node_element, pParentNode->GetName().c_str());
                // Only add attributes if mask is set.
                if ((pParentNode->GetFlags() & flagMask) != FlagType::none
                    || flagMask == FlagType::none) // If mask is FlagType::none it means 'don't use mask'.
                {
                    // Append all attributes.
                    AddNodeAttributes(pXmlParent, pParentNode);
                }
            }
            // Append child node.
            pXmlParent->append_node(pXmlChild);
        }
    }
    if (!onlyChangedNodes || pParentNode->AnyChanges())
    {
        if ((pParentNode->GetFlags() & flagMask) != FlagType::none
            || flagMask == FlagType::none) // If mask is FlagType::none it means 'don't use mask'.
        {
            if (pXmlParent == nullptr)
            {   // Create the childs parent node.
                pXmlParent = doc.allocate_node(node_element, pParentNode->GetName().c_str());
                // Append all attributes.
                AddNodeAttributes(pXmlParent, pParentNode);
            }
        }
    }
    return pXmlParent;
}

void NodeXmlConverter::ConvertToXml(const BaseNode* pRoot, std::string& xmlString, FlagType flagMask, bool onlyChangedNodes)
{
    doc.clear();
    xml_node<> *pXmlNode = AddChilds(pRoot, flagMask, onlyChangedNodes);
    if (pXmlNode)
    {
        doc.append_node(pXmlNode);
    }
    xmlString.clear();
    print(std::back_inserter(xmlString), doc, 0);
}

void NodeXmlConverter::UpdateTreeFromXml(BaseNode* pRoot, std::string& xmlString)
{
    doc.clear();
    doc.parse<0>(&xmlString[0]);
    xml_node<> *pXmlNode = doc.first_node();
    BaseNode* pNode = pRoot;

    bool bMoreNodes = (pXmlNode != nullptr);
    while (bMoreNodes)
    {
        // Check if current xml node exist in node tree.
        pNode = pNode->FindNode(pXmlNode->name());
        if (pNode)
        {
            UpdateNodeAttributes(pXmlNode, pNode);
        }
        else
        {   // Node not found 
            // create node
        }
        pXmlNode = pXmlNode->next_sibling();
    }
}

void NodeXmlConverter::UpdateNodeAttributes(rapidxml::xml_node<> *pXmlNode, BaseNode* pNode)
{
    xml_attribute<> *pXmlAttribute = pXmlNode->first_attribute();
    // Check all attributes
    while (pXmlAttribute)
    {
        attribID_t attrId = Attribute::GetAttributeID(pXmlAttribute->name());
        if (attrId != INVALID_ATTRIBUTE_ID)
        {
            bool success = pNode->SetAttribute(attrId, pXmlAttribute->value());
            if (!success)
            {
                if (attrId == typeAttrib.GetID())
                {
                    if (NodeFactory::NodeTypeName(pNode).compare(pXmlAttribute->value()) != 0)
                    {
                        // Error: Tried to change the type of an existing node.
                    }
                }
                // Unhandled attribute found in xml.
            }
        }
        else
        {
            // Unknown attribute found in xml.
        }
        pXmlAttribute = pXmlAttribute->next_attribute();
    }
}