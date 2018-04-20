using System;
using System.Globalization;

namespace ROBINinspect
{
    public abstract class AbstractValueNode : BaseNode
    {
        protected String unit;
        protected static char rangeDelimiter = ',';
        public AbstractValueNode(String nodeName)
            : base(nodeName)
        {
        }

        public override bool GetAttribute(AttributeTypes attribID, ref String strAttributeValue)
        {
            switch (attribID)
            {
                case AttributeTypes.value:
                    GetValue(ref strAttributeValue);
                    break;
                case AttributeTypes.unit:
                    strAttributeValue = unit;
                    break;
                case AttributeTypes.prefix:
                    return false;
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
                case AttributeTypes.unit:
                    unit = strAttributeValue;
                    SetAttributeChanged(AttributeTypes.unit);
                    break;
                case AttributeTypes.prefix:
                    return false;
                //break;
                default:
                    return base.SetAttribute(attribID, strAttributeValue);
            }
            return true;
        }

        protected abstract void GetValue(ref String strValue);
        protected abstract void SetValue(String strValue);
        bool IsValueChanged() { return IsAttributeChanged(AttributeTypes.value); }

        #region static helper functions for options attributes
        public const char optionDelimiter = ',';
        static protected void GetOptions(ref String strOptions, String[] options, char delimiter)
        {
            strOptions = String.Empty;
            bool first = true;
            foreach (string opt in options)
            {
                if (!first)
                {
                    strOptions += delimiter;
                }
                strOptions += opt;
                first = false;
            }
        }

        static protected void SetOptions(String strOptions, out String[] options, char delimiter)
        {
            options = strOptions.Split(delimiter);
        }

        static protected void SetOptionValue(String strValue, String[] options, out int optVal)
        {
            int i = 0;
            foreach (String opt in options)
            {
                if (opt.Equals(strValue))
                {
                    optVal = i;
                    return;
                }
                i++;
            }
            optVal = -1;
        }
        #endregion
    }


    internal class DecimalFormat
    {
        private static NumberFormatInfo instance;
        public static NumberFormatInfo FormatInfo
        {
            get
            {
                if (instance == null)
                {
                    instance = new NumberFormatInfo();
                    instance.NumberDecimalSeparator = ".";
                }
                return instance;
            }
        }
    }
    public abstract class AbstractNumericNode: AbstractValueNode
    {        
        public AbstractNumericNode(String nodeName)
            : base(nodeName)
        {
        }

        public override bool GetAttribute(AttributeTypes attribID, ref String strAttributeValue)
        {
            switch (attribID)
            {
                case AttributeTypes.range:
                    GetRange(ref strAttributeValue);
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
                case AttributeTypes.range:
                    SetRange(strAttributeValue);
                    break;
                default:
                    return base.SetAttribute(attribID, strAttributeValue);
            }
            return true;
        }
        protected abstract void GetRange(ref String strValue);
        protected abstract void SetRange(String strValue);
    }
}
