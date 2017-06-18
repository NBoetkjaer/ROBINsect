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
        public void ConvertToXml(BaseNode root, ref String strXml, FlagType flagMask = FlagType.none, bool onlyChanges = false)
        {
            StringWriter strWriter = new StringWriter();
            XmlWriter xmlwr = new XmlTextWriter(strWriter);

            xmlwr.WriteStartElement(root.Name);
            AddChilds(xmlwr, root, flagMask, onlyChanges);
            xmlwr.WriteEndElement();
            xmlwr.Close();
            strXml = strWriter.ToString();
        }

        void AddChilds(XmlWriter xmlwr, BaseNode parentNode, FlagType flagMask, bool onlyChanges)
        {
            foreach(BaseNode child in parentNode.Children)
            {
                if (onlyChanges && !child.AnyChanges())
                {
                    continue;
                }
                xmlwr.WriteStartElement(child.Name);
                BaseNode childNode = child;
                if (resolveMirrors && (child.GetType() == typeof(MirrorNode)))
                {
                    MirrorNode mirror = (child as MirrorNode);
                    if (mirror.IsLinked())
                    {
                        childNode = mirror.MirrorSource;
                    }
                }
                AddNodeAttributes(xmlwr, childNode, onlyChanges);
                if (childNode.Children.Count > 0)
                {
                    AddChilds(xmlwr, childNode, flagMask, onlyChanges);
                }
                xmlwr.WriteEndElement();
            }
        }

        void AddNodeAttributes(XmlWriter xmlwr, BaseNode node, bool onlyChanges)
        {
            if (!onlyChanges)
            {
                // Add type attribute.
                xmlwr.WriteAttributeString(Attributes.Name(AttributeTypes.type), NodeFactory.TypeAsString(NodeFactory.NodeType(node)));
            }
            foreach (AttributeTypes attr in Attributes.ValidAttributes)
            {
                if(node.IsAttributeUsed(attr))
                {
                    if(onlyChanges && !node.IsAttributeChanged(attr))
                    {
                        continue;
                    }
                    String attrValue = String.Empty;
                    if(node.GetAttribute(attr, ref attrValue))
                    {
                        xmlwr.WriteAttributeString(Attributes.Name(attr), attrValue);
                    }
                }
            }
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

        bool resolveMirrors = true;

        void UpdateChilds(XmlReader xmlrd, BaseNode parentNode)
        {
            //            if (xmlrd == null || parentNode == null) return;
            BaseNode child;
            xmlrd.Read();
            while (xmlrd.NodeType != XmlNodeType.EndElement)
            {
                bool noEndElement = xmlrd.IsEmptyElement;
                // We have a child.
                child = parentNode.FindNode(xmlrd.LocalName, false, resolveMirrors);
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
                    // Just an idea to accept new mirror nodes to be created dynamically.
                    //if(resolveMirrors && child is MirrorNode)
                    //{
                    //    if(!(child as MirrorNode).LinkMirror())
                    //    {
                    //        // Error - unable to link new mirror node.
                    //    }
                    //}
                }
                if (child == null) return;

                UpdateNodeAttributes(xmlrd, child);
                if (noEndElement)
                {
                    xmlrd.Read(); // This child has no childs itself - continue in while loop.
                }
                else
                {
                    UpdateChilds(xmlrd, child); // recursively update childs.
                    xmlrd.Read(); // Read the end element.
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
