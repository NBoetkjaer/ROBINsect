using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{
    class UInt16Node : AbstractNumericNode
    {
        public UInt16Node(String nodeName, UInt16 value = 0)
            : base(nodeName)
        {
            Value = value; // Use property to assign val - performs range check and mark any changes.
        }

        #region value attribute
        private UInt16 val;
        public virtual UInt16 Value
        {
            get { return val; }
            set
            {
                UInt16 v = Math.Max(Math.Min(value, maxValue), minValue);
                if (val == v) return;
                val = v;
                SetAttributeChanged(AttributeTypes.value);
            }
        }
        protected override void GetValue(ref String strValue)
        {
            strValue = val.ToString();
        }
        protected override void SetValue(String strValue)
        {
            UInt16 tmp;
            if (UInt16.TryParse(strValue, NumberStyles.Integer, DecimalFormat.FormatInfo, out tmp))
                Value = tmp;
        }
        #endregion
        #region range attribute
        private UInt16 minValue = UInt16.MinValue;
        private UInt16 maxValue = UInt16.MaxValue;
        public void GetRange(out UInt16 min, out UInt16 max)
        {
            min = minValue;
            max = maxValue;
        }
        public void SetRange(UInt16 min, UInt16 max)
        {
            if (min > max)
            {   // ToDo: Report error?
                return;
            }
            // Did anything change?
            if (min == minValue && max == maxValue)
            {
                return;
            }
            SetAttributeChanged(AttributeTypes.range); // Mark the change.
            minValue = min;
            maxValue = max;
            Value = val; // This will ensure that 'value' is within the new range.
        }

        protected override void GetRange(ref String strValue)
        {
            strValue = "[" + minValue.ToString() + rangeDelimiter + maxValue.ToString() + "]";
        }

        protected override void SetRange(String strValue)
        {
            string[] arrStr = strValue.Split(rangeDelimiter);
            if (arrStr.Length != 2) return;
            arrStr[0] = arrStr[0].Replace("[", String.Empty);
            arrStr[1] = arrStr[1].Replace("]", String.Empty);
            UInt16 min, max;
            if (UInt16.TryParse(arrStr[0], NumberStyles.Integer, DecimalFormat.FormatInfo, out min)
                &&
                UInt16.TryParse(arrStr[1], NumberStyles.Integer, DecimalFormat.FormatInfo, out max))
            {
                SetRange(min, max);
            }
        }
        #endregion
    }

    class UInt32Node: AbstractNumericNode
    {
        public UInt32Node(String nodeName, UInt32 value = 0)
            : base(nodeName)
        {
            Value = value; // Use property to assign val - performs range check and mark any changes.
        }

        #region value attribute
        private UInt32 val;
        public virtual UInt32  Value
        {
            get { return val; }
            set
            {
                UInt32 v = Math.Max(Math.Min(value, maxValue), minValue);
                if (val == v) return;
                val = v;
                SetAttributeChanged(AttributeTypes.value);
            }
        }
        protected override void GetValue(ref String strValue) 
        {
            strValue = val.ToString();
        }
        protected override void SetValue(String strValue)
        {
            UInt32 tmp;
            if (UInt32.TryParse(strValue, NumberStyles.Integer, DecimalFormat.FormatInfo, out tmp))
                Value = tmp;
        }
        #endregion
        #region range attribute
        private UInt32 minValue = UInt32.MinValue;
        private UInt32 maxValue = UInt32.MaxValue;
        public void SetRange(UInt32 min, UInt32 max)
        {
            if (min > max)
            {   // ToDo: Report error?
                return;
            }
            // Did anything change?
            if (min == minValue && max == maxValue)
            {
                return;
            }
            SetAttributeChanged(AttributeTypes.range); // Mark the change.
            minValue = min;
            maxValue = max;
            Value = val; // This will ensure that 'value' is within the new range.
        }

        protected override void GetRange(ref String strValue)
        {
            strValue = "[" + minValue.ToString() + rangeDelimiter + maxValue.ToString() + "]";
        }

        protected override void SetRange(String strValue)
        {
            string[] arrStr = strValue.Split(rangeDelimiter);
            if (arrStr.Length != 2) return;
            arrStr[0] = arrStr[0].Replace("[", String.Empty);
            arrStr[1] = arrStr[1].Replace("]", String.Empty);
            UInt32 min, max;
            if (UInt32.TryParse(arrStr[0], NumberStyles.Integer, DecimalFormat.FormatInfo, out min)
                &&
                UInt32.TryParse(arrStr[1], NumberStyles.Integer, DecimalFormat.FormatInfo, out max))
            {
                SetRange(min, max);
            }
        }
        #endregion
    }

    class Int32Node : AbstractNumericNode
    {
        public Int32Node(String nodeName, Int32 value = 0)
            : base(nodeName)
        {
            Value = value; // Use property to assign val - performs range check and mark any changes.
        }

        #region value attribute
        private Int32 val;
        public virtual Int32 Value
        {
            get { return val; }
            set
            {
                Int32 v = Math.Max(Math.Min(value, maxValue), minValue);
                if (val == v) return;
                val = v;
                SetAttributeChanged(AttributeTypes.value);
            }
        }
        protected override void GetValue(ref String strValue)
        {
            strValue = val.ToString();
        }
        protected override void SetValue(String strValue)
        {
            Int32 tmp;
            if (Int32.TryParse(strValue, NumberStyles.Integer, DecimalFormat.FormatInfo, out tmp))
                Value = tmp;
        }
        #endregion
        #region range attribute
        private Int32 minValue = Int32.MinValue;
        private Int32 maxValue = Int32.MaxValue;
        public void SetRange(Int32 min, Int32 max)
        {
            if (min > max)
            {   // ToDo: Report error?
                return;
            }
            // Did anything change?
            if (min == minValue && max == maxValue)
            {
                return;
            }
            SetAttributeChanged(AttributeTypes.range); // Mark the change.
            minValue = min;
            maxValue = max;
            Value = val; // This will ensure that 'value' is within the new range.
        }

        protected override void GetRange(ref String strValue)
        {
            strValue = "[" + minValue.ToString() + rangeDelimiter + maxValue.ToString() + "]";
        }

        protected override void SetRange(String strValue)
        {
            string[] arrStr = strValue.Split(rangeDelimiter);
            if (arrStr.Length != 2) return;
            arrStr[0] = arrStr[0].Replace("[", String.Empty);
            arrStr[1] = arrStr[1].Replace("]", String.Empty);
            Int32 min, max;
            if (Int32.TryParse(arrStr[0], NumberStyles.Integer, DecimalFormat.FormatInfo, out min)
                &&
                Int32.TryParse(arrStr[1], NumberStyles.Integer, DecimalFormat.FormatInfo, out max))
            {
                SetRange(min, max);
            }
        }
        #endregion
    }

    class Int64Node : AbstractNumericNode
    {
        public Int64Node(String nodeName, Int64 value = 0)
            : base(nodeName)
        {
            Value = value; // Use property to assign val - performs range check and mark any changes.
        }

        #region value attribute
        private Int64 val;
        public virtual Int64 Value
        {
            get { return val; }
            set
            {
                Int64 v = Math.Max(Math.Min(value, maxValue), minValue);
                if (val == v) return;
                val = v;
                SetAttributeChanged(AttributeTypes.value);
            }
        }
        protected override void GetValue(ref String strValue)
        {
            strValue = val.ToString();
        }
        protected override void SetValue(String strValue)
        {
            Int64 tmp;
            if (Int64.TryParse(strValue, NumberStyles.Integer, DecimalFormat.FormatInfo, out tmp))
                Value = tmp;
        }
        #endregion
        #region range attribute
        private Int64 minValue = Int64.MinValue;
        private Int64 maxValue = Int64.MaxValue;
        public void SetRange(Int64 min, Int64 max)
        {
            if (min > max)
            {   // ToDo: Report error?
                return;
            }
            // Did anything change?
            if (min == minValue && max == maxValue)
            {
                return;
            }
            SetAttributeChanged(AttributeTypes.range); // Mark the change.
            minValue = min;
            maxValue = max;
            Value = val; // This will ensure that 'value' is within the new range.
        }

        protected override void GetRange(ref String strValue)
        {
            strValue = "[" + minValue.ToString() + rangeDelimiter + maxValue.ToString() + "]";
        }

        protected override void SetRange(String strValue)
        {
            string[] arrStr = strValue.Split(rangeDelimiter);
            if (arrStr.Length != 2) return;
            arrStr[0] = arrStr[0].Replace("[", String.Empty);
            arrStr[1] = arrStr[1].Replace("]", String.Empty);
            Int64 min, max;
            if (Int64.TryParse(arrStr[0], NumberStyles.Integer, DecimalFormat.FormatInfo, out min)
                &&
                Int64.TryParse(arrStr[1], NumberStyles.Integer, DecimalFormat.FormatInfo, out max))
            {
                SetRange(min, max);
            }
        }
        #endregion
    }

}
