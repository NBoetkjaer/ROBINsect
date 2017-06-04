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
    }
}
