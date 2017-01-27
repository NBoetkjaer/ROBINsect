// Test.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <array>

#include "../nodes/BaseNode.hpp"
#include "../nodes/NumericNode.hpp"

int main(int argc, char* argv[])
{
    BaseNode root("root");
    BaseNode* pNode = root.AddChild<BaseNode>("Base");
    BaseNode* pBoolNode = root.AddChild<BoolNode>("Bool", true);
    BaseNode* pInt32Node = pNode->AddChild<Int32Node>("Int32", 0);
    BaseNode* pFloatNode = pInt32Node->AddChild<FloatNode>("Float", 0.0f);
    BaseNode* pStringNode = pFloatNode->AddChild<StringNode>("String", "Test string.");
    

    root.SetAttribute(Attribute::GetAttributeID("info"), "Dette er en test");
    assert(root.GetInfo()=="Dette er en test");

    //root.SetFlags("+hide|+readonly|+logging|persist|query");
    root.SetAttribute(Attribute::GetAttributeID("flags"),"+hide|+readonly|+logging|persist|query");
    assert(root.GetFlag(FlagType::hide));
    assert(root.GetFlag(FlagType::readonly));
    assert(root.GetFlag(FlagType::logging));
    assert(root.GetFlag(FlagType::persist));
    assert(root.GetFlag(FlagType::query));
    root.SetFlags("-hide|-logging|-query");
    assert(!root.GetFlag(FlagType::hide));
    assert(root.GetFlag(FlagType::readonly));
    assert(!root.GetFlag(FlagType::logging));
    assert(root.GetFlag(FlagType::persist));
    assert(!root.GetFlag(FlagType::query));

    BaseNode* pTmpNode;
    pTmpNode = pStringNode->FindNode("/Base/Int32/");
    assert(pTmpNode->GetName() == "Int32");

    pTmpNode = pStringNode->FindNode("/Base/Int32");
    assert(pTmpNode->GetName() == "Int32");

    pTmpNode = pStringNode->FindNode("../");
    assert(pTmpNode->GetName() == "Float");

    pTmpNode = pStringNode->FindNode("..");
    assert(pTmpNode->GetName() == "Float");

    pTmpNode = pStringNode->FindNode("../../../Int32");
    assert(pTmpNode->GetName() == "Int32");

    pTmpNode = pInt32Node->FindNode("Float/String");
    assert(pTmpNode->GetName() == "String");

    pTmpNode = pInt32Node->FindNode("Float/String/");
    assert(pTmpNode->GetName() == "String");

    pTmpNode = pStringNode->FindNode(".././../Int32");
    assert(pTmpNode == nullptr);

    pFloatNode->SetAttribute(Attribute::GetAttributeID("range"), "[12e-3,24.34e4]");
    pInt32Node->SetAttribute(Attribute::GetAttributeID("range"), "[12,24]");
    pInt32Node->SetAttribute(Attribute::GetAttributeID("value"), "25");

    pStringNode->SetAttribute(Attribute::GetAttributeID("value"), "Test string");
    root.Print(); 

    return 0;
}

