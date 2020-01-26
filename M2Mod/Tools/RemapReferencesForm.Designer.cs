namespace M2Mod.Tools
{
    partial class RemapReferencesForm
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
            this.closeButton = new System.Windows.Forms.Button();
            this.convertButton = new System.Windows.Forms.Button();
            this.m2BrowseButton = new System.Windows.Forms.Button();
            this.oldM2Label = new System.Windows.Forms.Label();
            this.m2PathTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.remapRelativePathTextBox = new System.Windows.Forms.TextBox();
            this.logTextBox = new M2Mod.LogTextBox();
            this.SuspendLayout();
            // 
            // closeButton
            // 
            this.closeButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.closeButton.Location = new System.Drawing.Point(305, 241);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(64, 30);
            this.closeButton.TabIndex = 41;
            this.closeButton.Text = "Close";
            this.closeButton.UseVisualStyleBackColor = true;
            this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
            // 
            // convertButton
            // 
            this.convertButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.convertButton.Location = new System.Drawing.Point(235, 241);
            this.convertButton.Name = "convertButton";
            this.convertButton.Size = new System.Drawing.Size(64, 30);
            this.convertButton.TabIndex = 40;
            this.convertButton.Text = "Convert";
            this.convertButton.UseVisualStyleBackColor = true;
            this.convertButton.Click += new System.EventHandler(this.convertButton_Click);
            // 
            // m2BrowseButton
            // 
            this.m2BrowseButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m2BrowseButton.Location = new System.Drawing.Point(543, 12);
            this.m2BrowseButton.Name = "m2BrowseButton";
            this.m2BrowseButton.Size = new System.Drawing.Size(52, 20);
            this.m2BrowseButton.TabIndex = 39;
            this.m2BrowseButton.Text = "...";
            this.m2BrowseButton.UseVisualStyleBackColor = true;
            this.m2BrowseButton.Click += new System.EventHandler(this.m2BrowseButton_Click);
            // 
            // oldM2Label
            // 
            this.oldM2Label.AutoSize = true;
            this.oldM2Label.Location = new System.Drawing.Point(14, 16);
            this.oldM2Label.Name = "oldM2Label";
            this.oldM2Label.Size = new System.Drawing.Size(46, 13);
            this.oldM2Label.TabIndex = 38;
            this.oldM2Label.Text = "M2 path";
            // 
            // m2PathTextBox
            // 
            this.m2PathTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m2PathTextBox.Location = new System.Drawing.Point(122, 13);
            this.m2PathTextBox.Name = "m2PathTextBox";
            this.m2PathTextBox.Size = new System.Drawing.Size(410, 20);
            this.m2PathTextBox.TabIndex = 37;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(14, 42);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(102, 13);
            this.label1.TabIndex = 44;
            this.label1.Text = "Remap relative path";
            // 
            // remapRelativePathTextBox
            // 
            this.remapRelativePathTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.remapRelativePathTextBox.Location = new System.Drawing.Point(122, 39);
            this.remapRelativePathTextBox.Name = "remapRelativePathTextBox";
            this.remapRelativePathTextBox.Size = new System.Drawing.Size(410, 20);
            this.remapRelativePathTextBox.TabIndex = 43;
            // 
            // logTextBox
            // 
            this.logTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.logTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.logTextBox.DetectUrls = false;
            this.logTextBox.InlcudeTimestamp = true;
            this.logTextBox.Location = new System.Drawing.Point(17, 67);
            this.logTextBox.Name = "logTextBox";
            this.logTextBox.ReadOnly = true;
            this.logTextBox.Size = new System.Drawing.Size(578, 168);
            this.logTextBox.TabIndex = 42;
            this.logTextBox.Text = "";
            this.logTextBox.UseColors = true;
            // 
            // RemapReferencesForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(610, 276);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.remapRelativePathTextBox);
            this.Controls.Add(this.logTextBox);
            this.Controls.Add(this.closeButton);
            this.Controls.Add(this.convertButton);
            this.Controls.Add(this.m2BrowseButton);
            this.Controls.Add(this.oldM2Label);
            this.Controls.Add(this.m2PathTextBox);
            this.Name = "RemapReferencesForm";
            this.Text = "References Remapper";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.RemapReferencesForm_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private LogTextBox logTextBox;
        private System.Windows.Forms.Button closeButton;
        private System.Windows.Forms.Button convertButton;
        private System.Windows.Forms.Button m2BrowseButton;
        private System.Windows.Forms.Label oldM2Label;
        private System.Windows.Forms.TextBox m2PathTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox remapRelativePathTextBox;
    }
}