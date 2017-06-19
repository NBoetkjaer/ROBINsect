using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{

    [Flags]
    public enum FlagType
    {
        none        = 0,
        hide        = 1 << 0,
        _readonly  = 1 << 1, // readonly is a keyword in C#
        logging     = 1 << 2,
        persist     = 1 << 3,
        query       = 1 << 4
    };

    public static class Flag
    {
        public static String Name(FlagType flag)
        {
            switch (flag)
            {
                case FlagType._readonly:
                    return "readonly";
                default:
                    return flag.ToString();
            }
        }
    }
}
