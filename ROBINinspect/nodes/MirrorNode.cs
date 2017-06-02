using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{
    class MirrorNode : BaseNode
    {
        public MirrorNode(String nodeName, String path, BaseNode parent = null)
            : base(nodeName, parent)
        {
            mirrorPath = path;
        }

        private String mirrorPath;
        public String MirrorPath
        {
            get { return mirrorPath; }
        }

        private BaseNode mirrorSource;
        public BaseNode MirrorSource
        {
            get { return mirrorSource; }
        }
    }
}
