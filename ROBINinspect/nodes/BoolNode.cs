using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{
    class BoolNode : AbstractValueNode
    {
        public BoolNode(String nodeName, Boolean value = false)
            : base(nodeName)
        {
            Value = value; // Use property to assign val - performs range check and mark any changes.
        }

        public override bool GetAttribute(AttributeTypes attribID, ref String strAttributeValue)
        {
            switch (attribID)
            {
                case AttributeTypes.value:
                    GetValue(ref strAttributeValue);
                    break;
                case AttributeTypes.options:
                    GetOptions(ref strAttributeValue, options, optionDelimiter);
                    break;
                default:
                    return base.GetAttribute(attribID, ref strAttributeValue);
            }
            return true;
        }

        public override bool SetAttribute(AttributeTypes attribID, String strAttributeValue)
        {
            switch (attribID)
            {
                case AttributeTypes.value:
                    SetValue(strAttributeValue);
                    break;
                case AttributeTypes.options:
                    string[] tmpOtions;
                    SetOptions( strAttributeValue, out tmpOtions, optionDelimiter);
                    if (tmpOtions.Length == 2)
                    {
                        options = tmpOtions;
                        SetAttributeChanged(AttributeTypes.options);
                    }
                    break;
                default:
                    return base.SetAttribute(attribID, strAttributeValue);
            }
            return true;
        }

        #region value attribute
        private Boolean val;
        public virtual Boolean Value
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
            if (options.Length != 2) return;
            strValue = val ? options[1] : options[0];
        }
        protected override void SetValue(String strValue)
        {
            int optVal;
            SetOptionValue(strValue, options, out optVal);
            if (optVal >= 0)
            {
                Value = (optVal != 0);
            }
        }
        #endregion

        #region option attribute
        static String[] defaultOptions = { "false", "true" };
        String[] options = defaultOptions;

        #endregion
    }
}
