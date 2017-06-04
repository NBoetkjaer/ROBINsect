using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{

    class Pos3D_32f_Node: StringNode
    {
        FloatNode nodeX;
        FloatNode nodeY;
        FloatNode nodeZ;

        public Pos3D_32f_Node(String nodeName, float x = 0.0f, float y = 0.0f, float z = 0.0f)
            : base(nodeName)
        {
            nodeX = new FloatNode("X", x);
            nodeY = new FloatNode("Y", y);
            nodeZ = new FloatNode("Z", z);
            AddChild(nodeX);
            AddChild(nodeY);
            AddChild(nodeZ);
        }

        protected override void GetValue(ref String strValue)
        {
            String strVal = "[";
            nodeX.GetAttribute(AttributeTypes.value, ref strVal);
            strValue += strVal + optionDelimiter;
            nodeY.GetAttribute(AttributeTypes.value, ref strVal);
            strValue += strVal + optionDelimiter;
            nodeZ.GetAttribute(AttributeTypes.value, ref strVal);
            strValue += strVal + "]";
        }
        protected override void SetValue(String strValue)
        {
            string[] arrStr = strValue.Split(optionDelimiter);
            if (arrStr.Length != 3) return;
            arrStr[0] = arrStr[0].Replace("[", String.Empty);
            arrStr[2] = arrStr[2].Replace("]", String.Empty);
            float tmpX, tmpY, tmpZ;
            if (float.TryParse(arrStr[0], NumberStyles.Float, DecimalFormat.FormatInfo, out tmpX)
                &&
                float.TryParse(arrStr[1], NumberStyles.Float, DecimalFormat.FormatInfo, out tmpY)
                &&
                float.TryParse(arrStr[2], NumberStyles.Float, DecimalFormat.FormatInfo, out tmpZ)
                )
            {
                SetPosition(tmpX, tmpY, tmpZ);
            }
        }
        public void GetPosition(out float x,out float y, out float z) 
        {
            x = nodeX.Value;
            y = nodeY.Value;
            z = nodeZ.Value;
        }

        void SetPosition(float x, float y, float z)
        {
            nodeX.Value = x;
            nodeY.Value = y;
            nodeZ.Value = z;
        }
    }
}
