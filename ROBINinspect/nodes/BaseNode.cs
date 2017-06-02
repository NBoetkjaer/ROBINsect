using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROBINinspect
{
    class BaseNode
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

        protected BaseNode parentNode;
        public BaseNode ParentNode
        {
            get { return parentNode; }
        }

        public BaseNode(string nodeName, BaseNode parent = null)
        {
            name = nodeName;
            parentNode = parent;
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

        public void AddNode(BaseNode childNode)
        {
            children.Add(childNode);
            //throw new NotImplementedException();
        }

        public BaseNode FindNode(String nodePath)
        {
            throw new NotImplementedException();
        }

        private BaseNode FindNodeInChildren(String nodeName)
        {
            if (children == null) return null;
            foreach (BaseNode node in children)
            {
                if (node.name.Equals(nodeName)) return node;
            }
            // Not found return null.
            return null;
        }

        // The argument nodePath contains the relative or absolute path to the node to search for.
        // An absolute path is specified with a leading "/" eg. "/node1/node2/nodeToFind", in which case the function will begin the search from the root node.
        // The syntax of a relative path is "node2/nodeToFind" or "../node1/node2/nodeToFind" and the search is started from this node.
        // Return value: If the node is found a pointer to the requested node is returned, otherwise null is returned.
        protected BaseNode FindNodeInternal(String nodePath, int strPos, bool allowPartialMatch, bool resolveMirrors)
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
            if (nodePath.Length == strPos) return this;
            // Special case if first character is a pathDelimiter we should start the search from the root node.
            if (nodePath[0] == pathDelimiter)
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
                // Otherwise search children for at string match with next node.
                BaseNode pPartialMatchNode = null;
                int partialMatchPos = -1;
                foreach(BaseNode child in  children)
                {
                    //const char* pChildName = child->name.c_str();
                    //const char* pNodeName = pNodePath;

                    //while (*pNodeName == *pChildName && *pChildName != 0 && *pNodeName != 0 && *pNodeName != pathDelimiter)
                    //{
                    //    pNodeName++;
                    //    pChildName++;
                    //}
                    //// did we match to the end of child name?
                    //if (*pChildName == 0)
                    //{
                    //    if (*pNodeName == pathDelimiter) { return child->FindNodeInternal(pNodeName + 1, allowPartialMatch, resolveMirrors);}
                    //    if (*pNodeName == 0) 
                    //    {
                    //        if (resolveMirrors && (typeid(*(child.get())) == typeid(MirrorNode)))
                    //        {
                    //            MirrorNode * pMirror = dynamic_cast<MirrorNode*> (child.get());
                    //            if (pMirror == nullptr || pMirror->GetMirrorSource() == nullptr)
                    //            {
                    //                // ToDo: Handle error.
                    //                return nullptr;
                    //            }
                    //            return pMirror->GetMirrorSource();
                    //        }
                    //        return child.get();
                    //    }
                    //}
                    //if (allowPartialMatch) // If partial match is allowed checke that the specified named is matched entirely.
                    //{
                    //    if (*pNodeName == pathDelimiter || *pNodeName == 0)
                    //    {
                    //        if (pPartialMatchNode) { return nullptr; } // ambiguity - More than one child node have a partial match.
                    //        pPartialMatchNode = child.get();
                    //        pPartialMatchPos = pNodeName;
                    //    }
                    //}
                }


            return null;
        }

        /*        if (resolveMirrors && (typeid(*this) == typeid(MirrorNode)))
                {
                    MirrorNode * pMirror = dynamic_cast<MirrorNode*> (this);
                    if (pMirror == nullptr || pMirror->GetMirrorSource() == nullptr)
                    {
                        // ToDo: Handle error.
                        return nullptr;
                    }
                    return pMirror->GetMirrorSource()->FindNodeInternal(pNodePath, allowPartialMatch, resolveMirrors);
                }
                static const char pathDelimiter = '/';
                if (pNodePath == nullptr) return nullptr;
                // Special case for an empty string - just return this.
                if (pNodePath[0] == 0) return this;
                // Special case if first character is a pathDelimiter we should start the search from the root node.
                if (pNodePath[0] == pathDelimiter)
                {
                    BaseNode* pNode = this;
                    // Traverse to root.
                    while (pNode->pParent)
                    {
                        pNode = pNode->pParent;
                    }
                    return pNode->FindNodeInternal(pNodePath + 1, allowPartialMatch, resolveMirrors);
                }
                // Special case if first two characters are ".." - then return the parent if not null.
                if (pNodePath[0] == '.' && pNodePath[1] != 0 && pNodePath[1] == '.' && pParent)
                {
                    if (pNodePath[2] == pathDelimiter) return pParent->FindNodeInternal(pNodePath + 3, allowPartialMatch, resolveMirrors);
                    if (pNodePath[2] == 0) return pParent;
                }

                // Otherwise lookup (next) node name in nodePath.
                // Otherwise search children for at string match with next node.
                BaseNode* pPartialMatchNode = nullptr;
                const char* pPartialMatchPos = nullptr;
                for (const auto &child : children)
                {
                    const char* pChildName = child->name.c_str();
                    const char* pNodeName = pNodePath;

                    while (*pNodeName == *pChildName && *pChildName != 0 && *pNodeName != 0 && *pNodeName != pathDelimiter)
                    {
                        pNodeName++;
                        pChildName++;
                    }
                    // did we match to the end of child name?
                    if (*pChildName == 0)
                    {
                        if (*pNodeName == pathDelimiter) { return child->FindNodeInternal(pNodeName + 1, allowPartialMatch, resolveMirrors);}
                        if (*pNodeName == 0) 
                        {
                            if (resolveMirrors && (typeid(*(child.get())) == typeid(MirrorNode)))
                            {
                                MirrorNode * pMirror = dynamic_cast<MirrorNode*> (child.get());
                                if (pMirror == nullptr || pMirror->GetMirrorSource() == nullptr)
                                {
                                    // ToDo: Handle error.
                                    return nullptr;
                                }
                                return pMirror->GetMirrorSource();
                            }
                            return child.get();
                        }
                    }
                    if (allowPartialMatch) // If partial match is allowed checke that the specified named is matched entirely.
                    {
                        if (*pNodeName == pathDelimiter || *pNodeName == 0)
                        {
                            if (pPartialMatchNode) { return nullptr; } // ambiguity - More than one child node have a partial match.
                            pPartialMatchNode = child.get();
                            pPartialMatchPos = pNodeName;
                        }
                    }
                }
                if (allowPartialMatch && pPartialMatchNode != nullptr)
                {
                    if (*pPartialMatchPos == 0) 
                    { 
                        if (resolveMirrors && (typeid(*pPartialMatchNode) == typeid(MirrorNode)))
                        {
                            MirrorNode * pMirror = dynamic_cast<MirrorNode*> (pPartialMatchNode);
                            if (pMirror == nullptr || pMirror->GetMirrorSource() == nullptr)
                            {
                                // ToDo: Handle error.
                                return nullptr;
                            }
                            return pMirror->GetMirrorSource();
                        }
                        return pPartialMatchNode; 
                    }
                    return pPartialMatchNode->FindNodeInternal(pPartialMatchPos + 1, allowPartialMatch, resolveMirrors); // (+1) It must have been a delimiter. Move one pos forward.
                }
                return nullptr;
            }
                */
    }
}
