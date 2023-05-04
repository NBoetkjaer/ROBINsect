using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace ROBINinspect
{
    public partial class frmLogger : Form
    {
        Monitor robinMonitor;
        int transactionCount = 0;
        const int maximumDataPoints = 20000;

        public frmLogger(Monitor monitor)
        {
            InitializeComponent();

            InitializeChart();
            robinMonitor = monitor;
        }

        ~frmLogger()
        {

        }

        void InitializeChart()
        {
            chart1.ChartAreas[0].CursorX.IsUserEnabled = true;
            chart1.ChartAreas[0].CursorX.IsUserSelectionEnabled = true;
            chart1.ChartAreas[0].CursorX.SelectionColor = System.Drawing.Color.IndianRed;
            chart1.ChartAreas[0].CursorY.IsUserEnabled = true;
            chart1.ChartAreas[0].CursorY.IsUserSelectionEnabled = true;
            chart1.ChartAreas[0].CursorY.SelectionColor = System.Drawing.Color.IndianRed;

            chart1.ChartAreas[0].AxisY2.MajorGrid.Enabled = false;
            chart1.ChartAreas[0].AxisY2.MinorGrid.Enabled = false;

            chart1.Series.Clear();
        }

        private void frmLogger_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (robinMonitor != null)
            {
                robinMonitor.TransactionRecieved -= RobinMonitor_TransactionRecieved;
            }
        }

        private void chkLogging_CheckedChanged(object sender, EventArgs e)
        {
            if (chkLogging.Checked)
            {
                transactionCount = 0;
                if (robinMonitor != null)
                {
                    robinMonitor.TransactionRecieved += RobinMonitor_TransactionRecieved;
                }
                chkLogging.Text = "Stop logging";
                foreach (Series myserie in chart1.Series)
                {
                    myserie.Points.Clear();
                }
            }
            else
            {
                chkLogging.Text = "Start logging";
                if (robinMonitor != null)
                {
                    robinMonitor.TransactionRecieved -= RobinMonitor_TransactionRecieved;
                }
            }
        }

        private void RobinMonitor_TransactionRecieved(object sender, EventArgs e)
        {
            int i = 0;
            transactionCount++;
            foreach (Series myserie in chart1.Series)
            {
                if (myserie.Points.Count > maximumDataPoints) continue;
                AbstractNumericNode node = myserie.Tag as AbstractNumericNode;
                if (node != null && node.IsAttributeChanged(AttributeTypes.value) )
                {
                    double newValue =  node.ValueAsDouble();
                    myserie.Points.AddXY(transactionCount, newValue);
                }
                i++;
            }
            if (chkAutoScroll.Checked)
            {
                ChartArea ca = chart1.ChartAreas[0];
                ca.AxisX.Minimum = double.NaN;
                ca.AxisX.Maximum = double.NaN;
                double viewSize = 600;
                double minX = Math.Max(0, transactionCount - viewSize);
                double maxX = minX + viewSize;
                ca.AxisX.ScaleView.Zoom(minX, maxX);
                ca.RecalculateAxesScale();
            }
        }

        private void txtFindNode_TextChanged(object sender, EventArgs e)
        {
            TextBox txtBox = sender as TextBox;
            if (txtBox == null) return;
            btnAdd.Enabled = false;
            BaseNode node = robinMonitor.rootNode.FindNode(txtBox.Text, true, true);
            if(node != null)
            {
                lblNode.Text = node.GetFullPath();
                if (node is BaseNode)
                    lblNode.Text += "/";

                if (node is FloatNode)
                    btnAdd.Enabled = true;
            }
            else
            {                
                lblNode.Text = "";
            }
        }

        private void txtFindNode_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyData == Keys.Tab)
            {
                e.IsInputKey = true;
                TextBox txtBox = sender as TextBox;
                if (txtBox == null || lblNode.Text.Length == 0) return;
                txtBox.Text = lblNode.Text;
                txtBox.SelectionLength = 0;
                txtBox.SelectionStart = txtBox.Text.Length;                
            }
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            AddNode(lblNode.Text);
        }

        void AddNode(String nodePath)
        {
            AbstractNumericNode node = robinMonitor.rootNode.FindNode(nodePath, true, true) as AbstractNumericNode;
            if (node != null && chart1.Series.IsUniqueName(node.GetFullPath()))
            {
                Series mySerie = new Series
                {
                    ChartType = SeriesChartType.FastLine, // Set fast line chart type
                    Name = node.GetFullPath(),
                    IsVisibleInLegend = true,
                    Tag = node
                };
                chart1.Series.Add(mySerie);
                chart1.ChartAreas[0].AxisX.LabelStyle.Format = "G3";
            }
        }

        private void btnAddGoal_Click(object sender, EventArgs e)
        {
            string path = "/Insect/Leg/l" + nudLegNumber.Value.ToString() + "/goalPos/";
            AddNode(path + "X");
            AddNode(path + "Y");
            AddNode(path + "Z");
        }

        private void btnAddJoints_Click(object sender, EventArgs e)
        {
            string path = "/Insect/Leg/l" + nudLegNumber.Value.ToString() + "/Joints/";
            AddNode(path + "j0/jointAngle");
            AddNode(path + "j1/jointAngle");
            AddNode(path + "j2/jointAngle");

        }

        private void chart1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                HitTestResult result = chart1.HitTest(e.X, e.Y);
                if (result != null && result.Object != null)
                {
                    if (result.ChartElementType == ChartElementType.LegendItem)
                    {
                        Series dataSerie = chart1.Series[(result.Object as LegendItem).SeriesName];
                        if (dataSerie == null) return;
                        ContextMenu popup = new ContextMenu();
                        MenuItem mnuItemDelete = new MenuItem("Remove: " + dataSerie.Name);

                        mnuItemDelete.Tag = dataSerie;
                        mnuItemDelete.Click += (mnuItem, ev) => {
                            chart1.Series.Remove((mnuItem as MenuItem).Tag as Series);  };
                        popup.MenuItems.Add(mnuItemDelete);
                        popup.Show(chart1, e.Location);
                    }
                }
            }
        }

    }
}
