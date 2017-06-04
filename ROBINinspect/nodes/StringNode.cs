using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{
    public class StringNode: AbstractValueNode
    {
        public StringNode(String nodeName, String value = "")
            : base(nodeName)
        {
            Value = value; // Use property to assign val - performs range check and mark any changes.
        }

        #region value attribute
        private String val;
        public virtual String Value
        {
            get { return val; }
            set
            {
                val = value;
                SetAttributeChanged(AttributeTypes.value);
            }
        }
        protected override void GetValue(ref String strValue)
        {
            strValue = val;
        }
        protected override void SetValue(String strValue)
        {
            Value = strValue;
        }
        #endregion
    }
}
