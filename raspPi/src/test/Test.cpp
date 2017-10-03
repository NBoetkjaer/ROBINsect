// Test.cpp : Defines the entry point for the console application.
//

#include <assert.h>
#include <memory>
#include <array>
#include <vector>

#ifdef WIN32
#include <conio.h>
#endif

#include "../application.hpp"
#include "../nodes/xml/nodeXmlConverter.hpp"
#include "../nodes/NodeFactory.hpp"
#include "../nodes/Nodes.hpp"

#include "../modules/TelnetModule.hpp"
#include "../modules/InsectModule.hpp"

int main(int argc, char* argv[])
{
    Eigen::Vector3f start(1.0f, 1.0f, 0.0f);
    Eigen::Vector3f target(1.0f, 1.0f, 5.0f);
    Eigen::Vector3f normal(0.0f, 1.0f, 0.0f);

    CircularTrajectorySegment test(start, target, normal, 5.0f, 30.0f);
    Eigen::Vector3f pos;
    test.GetPosition(0.0f, pos);
    test.GetPosition(30.0f, pos);

    float steps = 50.0f;
    float radius = 5.0f;
    float minAngle_rad = -0.2f;
    float maxAngle_rad = 1.2f;
    start <<  2.0f, radius * cos(minAngle_rad), radius * sin(minAngle_rad);
    target << 2.0f, radius * cos(maxAngle_rad), radius * sin(maxAngle_rad);
    normal << -1.0f, 0.0f, 0.0f;
    CircularTrajectorySegment testTrack(start, target, normal, radius, steps);
    Eigen::Vector3f posRef;
    for (float time = 0.0f; time < steps; time += 1.0f)
    {
        float angle_rad = minAngle_rad + time * (maxAngle_rad - minAngle_rad) / steps;
        posRef << 2.0f, radius * cos(angle_rad), radius * sin(angle_rad);
        testTrack.GetPosition(time, pos);
        std::cout << "diff: " << (posRef - pos).norm() << std::endl;
    }

    return 0;
    BaseNode root("root");

    std::cout << "Size of std::vector<NodeObserver*>: " << sizeof(std::vector<NodeObserver*>) << std::endl;
    std::cout << "Size of std::forward_list<MirrorNodes*>: " << sizeof(std::forward_list<MirrorNode*>) << std::endl;
    std::vector<std::unique_ptr<BaseNode>> testVec;
    std::cout << "Size of std::vector<std::unique_ptr<BaseNode>>: " << sizeof(testVec) << std::endl;
    std::cout << "Initial capacity of a vector: " << testVec.capacity() << std::endl;
    testVec.push_back(std::make_unique<BaseNode>("test"));
    std::cout << "Capacity of a vector (one added): " << testVec.capacity() << std::endl;
    std::cout << "Size of std::string: " << sizeof(std::string) << std::endl;
    std::string str;
    std::cout << "Initial capacity of a string: " << str.capacity() << std::endl;
    std::cout << "Size of BaseNode: " << sizeof(BaseNode) << std::endl;
    std::cout << "Size of BoolNode: " << sizeof(BoolNode) << std::endl;
    std::cout << "Size of Int32Node: " << sizeof(Int32Node) << std::endl;
    std::cout << "Size of Int64Node: " << sizeof(Int64Node) << std::endl;
    std::cout << "Size of FloatNode: " << sizeof(FloatNode) << std::endl;
    std::cout << "Size of DoubleNode: " << sizeof(DoubleNode) << std::endl;

    
    std::unique_ptr<BaseNode> baseNode = NodeFactory::CreateNode("node", "NodeFactoryTest");
    root.AddChild<BaseNode>(std::move(baseNode));
    BaseNode* pNode = root.AddChild<BaseNode>("Base");
    BaseNode* pBoolNode = root.AddChild<BoolNode>("Bool", true);
    BaseNode* pInt32Node = pNode->AddChild<Int32Node>("Int32", 0);
    BaseNode* pInt64Node = pNode->AddChild<Int64Node>("Int64", 0L);
    BaseNode* pFloatNode = pInt32Node->AddChild<FloatNode>("Float", 0.0f);
    BaseNode* pDoubleNode = pInt64Node->AddChild<DoubleNode>("Double", 0.0);
    BaseNode* pStringNode = pFloatNode->AddChild<StringNode>("String", "Test string.");
    
    std::cout << "Num Attrib =" << (int)valueAttrib.GetNumAttributes() << std::endl;
    std::cout << "GetAttributeID('info') =" << (int)Attribute::GetAttributeID("info") << std::endl;

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

    root.Print();

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

    pTmpNode = pStringNode->FindNode("../../../../Ba/Int3/F/S", true);
    assert(pTmpNode == pStringNode);

    pTmpNode = pStringNode->FindNode("../../../../B/I/F/S", true);
    assert(pTmpNode == nullptr);

    pFloatNode->SetAttribute(Attribute::GetAttributeID("range"), "[12e-3,24.34e4]");
    pFloatNode->SetAttribute((attribID_t)unitAttrib, "m/s");
    pFloatNode->SetFlag(FlagType::persist, true);
    pDoubleNode->SetAttribute(Attribute::GetAttributeID("range"), "[-180,180]");
    pDoubleNode->SetAttribute((attribID_t)unitAttrib, "deg");

    pInt32Node->SetAttribute(Attribute::GetAttributeID("range"), "[12,24]");
    pInt64Node->SetAttribute(Attribute::GetAttributeID("range"), "[12,24]");

    pBoolNode->SetAttribute(Attribute::GetAttributeID("value"), "true");
    pBoolNode->SetAttribute(Attribute::GetAttributeID("options"), "no,yes");
    pBoolNode->SetAttribute(Attribute::GetAttributeID("value"), "no");

    pInt32Node->SetAttribute(Attribute::GetAttributeID("value"), "25");

    pStringNode->SetAttribute(Attribute::GetAttributeID("value"), "Test string");    


    std::string tmpStr;
    root.GetAttribute((attribID_t)flagsAttrib, tmpStr);
    std::cout << tmpStr << std::endl;
    pBoolNode->GetAttribute((attribID_t)valueAttrib, tmpStr);
    std::cout << tmpStr << std::endl;
    pFloatNode->GetAttribute((attribID_t)valueAttrib, tmpStr);
    std::cout << tmpStr << std::endl;
    pStringNode->GetAttribute((attribID_t)valueAttrib, tmpStr);
    std::cout << tmpStr << std::endl;

    std::string xmlStr;
    NodeXmlConverter xmlConverter;
    xmlConverter.ConvertToXml(&root, xmlStr);
    std::cout << xmlStr;
    BaseNode newTree("NewTree");
    xmlConverter.UpdateTreeFromXml(&newTree, xmlStr);
    xmlConverter.ConvertToXml(&newTree, xmlStr);
    std::cout << xmlStr;

    Application app;
    app.RunLoop();
    return 0;
}

