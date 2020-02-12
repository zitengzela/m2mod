namespace M2Mod.Controls
{
    partial class NormalizationRuleControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.targetTypeComboBox = new System.Windows.Forms.ComboBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.sourceTypeComboBox = new System.Windows.Forms.ComboBox();
            this.sourceTextBox = new M2Mod.Controls.CueTextBox();
            this.targetTextBox = new M2Mod.Controls.CueTextBox();
            this.preferSourceCheckBox = new System.Windows.Forms.CheckBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.enabledCheckBox = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // targetTypeComboBox
            // 
            this.targetTypeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.targetTypeComboBox.FormattingEnabled = true;
            this.targetTypeComboBox.Location = new System.Drawing.Point(3, 4);
            this.targetTypeComboBox.Name = "targetTypeComboBox";
            this.targetTypeComboBox.Size = new System.Drawing.Size(69, 21);
            this.targetTypeComboBox.TabIndex = 2;
            this.targetTypeComboBox.SelectedIndexChanged += new System.EventHandler(this.targetTypeComboBox_SelectedIndexChanged);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.enabledCheckBox);
            this.splitContainer1.Panel1.Controls.Add(this.sourceTypeComboBox);
            this.splitContainer1.Panel1.Controls.Add(this.sourceTextBox);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.targetTextBox);
            this.splitContainer1.Panel2.Controls.Add(this.targetTypeComboBox);
            this.splitContainer1.Size = new System.Drawing.Size(298, 56);
            this.splitContainer1.SplitterDistance = 148;
            this.splitContainer1.SplitterWidth = 1;
            this.splitContainer1.TabIndex = 5;
            // 
            // sourceTypeComboBox
            // 
            this.sourceTypeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.sourceTypeComboBox.FormattingEnabled = true;
            this.sourceTypeComboBox.Location = new System.Drawing.Point(3, 4);
            this.sourceTypeComboBox.Name = "sourceTypeComboBox";
            this.sourceTypeComboBox.Size = new System.Drawing.Size(69, 21);
            this.sourceTypeComboBox.TabIndex = 3;
            this.sourceTypeComboBox.SelectedIndexChanged += new System.EventHandler(this.sourceTypeComboBox_SelectedIndexChanged);
            // 
            // sourceTextBox
            // 
            this.sourceTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.sourceTextBox.Cue = "Enter rule";
            this.sourceTextBox.Location = new System.Drawing.Point(3, 31);
            this.sourceTextBox.Name = "sourceTextBox";
            this.sourceTextBox.Size = new System.Drawing.Size(139, 20);
            this.sourceTextBox.TabIndex = 2;
            // 
            // targetTextBox
            // 
            this.targetTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.targetTextBox.Cue = "Enter rule";
            this.targetTextBox.Location = new System.Drawing.Point(3, 31);
            this.targetTextBox.Name = "targetTextBox";
            this.targetTextBox.Size = new System.Drawing.Size(140, 20);
            this.targetTextBox.TabIndex = 3;
            // 
            // preferSourceCheckBox
            // 
            this.preferSourceCheckBox.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.preferSourceCheckBox.AutoSize = true;
            this.preferSourceCheckBox.Location = new System.Drawing.Point(233, 6);
            this.preferSourceCheckBox.Name = "preferSourceCheckBox";
            this.preferSourceCheckBox.Size = new System.Drawing.Size(89, 17);
            this.preferSourceCheckBox.TabIndex = 6;
            this.preferSourceCheckBox.Text = "Prefer source";
            this.toolTip1.SetToolTip(this.preferSourceCheckBox, "Normals of source mesh are used instead of computing average ones");
            this.preferSourceCheckBox.UseVisualStyleBackColor = true;
            // 
            // enabledCheckBox
            // 
            this.enabledCheckBox.AutoSize = true;
            this.enabledCheckBox.Checked = true;
            this.enabledCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.enabledCheckBox.Location = new System.Drawing.Point(77, 6);
            this.enabledCheckBox.Name = "enabledCheckBox";
            this.enabledCheckBox.Size = new System.Drawing.Size(65, 17);
            this.enabledCheckBox.TabIndex = 4;
            this.enabledCheckBox.Text = "Enabled";
            this.enabledCheckBox.UseVisualStyleBackColor = true;
            this.enabledCheckBox.CheckedChanged += new System.EventHandler(this.enabledCheckBox_CheckedChanged);
            // 
            // NormalizationRuleControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.preferSourceCheckBox);
            this.Controls.Add(this.splitContainer1);
            this.Name = "NormalizationRuleControl";
            this.Size = new System.Drawing.Size(399, 56);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ComboBox targetTypeComboBox;
        private CueTextBox targetTextBox;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.ComboBox sourceTypeComboBox;
        private CueTextBox sourceTextBox;
        private System.Windows.Forms.CheckBox preferSourceCheckBox;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.CheckBox enabledCheckBox;
    }
}
