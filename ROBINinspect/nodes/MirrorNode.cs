using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{
    public class MirrorNode : BaseNode
    {
        public MirrorNode(String nodeName, String path = "")
            : base(nodeName)
        {
            mirrorPath = path;
        }

        private String mirrorPath;
        public String MirrorPath
        {
            get { return mirrorPath; }
            set
            {
                mirrorPath = value;
                SetAttributeChanged(AttributeTypes.path);
            }
        }

        private BaseNode mirrorSource;
        public BaseNode MirrorSource
        {
            get { return mirrorSource; }
        }

        public bool IsLinked() { return mirrorSource != null; }
        public bool LinkMirror()
        {
            BaseNode mirror = FindNode(mirrorPath, false, false);
            SetMirrorSource(mirror);
            return IsLinked();
        }

        void SetMirrorSource(BaseNode source)
        {
            if (source == null)
            {
                if(mirrorSource != null) 
                {   // Remove old link.
                    mirrorSource.RemoveMirror(this);
                    mirrorSource = null;
                }
                return;
            }

            if(mirrorSource != null &&  mirrorSource != source)
            {  // Remove old link.
                mirrorSource.RemoveMirror(this);
            }

            // Link to new source.
            mirrorSource = source; 
            mirrorSource.AddMirror(this);
        }

        public override bool GetAttribute(AttributeTypes attribID, ref String strAttributeValue)
        {
            if (attribID == AttributeTypes.path)
            {
                strAttributeValue = MirrorPath;
                return true;
            }
            if (mirrorSource == null)
            {
                return false;
            }
            else
            {
                return mirrorSource.GetAttribute(attribID,ref strAttributeValue);
            }
        }

        public override bool SetAttribute(AttributeTypes attribID, String strAttributeValue)
        {
            if (attribID == AttributeTypes.path)
            {
                MirrorPath = strAttributeValue;
                return true;
            }
            if (mirrorSource == null)
            {
                return false;
            }
            else
            {
                return mirrorSource.SetAttribute(attribID, strAttributeValue);
            }
        }

        public override bool IsAttributeChanged(AttributeTypes attribID)
        {
            if (attribID == AttributeTypes.path)
            {
                return base.IsAttributeChanged(attribID);
            }
            if (mirrorSource == null)
            {
                return false; 
            }
            else
            {
                return mirrorSource.IsAttributeChanged(attribID);
            }
        }

        public override bool IsAttributeUsed(AttributeTypes attribID)
        {
            if (attribID == AttributeTypes.path)
            {
                return base.IsAttributeUsed(attribID);
            }
            if (mirrorSource == null)
            {
                return false;
            }
            else
            {
                return mirrorSource.IsAttributeUsed(attribID);
            }
        }
    }
}
