
namespace ROBINinspect
{
    partial class frmLogger
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend3 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.txtFindNode = new System.Windows.Forms.TextBox();
            this.lblNode = new System.Windows.Forms.Label();
            this.btnAdd = new System.Windows.Forms.Button();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chkLogging = new System.Windows.Forms.CheckBox();
            this.grbPredefinedCollections = new System.Windows.Forms.GroupBox();
            this.btnAddJoints = new System.Windows.Forms.Button();
            this.btnAddGoal = new System.Windows.Forms.Button();
            this.nudLegNumber = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.grbPredefinedCollections.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudLegNumber)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtFindNode
            // 
            this.txtFindNode.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
            this.txtFindNode.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.CustomSource;
            this.txtFindNode.Location = new System.Drawing.Point(9, 19);
            this.txtFindNode.Name = "txtFindNode";
            this.txtFindNode.Size = new System.Drawing.Size(209, 20);
            this.txtFindNode.TabIndex = 0;
            this.txtFindNode.Text = "/";
            this.txtFindNode.TextChanged += new System.EventHandler(this.txtFindNode_TextChanged);
            this.txtFindNode.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.txtFindNode_PreviewKeyDown);
            // 
            // lblNode
            // 
            this.lblNode.AutoSize = true;
            this.lblNode.Location = new System.Drawing.Point(6, 39);
            this.lblNode.Name = "lblNode";
            this.lblNode.Size = new System.Drawing.Size(12, 13);
            this.lblNode.TabIndex = 1;
            this.lblNode.Text = "/";
            // 
            // btnAdd
            // 
            this.btnAdd.Enabled = false;
            this.btnAdd.Location = new System.Drawing.Point(187, 55);
            this.btnAdd.Name = "btnAdd";
            this.btnAdd.Size = new System.Drawing.Size(58, 20);
            this.btnAdd.TabIndex = 2;
            this.btnAdd.Text = "Add";
            this.btnAdd.UseVisualStyleBackColor = true;
            this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
            // 
            // chart1
            // 
            this.chart1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            chartArea3.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea3);
            legend3.Name = "Legend1";
            this.chart1.Legends.Add(legend3);
            this.chart1.Location = new System.Drawing.Point(2, 93);
            this.chart1.Name = "chart1";
            series3.ChartArea = "ChartArea1";
            series3.Legend = "Legend1";
            series3.Name = "Series1";
            this.chart1.Series.Add(series3);
            this.chart1.Size = new System.Drawing.Size(898, 488);
            this.chart1.TabIndex = 3;
            this.chart1.Text = "chart1";
            this.chart1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.chart1_MouseDown);
            // 
            // chkLogging
            // 
            this.chkLogging.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.chkLogging.Appearance = System.Windows.Forms.Appearance.Button;
            this.chkLogging.Location = new System.Drawing.Point(772, 16);
            this.chkLogging.Name = "chkLogging";
            this.chkLogging.Size = new System.Drawing.Size(128, 51);
            this.chkLogging.TabIndex = 5;
            this.chkLogging.Text = "Start Logging";
            this.chkLogging.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.chkLogging.UseVisualStyleBackColor = true;
            this.chkLogging.CheckedChanged += new System.EventHandler(this.chkLogging_CheckedChanged);
            // 
            // grbPredefinedCollections
            // 
            this.grbPredefinedCollections.Controls.Add(this.btnAddJoints);
            this.grbPredefinedCollections.Controls.Add(this.btnAddGoal);
            this.grbPredefinedCollections.Controls.Add(this.nudLegNumber);
            this.grbPredefinedCollections.Controls.Add(this.label1);
            this.grbPredefinedCollections.Location = new System.Drawing.Point(12, 4);
            this.grbPredefinedCollections.Name = "grbPredefinedCollections";
            this.grbPredefinedCollections.Size = new System.Drawing.Size(250, 83);
            this.grbPredefinedCollections.TabIndex = 6;
            this.grbPredefinedCollections.TabStop = false;
            this.grbPredefinedCollections.Text = "Predefined Collections";
            // 
            // btnAddJoints
            // 
            this.btnAddJoints.Location = new System.Drawing.Point(127, 44);
            this.btnAddJoints.Name = "btnAddJoints";
            this.btnAddJoints.Size = new System.Drawing.Size(108, 19);
            this.btnAddJoints.TabIndex = 2;
            this.btnAddJoints.Text = "Add Joint Angles";
            this.btnAddJoints.UseVisualStyleBackColor = true;
            this.btnAddJoints.Click += new System.EventHandler(this.btnAddJoints_Click);
            // 
            // btnAddGoal
            // 
            this.btnAddGoal.Location = new System.Drawing.Point(127, 19);
            this.btnAddGoal.Name = "btnAddGoal";
            this.btnAddGoal.Size = new System.Drawing.Size(108, 19);
            this.btnAddGoal.TabIndex = 2;
            this.btnAddGoal.Text = "Add Goal Pos";
            this.btnAddGoal.UseVisualStyleBackColor = true;
            this.btnAddGoal.Click += new System.EventHandler(this.btnAddGoal_Click);
            // 
            // nudLegNumber
            // 
            this.nudLegNumber.Location = new System.Drawing.Point(61, 28);
            this.nudLegNumber.Maximum = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.nudLegNumber.Name = "nudLegNumber";
            this.nudLegNumber.Size = new System.Drawing.Size(48, 20);
            this.nudLegNumber.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 30);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(43, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Leg no.";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.txtFindNode);
            this.groupBox1.Controls.Add(this.lblNode);
            this.groupBox1.Controls.Add(this.btnAdd);
            this.groupBox1.Location = new System.Drawing.Point(269, 4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(251, 83);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Find node";
            // 
            // frmLogger
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(912, 593);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.grbPredefinedCollections);
            this.Controls.Add(this.chkLogging);
            this.Controls.Add(this.chart1);
            this.Name = "frmLogger";
            this.Text = "frmLogger";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.frmLogger_FormClosed);
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            this.grbPredefinedCollections.ResumeLayout(false);
            this.grbPredefinedCollections.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudLegNumber)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox txtFindNode;
        private System.Windows.Forms.Label lblNode;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.CheckBox chkLogging;
        private System.Windows.Forms.GroupBox grbPredefinedCollections;
        private System.Windows.Forms.Button btnAddJoints;
        private System.Windows.Forms.Button btnAddGoal;
        private System.Windows.Forms.NumericUpDown nudLegNumber;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
    }
}