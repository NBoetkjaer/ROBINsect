#include "NodeFactory.hpp"
#include "MirrorNode.hpp"
#include <typeinfo>
#include <typeindex>
#include <array>
#include <tuple>

using namespace std;

static const array< tuple<type_index, std::string>, 11> typeNames =
{
    std::make_tuple(std::type_index(typeid(BaseNode)),       "node"),
    std::make_tuple(std::type_index(typeid(MirrorNode)),     "mirror"),
    std::make_tuple(std::type_index(typeid(BoolNode)),       "bool"),
    std::make_tuple(std::type_index(typeid(UInt16Node)),     "uint16"),
    std::make_tuple(std::type_index(typeid(UInt32Node)),     "uint32"),
    std::make_tuple(std::type_index(typeid(Int32Node)),      "int32"),
    std::make_tuple(std::type_index(typeid(Int64Node)),      "int64"),
    std::make_tuple(std::type_index(typeid(FloatNode)),      "float"),
    std::make_tuple(std::type_index(typeid(DoubleNode)),     "double"),
    std::make_tuple(std::type_index(typeid(Pos3D_32f_Node)), "pos3D_32f"),
    std::make_tuple(std::type_index(typeid(StringNode)),     "string")
};
std::string unknownNode = "Unknown";

std::unique_ptr<BaseNode> NodeFactory::CreateNode(const std::string& nodeType, const std::string& nodeName)
{
    for(const auto& type : typeNames)
    {
        if (get<1>(type).compare(nodeType) != 0)
        {
            continue;
        }
        const type_index& typeIdx = get<0>(type);
#define CREATE_NODE(TNodeType) if (typeIdx == typeid(TNodeType)) { return make_unique<TNodeType>(nodeName); }

        CREATE_NODE(BaseNode);
        CREATE_NODE(MirrorNode);
        CREATE_NODE(BoolNode);
        CREATE_NODE(UInt16Node);
        CREATE_NODE(UInt32Node);
        CREATE_NODE(Int32Node);
        CREATE_NODE(Int64Node);
        CREATE_NODE(FloatNode);
        CREATE_NODE(DoubleNode);
        CREATE_NODE(Pos3D_32f_Node);
        CREATE_NODE(StringNode);
#undef CREATE_NODE
    }
    return nullptr;
    // Error unknown node type.
}

const std::string& NodeFactory::NodeTypeName(const BaseNode* pNode)
{
    for (const auto& type : typeNames)
    {
        if (get<0>(type) == type_index(typeid(*pNode)) )
        {
            return get<1>(type);
        }
    }
    // Error unknown node type.
    return unknownNode;
}