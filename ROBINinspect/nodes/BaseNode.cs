using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{
    public class BaseNode
    {
        protected String name;
        public String Name
        {
            get { return name; }
        }

        protected List<BaseNode> children = new List<BaseNode>();
        public IReadOnlyCollection<BaseNode> Children
        {
            get { return children.AsReadOnly(); }
        }

        public IEnumerable<BaseNode> ChildrenIterator()
        {
            int numChilds = children.Count;
            foreach(BaseNode ownChild in children)
            { 
                yield return ownChild;
            }
            if (this.GetType() == typeof(MirrorNode))
            {
                
                foreach (BaseNode mirrorChild in (this as MirrorNode).MirrorSource.children)
                { 
                    yield return mirrorChild; 
                }
            }
        }


        protected BaseNode parentNode;
        public BaseNode ParentNode
        {
            get { return parentNode; }
        }

        public BaseNode(string nodeName, BaseNode parent = null)
        {
            name = nodeName;
            parentNode = parent;
            if (parentNode != null)
                parentNode.SetNodeChanged();
        }

        public BaseNode GetRoot()
        {
            BaseNode node = this;
            // Traverse to root.
            while (node.ParentNode != null)
            {
                node = node.ParentNode;
            }
            return node;
        }

        #region Attributes
        #region Flag attribute
        private FlagType flags;
        public FlagType Flags
        {
            get { return flags; }
            set
            {
                flags = value;
                SetAttributeChanged(AttributeTypes.flags);
            }
        }
        public void SetFlags(String strFlags)
        {
            // Syntax: "[+/-]flag1|[+/-]flag2" eg. "+flag1|+flag2|-flag3" set 
            // Parse string and set flags accordingly.
            const char flagDelimiter = '|';
            string[] arrFlags = strFlags.Split(flagDelimiter);
            foreach (String strFlag in arrFlags)
            {
                // Skip leading white spaces.
                String tmp = strFlag.Trim();
                bool removeFlag = false;
                if (strFlag[0] == '-')
                {
                    removeFlag = true;
                    tmp  = tmp.TrimStart('-');
                }
                else if (strFlag[0] == '+')
                {
                    tmp = tmp.Trim('+');
                }
                foreach (FlagType flag in Enum.GetValues(typeof(FlagType)))
                {
                    if (tmp.Equals(flag.ToString()))
                    {
                        SetFlag(flag, !removeFlag);
                    }
                }
            }
        }

        public void SetFlag(FlagType flag, bool value, bool recursive = false)
        {
            FlagType oldFlags = flags;
            if (value)
            {   // Add flag.
                flags |= flag;
            }
            else
            {   // Remove flag
                flags &= (~flag);
            }
            if (oldFlags != flags)
            {
                SetAttributeChanged(AttributeTypes.flags); // Mark the change.;
            }
            if (recursive)
            {
                foreach (BaseNode child in children)
                {
                    child.SetFlag(flag, value, recursive);
                }
            }
        }
        void GetFlags(ref string strFlags)
        {
            strFlags = String.Empty;
            foreach (FlagType flag in Enum.GetValues(typeof(FlagType)))
            {
                if ((flag & flags) != FlagType.none)
                {
                    if (!(strFlags.Length == 0))
                    {
                        strFlags += "|";
                    }
                    strFlags += flag.ToString();
                }
            }
        }
        #endregion

        private String info;
        public String Info
        {
            get { return info; }
            set
            {
                info = value;
                SetAttributeChanged(AttributeTypes.info);
            }
        }

        // -------- Attributes --------
        // Get the string representation of a given attribute.
        public virtual bool GetAttribute(AttributeTypes attribID, ref String strAttributeValue)
        {
            switch (attribID)
            {
                case AttributeTypes.flags:
                    GetFlags(ref strAttributeValue);
                    break;
                case AttributeTypes.info:
                    strAttributeValue = info;
                    break;
                default:
                    return false; // Unknown attribute return false.
            }
            return true;
        }

        // Sets the given attribute, based on the string argument.
        // Inherited nodes should call and return a base implementation if it does not handle the attributeID.
        // Returns true is the attribute is handled. 
        public virtual bool SetAttribute(AttributeTypes attribID, String strAttributeValue)
        {
            switch (attribID)
            {
                case AttributeTypes.flags:
                    SetFlags(strAttributeValue);
                    break;
                case AttributeTypes.info:
                    Info = strAttributeValue;
                    break;
                default:
                    return false; // Unknown attribute return false.
            }
            return true;
        }

        private UInt32 attributeChanges;
        private UInt32 recentChanges;
        private UInt32 touchedAttributes;
        // Mark a given attribute as changed.
        public void SetAttributeChanged(AttributeTypes attribID)
        {
            if (attributeChanges == 0)
            {   // Notify the node tree that a change has been applied.
               SetNodeChanged();
            }
            attributeChanges |= (1U << (int)attribID);
            touchedAttributes |= attributeChanges;
        }
        public virtual bool IsAttributeChanged(AttributeTypes attribID) { return (attributeChanges & (1 << (int)attribID)) != 0; }
        public virtual bool IsAttributeUsed(AttributeTypes attribID) { return (touchedAttributes & (1 << (int)attribID)) != 0; }

        void SetNodeChanged()
        {
            if (!AnyRecentChanges())
            {
                recentChanges |= 1; // Set LSB to indicate a new change.
                if (parentNode != null)
                {
                    parentNode.SetNodeChanged(); // Proceed to set parent as changed.
                }
                //for (auto mirror: mirrors)
                //{
                //    mirror->SetNodeChanged(); // Mark all the mirrors (if any) as changed.
                //}
            }
        }
        #endregion

        public bool AnyChanges()  { return recentChanges != 0; }
        public bool AnyRecentChanges() { return (recentChanges & 1) == 1; }
        public int RecentChangeCount() { return (int)((recentChanges >> 2) + (recentChanges & 1)); }

        public void AddChild(BaseNode childNode)
        {
            if (Children.Contains(childNode)) return; // No duplicates allowed.
            // ToDo: Validate node name (must be unique and no special characters which are not allowed in an XML tag name)
            // Problem regarding mirrors: it's impossible to ensure a unique node name before mirros are linked.
            children.Add(childNode);
            childNode.parentNode = this;
            SetNodeChanged();
        }

        public BaseNode FindNodeInChildren(String nodeName)
        {
            //if (children == null) return null;
            foreach (BaseNode node in ChildrenIterator())
            {
                if (node.name.Equals(nodeName)) return node;
            }
            // Not found return null.
            return null;
        }

        public BaseNode FindNode(String nodePath, bool allowPartialMatch = false, bool resolveMirrors = true)
        {
            return FindNodeInternal(nodePath, 0, allowPartialMatch, resolveMirrors);
        }
        // The argument nodePath contains the relative or absolute path to the node to search for.
        // An absolute path is specified with a leading "/" eg. "/node1/node2/nodeToFind", in which case the function will begin the search from the root node.
        // The syntax of a relative path is "node2/nodeToFind" or "../node1/node2/nodeToFind" and the search is started from this node.
        // Return value: If the node is found a pointer to the requested node is returned, otherwise null is returned.
        private BaseNode FindNodeInternal(String nodePath, int strPos, bool allowPartialMatch , bool resolveMirrors)
        {
            if (resolveMirrors && (this.GetType() == typeof(MirrorNode)))
            {
                MirrorNode mirror = this as MirrorNode;
                if (mirror == null || mirror.MirrorSource == null)
                {
                    // ToDo: Handle error.
                    return null;
                }
                return mirror.MirrorSource.FindNodeInternal(nodePath, strPos, allowPartialMatch, resolveMirrors);
            }
            const char pathDelimiter = '/';
            if (nodePath == null) return null;
            // Special case for an empty string - just return this.
            if (nodePath.Length <= strPos) return this;
            // Special case if first character is a pathDelimiter we should start the search from the root node.
            if (strPos == 0 && nodePath[0] == pathDelimiter)
            {
                BaseNode node = GetRoot();
                return node.FindNodeInternal(nodePath, 1, allowPartialMatch, resolveMirrors);
            }
            // Special case if first two characters are ".." - then return the parent if not null.
            if (nodePath[strPos] == '.' && strPos <= nodePath.Length && nodePath[strPos + 1] == '.' && ParentNode != null)
            {
                if (strPos + 2 == nodePath.Length) return ParentNode;
                if (nodePath[strPos + 2] == pathDelimiter) return parentNode.FindNodeInternal(nodePath, strPos + 3, allowPartialMatch, resolveMirrors);
            }
            // Otherwise lookup (next) node name in nodePath.
            int sepPos = nodePath.IndexOf(pathDelimiter, strPos);
            if (sepPos == -1)
            {
                sepPos = nodePath.Length;
            }
            string nodeName = nodePath.Substring(strPos, sepPos - strPos);
            // Otherwise search children for at string match with next node.
            BaseNode partialMatchNode = null;
            foreach(BaseNode child in  children)
            {
                if(allowPartialMatch)
                {
                    if (child.name.StartsWith(nodeName))
                    {
                        if (partialMatchNode != null)
                        {  // ambiguity - More than one child node have a partial match.
                            return null;
                        }
                        partialMatchNode = child;
                    }
                }
                else
                {
                    if (child.name.Equals(nodeName))
                    {
                        return child.FindNodeInternal(nodePath, sepPos+1, allowPartialMatch, resolveMirrors);
                    }
                }
            }

            if (allowPartialMatch && partialMatchNode != null)
            {
                if (sepPos >= nodePath.Length)
                {
                    if (resolveMirrors && (partialMatchNode.GetType() == typeof(MirrorNode)))
                    {
                        MirrorNode mirror = partialMatchNode as MirrorNode;
                        if (mirror == null || mirror.MirrorSource == null)
                        {
                            // ToDo: Handle error.
                            return null;
                        }
                        return mirror.MirrorSource;
                    }
                    return partialMatchNode;
                }
                return partialMatchNode.FindNodeInternal(nodePath, sepPos + 1, allowPartialMatch, resolveMirrors); // (+1) It must have been a delimiter. Move one pos forward.
            }
            return null;
        }

        #region Mirrors
        List<MirrorNode> mirrors = new List<MirrorNode>();
        public void AddMirror(MirrorNode mirror)
        {
            // Check if the mirror is already in the list.
            if (mirrors.Contains(mirror))
                return;
            // Add mirror to list of mirrors.
            mirrors.Add(mirror);
        }

        public void RemoveMirror(MirrorNode mirror)
        {
            mirrors.Remove(mirror);
        }

        private bool LinkAllMirrors()
        {
            bool success = true;
            MirrorNode mirror = this as MirrorNode;
            if (mirror != null) // Check if this node is a mirror.
            {
                success &= mirror.LinkMirror();
            }
            // Recursively try to link any mirrors.
            foreach(BaseNode child in children)
            {
                success &= child.LinkAllMirrors();
            }
            return success;
        }


        #endregion

    }
}
