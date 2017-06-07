using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ROBINinspect
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            BaseNode rootNode = new BaseNode("root");
            BaseNode child = new BaseNode("child");
            BaseNode sibling1 = new BaseNode("sister");
            BaseNode sibling2 = new BaseNode("brother1");
            BaseNode sibling3 = new BaseNode("brother2");
            rootNode.AddChild(child);
            rootNode.AddChild(sibling1);
            rootNode.AddChild(sibling2);
            rootNode.AddChild(sibling3);
            BaseNode grandChild = new BaseNode("grandChild1");            
            child.AddChild(grandChild);
            BaseNode grandGrandChild = new BaseNode("grandGrandChild ");
            grandChild.AddChild(grandGrandChild);

            BaseNode tmp = rootNode.FindNode("child/grandChild1");
            tmp = grandChild.FindNode("../..");
            tmp = rootNode.FindNode("si/../ch/gr/..", true);
            tmp = grandChild.FindNode("/brother1");
            tmp = grandChild.FindNode("/wrongNode");
            tmp = grandChild.FindNode("//brother1");

            rootNode.SetFlags("+hide|+logging|persist|query");
            rootNode.SetAttribute(AttributeTypes.flags, "+hide|-logging|persist|-query");
            rootNode.SetAttribute(AttributeTypes.info, "Dette er en test");

            String tmpStr = String.Empty;
            UInt16Node int32Node = new UInt16Node("Int32");
            int32Node.SetAttribute(AttributeTypes.value, "123");
            int32Node.SetAttribute(AttributeTypes.range, "[3, 123]");
            int32Node.SetAttribute(AttributeTypes.range, "[-123, 123]");
            int32Node.GetAttribute(AttributeTypes.value, ref tmpStr);
            int32Node.GetAttribute(AttributeTypes.range, ref tmpStr);

            FloatNode floatNode = new FloatNode("float");
            floatNode.SetAttribute(AttributeTypes.value, "123.456");
            floatNode.SetAttribute(AttributeTypes.range, "[-123.456, 654.321]");
            floatNode.GetAttribute(AttributeTypes.value, ref tmpStr);
            floatNode.GetAttribute(AttributeTypes.range, ref tmpStr);

            DoubleNode doubleNode = new DoubleNode("double");
            doubleNode.SetAttribute(AttributeTypes.value, "123.456");
            doubleNode.SetAttribute(AttributeTypes.range, "[-123.456, 654.321]");
            doubleNode.GetAttribute(AttributeTypes.value, ref tmpStr);
            doubleNode.GetAttribute(AttributeTypes.range, ref tmpStr);

            Pos3D_32f_Node posNode = new Pos3D_32f_Node("Pos3D");
            posNode.SetAttribute(AttributeTypes.value, "[1.1, 2.2, 3.3]");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string xmlText = System.IO.File.ReadAllText(@"..\..\ROBINsect.xml");
            BaseNode rootNode = new BaseNode("root");
            NodeXmlConverter xmlConv = new NodeXmlConverter();
            xmlConv.UpdateTreeFromXml(rootNode, xmlText);

            treeView1.BeginUpdate();
            TreeNode rootTreeNode = treeView1.Nodes.Add(rootNode.Name);
            rootTreeNode.Tag = rootNode;
            AddChildrenToNodeTree(rootNode, rootTreeNode);
            treeView1.EndUpdate();
        }

        private void AddChildrenToNodeTree(BaseNode parentNode , TreeNode tnode)
        {
            foreach (BaseNode childNode in parentNode.Children)
            {
                TreeNode childTNode = tnode.Nodes.Add(childNode.Name);
                childTNode.Tag = childNode;
                if (childNode.Children.Count > 0)
                {
                    AddChildrenToNodeTree(childNode, childTNode);
                }
            }
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            dataGridView1.Rows.Clear();
            dataGridView1.ColumnCount = 2;
            if(treeView1.SelectedNode != null)
            {
                BaseNode node = treeView1.SelectedNode.Tag as BaseNode;
                if (node == null) return;
                string nodeType = NodeFactory.TypeAsString(NodeFactory.NodeType(node));
                dataGridView1.Columns[0].HeaderText = "Attribute";
                dataGridView1.Columns[0].ReadOnly = true;
                dataGridView1.Columns[1].HeaderText = nodeType;
                foreach (AttributeTypes attr in Attributes.ValidAttributes)
                {
                    if(node.IsAttributeUsed(attr))
                    {
                        string attribValue = String.Empty;
                        if(node.GetAttribute(attr, ref attribValue))
                        {
                             dataGridView1.Rows.Add(attr, attribValue);
                        }
                    }
                }
            }
        }
    }
}
