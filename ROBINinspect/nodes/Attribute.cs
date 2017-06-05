using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{
    public enum AttributeTypes { flags, type, value, range, info, options, unit, prefix, path, lastAttribute, unknown = lastAttribute};

    internal abstract class Attributes
    {
        private static String[] attributeNames;        
        public static String Name(AttributeTypes type)
        {
            if (type < 0 || type >= AttributeTypes.lastAttribute) return String.Empty;
            if (attributeNames == null)
            {
                Array enumArr = Enum.GetValues(typeof(AttributeTypes));
                attributeNames = new String[enumArr.Length-2]; //-2:  not last and unknown.
                foreach (AttributeTypes eAttributeType in enumArr)
                {
                    if (eAttributeType == AttributeTypes.lastAttribute) break;
                    attributeNames[(int)eAttributeType] = eAttributeType.ToString();
                }
            }
            return attributeNames[(int)type];
        }

        public static AttributeTypes GetAttribute(String strAttrib)
        {
            int i = 0;
            foreach(String attr in attributeNames)
            {
                if (attr.Equals(strAttrib))
                {
                    return (AttributeTypes)i;
                }
                i++;
            }
            return AttributeTypes.unknown;
        }
    }
}
