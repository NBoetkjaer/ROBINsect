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
        /*readonly  = 1 << 1, // keyword ??*/
        logging     = 1 << 2,
        persist     = 1 << 3,
        query       = 1 << 4
    };
}
