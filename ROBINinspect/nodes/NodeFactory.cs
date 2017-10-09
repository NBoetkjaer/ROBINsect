using System;

namespace ROBINinspect
{
    public enum NodeTypes {
        nodeNode,
        mirrorNode,
        boolNode,
        uint16Node,
        uint32Node,
        int32Node,
        int64Node,
        floatNode,
        doubleNode,
        pos3D_32fNode,
        stringNode
    }
    class NodeFactory
    {
        private static String[] nodeNames;
        private static String[] NodeNames
        {
            get
            {
                if (nodeNames == null)
                {
                    Array enumArr = Enum.GetValues(typeof(NodeTypes));
                    nodeNames = new String[enumArr.Length];
                    foreach (NodeTypes eNodeType in enumArr)
                    {
                        String tmp = eNodeType.ToString();
                        nodeNames[(int)eNodeType] = tmp.Replace("Node", "");
                    }
                }
                return nodeNames;
            }
        }
        public static BaseNode CreateNode(String nodeType, String nodeName)
        {
            BaseNode newNode = null;
            NodeTypes eNodeType = NodeTypes.nodeNode;
            if(GetNodeType(nodeType, ref eNodeType))
            {
                switch (eNodeType)
                {
                    case NodeTypes.nodeNode:
                        newNode = new BaseNode(nodeName);
                        break;
                    case NodeTypes.mirrorNode:
                        newNode = new MirrorNode(nodeName);
                        break;                        
                    case NodeTypes.boolNode:
                        newNode = new BoolNode(nodeName);
                        break;
                    case NodeTypes.uint16Node:
                        newNode = new UInt16Node(nodeName);
                        break;
                    case NodeTypes.uint32Node:
                        newNode = new UInt32Node(nodeName);
                        break;
                    case NodeTypes.int32Node:
                        newNode = new Int32Node(nodeName);
                        break;
                    case NodeTypes.int64Node:
                        newNode = new Int64Node(nodeName);
                        break;
                    case NodeTypes.floatNode:
                        newNode = new FloatNode(nodeName);
                        break;
                    case NodeTypes.doubleNode:
                        newNode = new DoubleNode(nodeName);
                        break;
                    case NodeTypes.pos3D_32fNode:
                        newNode = new Pos3D_32f_Node(nodeName);
                        break;
                    case NodeTypes.stringNode:
                        newNode = new StringNode(nodeName);
                        break;
                    default:
                        break;
                }
            }        
            return newNode;
        }

        static public bool GetNodeType(String strNodeType, ref NodeTypes type)
        {
            String[] names = NodeNames;
            int i = 0;
            foreach (String str in names)
            {
                if (str.Equals(strNodeType))
                {
                    type = (NodeTypes)i;
                    return true;
                }
                i++;
            }
            return false;
        }

        static public String TypeAsString(NodeTypes type)
        {
            if ((int)type >= 0 && (int)type < NodeNames.Length)
                return NodeNames[(int)type];
            return "Unknown";
        }

        static public NodeTypes NodeType(BaseNode node)
        {

            if (node.GetType() == typeof(BaseNode)) return NodeTypes.nodeNode;
            if (node.GetType() == typeof(BoolNode)) return NodeTypes.boolNode;
            if (node.GetType() == typeof(UInt16Node)) return NodeTypes.uint16Node;
            if (node.GetType() == typeof(UInt32Node)) return NodeTypes.uint32Node;
            if (node.GetType() == typeof(Int32Node)) return NodeTypes.int32Node;
            if (node.GetType() == typeof(Int64Node)) return NodeTypes.int64Node;
            if (node.GetType() == typeof(FloatNode)) return NodeTypes.floatNode;
            if (node.GetType() == typeof(DoubleNode)) return NodeTypes.doubleNode;
            if (node.GetType() == typeof(Pos3D_32f_Node)) return NodeTypes.pos3D_32fNode;
            if (node.GetType() == typeof(StringNode)) return NodeTypes.stringNode;
            if (node.GetType() == typeof(MirrorNode)) return NodeTypes.mirrorNode;
            throw new NotSupportedException("You need to add support for node" + node.GetType().ToString());
        }
    }
}
