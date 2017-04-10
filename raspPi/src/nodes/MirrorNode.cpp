#include "MirrorNode.hpp"

Attribute pathAttrib("path");


bool MirrorNode::LinkMirror()
{
    BaseNode* pMirror = FindNode(mirrorPath, false, false);
    SetMirrorSource(pMirror);
    return IsLinked();
}

void MirrorNode::SetMirrorSource(BaseNode *pSource)
{
    if (pSource == nullptr)
    {
        if(pMirrorSource != nullptr) 
        {   // Remove old link.
            pMirrorSource->RemoveMirror(this);
            pMirrorSource = nullptr;
        }
        return;
    }

    if(pMirrorSource != nullptr &&  pMirrorSource != pSource)
    {  // Remove old link.
        pMirrorSource->RemoveMirror(this);
    }

    // Link to new source.
    pMirrorSource = pSource; 
    pMirrorSource->AddMirror(this);
}

void MirrorNode::SetMirrorPath(const char* pPath)
{
    mirrorPath = pPath;
    SetAttributeChanged(pathAttrib.GetID());
}

bool MirrorNode::SetAttribute(attribID_t attribID, const char* pAttributeValue)
{
    if (pathAttrib.GetID() == attribID)
    {
        SetMirrorPath(pAttributeValue);
        return true;
    }
    if (pMirrorSource == nullptr) return false;
    return pMirrorSource->SetAttribute(attribID, pAttributeValue);
}

bool MirrorNode::GetAttribute(attribID_t attribID, std::string &strAttributeValue) const
{
    if (pathAttrib.GetID() == attribID)
    {
        strAttributeValue = mirrorPath;
        return true;
    }if (pMirrorSource == nullptr) return false;
    return pMirrorSource->GetAttribute(attribID, strAttributeValue);
}

bool MirrorNode::IsAttributeChanged(attribID_t attribID) const
{
    if (pathAttrib.GetID() == attribID)
    {
        return BaseNode::IsAttributeChanged(attribID);
    }
    if (pMirrorSource == nullptr) return false;
    return pMirrorSource->IsAttributeChanged(attribID); 
}

bool MirrorNode::IsAttributeUsed(attribID_t attribID) const
{
    if (pathAttrib.GetID() == attribID)
    {
        return BaseNode::IsAttributeUsed(attribID);
    }
    if (pMirrorSource == nullptr) return false;
    return pMirrorSource->IsAttributeUsed(attribID);
}

