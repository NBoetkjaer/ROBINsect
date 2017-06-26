#pragma once
#include <array>
#include <string>
#include <vector>
#include <forward_list>
#include <algorithm>
#include <memory>
#include <iostream>
#include "Flags.hpp"
#include "Attribute.hpp"

#include "../util/BitmaskEnumClass.h"

#ifdef _MSC_VER
    #define strcasecmp _stricmp
#endif

// Attributes: Value, Type, Range, flags, Enums, DisplayName, DisplayFormat, unit, prefix, info, ... 
// Common attributes.
extern Attribute flagsAttrib;
extern Attribute typeAttrib;
extern Attribute infoAttrib;
extern Attribute optionsAttrib;

class MirrorNode; // Forward declare.
class NodeObserver;

class BaseNode
{
private:
    static std::array< std::tuple<const std::string, FlagType>, (size_t)FlagBitType::numflags> flagNames;

    size_t recentChanges;    // Indicate if this node or any of its child have changed. LSB indicate newest change, remaing bits act as a counter.
    size_t attributeChanges; // Variable to hold the applied changes to this node. Each attribID has a bit in changes.
    size_t touchedAttributes; // Variable used to collect all used (active) attributes.
    std::forward_list<NodeObserver*> subscribers;
    std::forward_list<MirrorNode*> mirrors;
protected:
    std::string name;
    BaseNode* pParent;
    std::vector<std::unique_ptr<BaseNode>> children;
public:
    BaseNode(const std::string &nodeName, BaseNode* pParentNode = nullptr);
    virtual ~BaseNode();

    const std::string&  GetName() const { return name; };
    BaseNode* GetParent() const { return pParent; };    
    const std::vector<std::unique_ptr<BaseNode>>& GetChilds() const { return children; };
    BaseNode* GetRoot() const;

    // -------- Atributes --------
    // Get the string representation of a given attribute.
    virtual bool GetAttribute(attribID_t attribID, std::string &strAttributeValue) const;
    // Sets the given attribute, based on the string argument.
    // Inherited nodes should call and return a base implementation if it does not handle the attributeID.
    // Returns true is the attribute is handled. 
    bool SetAttribute(attribID_t attribID, const std::string &strAttributeValue)
    {
        return SetAttribute(attribID, strAttributeValue.c_str());
    }
    virtual bool SetAttribute(attribID_t attribID, const char* pAttributeValue);

    // Mark a given attribute as changed.
    inline void SetAttributeChanged(attribID_t attribID)
    { 
        if (attributeChanges == 0)
        {   // Notify the node tree that a change has been applied.
            SetNodeChanged();
        }
        attributeChanges |= (size_t)1 << attribID;
        touchedAttributes |= attributeChanges;
    }
    virtual inline bool IsAttributeChanged(attribID_t attribID) const { return (attributeChanges & ((size_t)1 << attribID))!=0;}
    virtual inline bool IsAttributeUsed(attribID_t attribID) const { return (touchedAttributes  & ((size_t)1 << attribID)) != 0; }
    inline bool AnyChanges() const { return recentChanges != 0; };
    inline bool AnyRecentChanges() const { return recentChanges & 1; };
    inline int RecentChangeCount() const { return int((recentChanges >> 2) + (recentChanges & 1)); };


    void PushChangeHistory();
    void ClearAllChanges();
private:
    FlagType nodeFlag;
    std::string info;

    // Flag attribute methods.
public:
    void SetFlags(const char* pValues);
private:
    void GetFlags(std::string &strFlags) const;    
public:
    FlagType GetFlags() const { return nodeFlag; };
    void SetFlag(FlagType flag, bool value, bool recursive = false);
    bool GetFlag(FlagType flag) const;
    // Info attributes methods.
    const std::string &GetInfo() const {return info;}
    void SetInfo(const char* pValue);

    // Add/remove observers to this node.
    void Subscribe(NodeObserver* pObserver);
    void UnSubscribe(NodeObserver* pObserver);
    void Notify(bool recursive); // Notify all subscribers

    void AddMirror(MirrorNode* pMirror);
    void RemoveMirror(MirrorNode* pMirror);
    bool LinkAllMirrors();

private:
    BaseNode* FindNodeInternal(const char * pNodePath, bool allowPartialMatch, bool resolveMirrors);
    BaseNode* FindChild(const char * pNodeName)
    {   // Search in children
        for (const auto &child : children)
        {
            if (child->name.compare(pNodeName) == 0) return child.get();
        }
        return nullptr;
    }

public:
    template<typename TNode = BaseNode>
    TNode* FindNode(const std::string& nodePath, bool allowPartialMatch = false, bool resolveMirrors = true)
    {
        return dynamic_cast<TNode*>(FindNodeInternal(nodePath.c_str(), allowPartialMatch, resolveMirrors));
    }

    template<typename TNode = BaseNode>
    TNode* FindNode(const char * pNodePath, bool allowPartialMatch = false, bool resolveMirrors = true)
    {
        return dynamic_cast<TNode*>(FindNodeInternal(pNodePath, allowPartialMatch, resolveMirrors));
    }

    template<typename TNode = BaseNode>
    TNode* AddChild(std::unique_ptr<BaseNode> childNode);

    template<typename TNode = BaseNode, typename ...Args>
    TNode* AddChild(Args&&... params);

    template<typename TNode = BaseNode, typename ...Args>
    TNode* FindOrCreateChild(const char * pNodeName, Args&&... params);

    template<typename TNode = BaseNode, typename ...Args>
    TNode* FindOrCreateChild(const std::string& nodeName, Args&&... params)
    {
        return FindOrCreateChild<TNode>(nodeName.c_str(), std::forward<Args>(params)...);
    }

    void SetNodeChanged();
    void Print(int indentLevel = 0) const;
protected:
    static void GetOptions(std::string& strOptions, const std::vector<std::string> & options, const char delimiter = ',');
    static void SetOptions(const char* pValues, std::vector<std::string> & options, const char delimiter = ',');
    static void SetOptionValue(const char* pValue, const std::vector<std::string> & options, int &value);
};

