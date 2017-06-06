using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;

namespace ROBINinspect
{
    public class NodeXmlConverter
    {
        void ConvertToXml(BaseNode root, ref String strXml, FlagType flagMask = FlagType.none, bool onlyChanges = false)
        {
            throw new NotImplementedException();
        }

        public void UpdateTreeFromXml(BaseNode root, String strXml)
        {
            // Create a reader that uses the NameTable.
            XmlReaderSettings settings = new XmlReaderSettings();
            // settings.NameTable = xmlNt;
            settings.CheckCharacters = false;
            settings.IgnoreWhitespace = true;
            XmlReader xmlrd = XmlReader.Create(new StringReader(strXml), settings);
            xmlrd.Read();

            UpdateChilds(xmlrd, root);
        }


        void UpdateChilds(XmlReader xmlrd, BaseNode parentNode)
        {
            //            if (xmlrd == null || parentNode == null) return;
            BaseNode child;
            xmlrd.Read();
            while (xmlrd.NodeType != XmlNodeType.EndElement)
            {
                bool noEndElement = xmlrd.IsEmptyElement;
                // We have a child.
                child = parentNode.FindNodeInChildren(xmlrd.LocalName);
                if (child == null)
                {   // Child name not found in node tree. Create a new node.
                    // Look for type attribute
                    String nodeName = xmlrd.LocalName;
                    if (xmlrd.MoveToAttribute(Attributes.Name(AttributeTypes.type)))
                    {
                        child = NodeFactory.CreateNode(xmlrd.Value, nodeName);
                    }
                    else
                    { // Default is a basic node.
                        child = new BaseNode(nodeName);
                    }
                    if (child != null) parentNode.AddChild(child);
                }
                if (child == null) return;

                UpdateNodeAttributes(xmlrd, child);
                if (noEndElement)
                {
                    xmlrd.Read(); // Read next element and continue.
                }
                else
                {
                    UpdateChilds(xmlrd, child);
                    xmlrd.Read(); // Read end element.
                }
            }
        }
        void UpdateNodeAttributes(XmlReader xmlrd, BaseNode node)
        {
            bool moreAttributes = xmlrd.MoveToFirstAttribute();
            while (moreAttributes)
            {
                AttributeTypes attrID = Attributes.GetAttribute(xmlrd.Name);
                if (attrID != AttributeTypes.unknown)
                {
                    bool success = node.SetAttribute(attrID, xmlrd.Value);
                    if (!success)
                    {
                        if (attrID == AttributeTypes.type)
                        {
                            NodeTypes nodeType = NodeTypes.nodeNode;
                            bool typeIsKnown = NodeFactory.GetNodeType(xmlrd.Value, ref nodeType);
                            if (typeIsKnown && nodeType != NodeFactory.NodeType(node))
                            {
                                // Error: Tried to change the type of an existing node.
                            }
                        }
                        // Unhandled attribute found in xml.
                    }
                }
                else
                {
                    // Unhandled attribute found in xml.
                }
                moreAttributes = xmlrd.MoveToNextAttribute();
            }
        }
    }
}
