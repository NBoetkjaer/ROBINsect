#pragma once

#include "BaseNode.hpp"
#include <string>
// MirrorNode example.
// RootNode
//        +--MirrorNode1 => FloatNode
//        +--MirrorNOde2 => FloatNode
//        +--FloatNode
// RootNode has three children.
// RootNode.FindNode("MirrorNode1") should return FloatNode.
//
// When FloatNode is changed (IsChanged) so is MirrorNode 1 and 2, this means that FloatNode must be able 
// to reach both mirroring nodes. 
// BaseNode must be extended with a pointer to a mirror that will point to first mirroring node. If more nodes are
// pointing to the same node they must form a chain eg.
//    FloatNode.ptrMirror => MirrorNode1
//    MirrorNode1.ptrMirror => MirrorNode2
//    MirrorNode2.ptrMirror => nullptr;
//
// Setting/getting attributes on the MirrorNode should be forwarded to the source node.
//

extern Attribute pathAttrib;
class MirrorNode : public BaseNode
{
public:
    MirrorNode(const std::string &nodeName, const std::string &path = ""):
        BaseNode(nodeName),
        pMirrorSource(nullptr)
    {
            SetMirrorPath(path.c_str());
    }

    virtual ~MirrorNode()
    {}

    template<typename TNode = BaseNode>
    TNode* GetMirrorSource() const
    {
        return dynamic_cast<TNode*>(pMirrorSource);
    }

    bool LinkMirror();
    bool IsLinked() const { return pMirrorSource != nullptr; }
    virtual bool GetAttribute(attribID_t attribID, std::string &strAttributeValue) const override;
    virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue) override;

    virtual bool IsAttributeChanged(attribID_t attribID) const override;
    virtual bool IsAttributeUsed(attribID_t attribID) const override;
private:
    void SetMirrorSource(BaseNode *pSource);
    void SetMirrorPath(const char* pPath);
    std::string mirrorPath;
    BaseNode *pMirrorSource;
};
