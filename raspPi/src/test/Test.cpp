// Test.cpp : Defines the entry point for the console application.
//

#include <assert.h>
#include <array>
#include "../nodes/BaseNode.hpp"

int main(int argc, char* argv[])
{
    BaseNode root("root");
    BaseNode* pNode = root.AddChild<BaseNode>("Base");
    BaseNode* pBoolNode = root.AddChild<BoolNode>("Bool", true);
    BaseNode* pInt32Node = pNode->AddChild<Int32Node>("Int32", 0);
    BaseNode* pFloatNode = pInt32Node->AddChild<FloatNode>("Float", 0.0f);
    BaseNode* pStringNode = pFloatNode->AddChild<StringNode>("String", "Test string.");
    
    root.SetFlags("+hide|+readonly|+log|+persist|-query");

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
    root.Print(); 

	return 0;
}

