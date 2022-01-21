using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ROBINinspect
{
    public partial class frmMonitor : Form
    {
        Monitor robinMonitor = new Monitor();
        Timer tmr = new Timer();
        DiscoveryInfo robinInfo;

        public frmMonitor()
        {
            InitializeComponent();
        }
        public frmMonitor(DiscoveryInfo info)
        {
            robinInfo = info;
            InitializeComponent();
        }
        private void btnConnect_Click(object sender, EventArgs e)
        {
            tmr.Interval = 20;
            tmr.Tick += tmr_Tick;
            tmr.Enabled = true;
            if (robinMonitor.IsConnected)
            {
                treeView1.Nodes.Clear();
                dataGridView1.Rows.Clear();
                robinMonitor.Close();
                robinMonitor.TransactionRecieved -= robinMonitor_TransactionRecieved;
            }
            else
            {
                robinMonitor.info = robinInfo;
                robinMonitor.TransactionRecieved += robinMonitor_TransactionRecieved;
                robinMonitor.Connect();
            }
        }
        void tmr_Tick(object sender, EventArgs e)
        {
            if (robinMonitor.IsConnected)
            {
                robinMonitor.Update();
                btnConnect.Text = "Close";
            }
            else
            {
                btnConnect.Text = "Connect";
            }
        }

        private void robinMonitor_TransactionRecieved(object sender, EventArgs e)
        {

            if (treeView1.Nodes.Count == 0)
            {
                treeView1.BeginUpdate();
                BaseNode rootNode = robinMonitor.rootNode;
                TreeNode currentTreeNode = treeView1.Nodes.Add(rootNode.Name);
                AddChildrenToNodeTree(rootNode, currentTreeNode);
                treeView1.EndUpdate();
            }

            if (treeView1.SelectedNode != null)
            {
                string path = treeView1.SelectedNode.FullPath;
                path = path.Replace('\\', '/');
                path = path.Replace("root", "");
                BaseNode selectedNode = robinMonitor.rootNode.FindNode(path);
                if (selectedNode == null || selectedNode.Children == null) return;
                // Update grid view.
                if (selectedNode.AnyChanges())
                {
                    treeView1_AfterSelect(null, null);
                }
            }
            //Console.WriteLine("New Transaction ...");
        }

        static private void AddChildrenToNodeTree(BaseNode parentNode, TreeNode tnode)
        {
            foreach (BaseNode childNode in parentNode.Children)
            {
                TreeNode childTNode = tnode.Nodes.Add(childNode.Name);
                BaseNode child = childNode;
                if (childNode is MirrorNode)
                {
                    MirrorNode mirror = childNode as MirrorNode;
                    if (mirror.IsLinked())
                    {
                        child = mirror.MirrorSource;
                    }
                }
                childTNode.Tag = child;
                if (child.Children.Count > 0)
                {
                    AddChildrenToNodeTree(child, childTNode);
                }
            }
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            dataGridView1.Rows.Clear();
            dataGridView1.ColumnCount = 2;
            if (treeView1.SelectedNode != null)
            {
                BaseNode node = treeView1.SelectedNode.Tag as BaseNode;
                if (node == null) return;
                string nodeType = NodeFactory.TypeAsString(NodeFactory.NodeType(node));
                dataGridView1.Columns[0].HeaderText = "Attribute";
                dataGridView1.Columns[0].ReadOnly = true;
                dataGridView1.Columns[1].HeaderText = nodeType;
                foreach (AttributeTypes attr in Attributes.ValidAttributes)
                {
                    if (node.IsAttributeUsed(attr))
                    {
                        string attribValue = String.Empty;
                        if (node.GetAttribute(attr, ref attribValue))
                        {
                            dataGridView1.Rows.Add(attr, attribValue);
                        }
                    }
                }
            }
        }

        private void dataGridView1_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (treeView1.SelectedNode != null)
            {
                BaseNode node = treeView1.SelectedNode.Tag as BaseNode;
                BaseNode root = node.GetRoot();
                root.ClearAllChanges();
                if (node == null) return;
                AttributeTypes attr = (AttributeTypes)dataGridView1.Rows[e.RowIndex].Cells[0].Value;
                string attribValue = dataGridView1.Rows[e.RowIndex].Cells[1].Value as string;
                node.SetAttribute(attr, attribValue);
                robinMonitor.SendTreeUpdates();
            }
        }

        private void btnLogger_Click(object sender, EventArgs e)
        {
            frmLogger frm = new frmLogger(robinMonitor);
            frm.Show();
        }
    }
}
