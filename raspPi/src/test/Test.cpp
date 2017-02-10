// Test.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <array>

#include "../nodes/BaseNode.hpp"
#include "../nodes/NumericNode.hpp"
#include "../nodes/BoolNode.hpp"
#include "../nodes/StringNode.hpp"

int main(int argc, char* argv[])
{
    std::cout << "Size of BaseNode: " << sizeof(BaseNode) << std::endl;
    std::cout << "Size of BoolNode: " << sizeof(BoolNode) << std::endl;
    std::cout << "Size of FloatNode: " << sizeof(FloatNode) << std::endl;
    std::cout << "Size of DoubleNode: " << sizeof(DoubleNode) << std::endl;

    BaseNode root("root");
    BaseNode* pNode = root.AddChild<BaseNode>("Base");
    BaseNode* pBoolNode = root.AddChild<BoolNode>("Bool", true);
    BaseNode* pInt32Node = pNode->AddChild<Int32Node>("Int32", 0);
    BaseNode* pInt64Node = pNode->AddChild<Int64Node>("Int64", 0L);
    BaseNode* pFloatNode = pInt32Node->AddChild<FloatNode>("Float", 0.0f);
    BaseNode* pDoubleNode = pInt64Node->AddChild<DoubleNode>("Double", 0.0);
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
    pDoubleNode->SetAttribute(Attribute::GetAttributeID("range"), "[12e-3,24.34e4]");

    pInt32Node->SetAttribute(Attribute::GetAttributeID("range"), "[12,24]");
    pInt64Node->SetAttribute(Attribute::GetAttributeID("range"), "[12,24]");

    pBoolNode->SetAttribute(Attribute::GetAttributeID("value"), "true");
    pBoolNode->SetAttribute(Attribute::GetAttributeID("options"), "no,yes");
    pBoolNode->SetAttribute(Attribute::GetAttributeID("value"), "no");

    pInt32Node->SetAttribute(Attribute::GetAttributeID("value"), "25");

    pStringNode->SetAttribute(Attribute::GetAttributeID("value"), "Test string");
    root.Print(); 

    return 0;
}

