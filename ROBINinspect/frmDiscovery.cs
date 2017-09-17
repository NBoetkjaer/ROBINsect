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
    public partial class frmDiscovery : Form
    {
        Discovery discover = new Discovery();
        Timer tmr = new Timer();

        public frmDiscovery()
        {
            InitializeComponent();
        }

        private void btnDiscover_Click(object sender, EventArgs e)
        {
            tmr.Interval = 100;
            tmr.Tick += tmr_Tick;
            tmr.Enabled = true;
            discover.StartDiscovery();
            discover.Discover += discover_Discover;
        }

        void tmr_Tick(object sender, EventArgs e)
        {
            discover.Update();
        }

        void discover_Discover(object sender, EventArgs e)
        {
            // Clear old entries
            flowLayoutPanel1.Controls.Clear();
            // Update list
            foreach (DiscoveryInfo info in discover.dicoveryMap.Values)
            {
                Button btn = new Button();
                btn.Width = flowLayoutPanel1.Width - flowLayoutPanel1.Margin.Size.Width;
                btn.Text = info.DisplayString();
                btn.Tag = info;
                btn.AutoSize = true;
                btn.Click += btn_Click;
                flowLayoutPanel1.Controls.Add(btn);
            }
        }

        void btn_Click(object sender, EventArgs e)
        {
            Button btn = (sender as Button);
            if (btn != null)
            {
                frmMonitor monitorFrm = new frmMonitor(btn.Tag as DiscoveryInfo);
                monitorFrm.Show();
            }
        }

        private void frmDiscovery_Load(object sender, EventArgs e)
        {
            btnDiscover_Click(btnDiscover, EventArgs.Empty);
        }
    }
}
