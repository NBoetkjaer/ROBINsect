using System;
using System.Globalization;

namespace ROBINinspect
{
    class FloatNode : AbstractNumericNode
    {
        public FloatNode(String nodeName, float value = 0f)
            : base(nodeName)
        {
            Value = value; // Use property to assign val - performs range check and mark any changes.
        }

        #region value attribute
        private float  val;
        public virtual float Value
        {
            get { return val; }
            set
            {
                float v = Math.Max(Math.Min(value, maxValue), minValue);
                if (val == v) return;
                val = v;
                SetAttributeChanged(AttributeTypes.value);
            }
        }
        protected override void GetValue(ref String strValue)
        {
            strValue = val.ToString("R", DecimalFormat.FormatInfo);
        }
        protected override void SetValue(String strValue)
        {
            float tmp;
            if (float.TryParse(strValue, NumberStyles.Float, DecimalFormat.FormatInfo, out tmp))
                Value = tmp;
        }
        #endregion
        #region range attribute
        private float minValue = float.MinValue;
        private float maxValue = float.MaxValue;
        public void SetRange(float min, float max)
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

        public void GetRange(out float min, out float max)
        {
            min = minValue;
            max = maxValue;
        }

        protected override void GetRange(ref String strValue)
        {
            strValue = "[" + minValue.ToString("R", DecimalFormat.FormatInfo) + rangeDelimiter + maxValue.ToString("R", DecimalFormat.FormatInfo) + "]";
        }

        protected override void SetRange(String strValue)
        {
            string[] arrStr = strValue.Split(rangeDelimiter);
            if (arrStr.Length != 2) return;
            arrStr[0] = arrStr[0].Replace("[", String.Empty);
            arrStr[1] = arrStr[1].Replace("]", String.Empty);
            float min, max;
            if (float.TryParse(arrStr[0], NumberStyles.Float, DecimalFormat.FormatInfo, out min)
                &&
                float.TryParse(arrStr[1], NumberStyles.Float, DecimalFormat.FormatInfo, out max))
            {
                SetRange(min, max);
            }
        }
        #endregion
    }

    class DoubleNode : AbstractNumericNode
    {
        public DoubleNode(String nodeName, double value = 0)
            : base(nodeName)
        {
            Value = value; // Use property to assign val - performs range check and mark any changes.
        }

        #region value attribute
        private double val;
        public virtual double Value
        {
            get { return val; }
            set
            {
                double v = Math.Max(Math.Min(value, maxValue), minValue);
                if (val == v) return;
                val = v;
                SetAttributeChanged(AttributeTypes.value);
            }
        }
        protected override void GetValue(ref String strValue)
        {
            strValue = val.ToString("R", DecimalFormat.FormatInfo);
        }
        protected override void SetValue(String strValue)
        {
            double tmp;
            if (double.TryParse(strValue, NumberStyles.Float, DecimalFormat.FormatInfo, out tmp))
                Value = tmp;
        }
        #endregion
        #region range attribute
        private double minValue = float.MinValue;
        private double maxValue = float.MaxValue;
        public void GetRange(out double min, out double max)
        {
            min = minValue;
            max = maxValue;
        }
        public void SetRange(double min, double max)
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
            strValue = "[" + minValue.ToString("R", DecimalFormat.FormatInfo) + rangeDelimiter + maxValue.ToString("R", DecimalFormat.FormatInfo) + "]";
        }

        protected override void SetRange(String strValue)
        {
            string[] arrStr = strValue.Split(rangeDelimiter);
            if (arrStr.Length != 2) return;
            arrStr[0] = arrStr[0].Replace("[", String.Empty);
            arrStr[1] = arrStr[1].Replace("]", String.Empty);
            double min, max;
            if (double.TryParse(arrStr[0], NumberStyles.Float, DecimalFormat.FormatInfo, out min)
                &&
                double.TryParse(arrStr[1], NumberStyles.Float, DecimalFormat.FormatInfo, out max))
            {
                SetRange(min, max);
            }
        }
        #endregion
    }
} // End namespace
