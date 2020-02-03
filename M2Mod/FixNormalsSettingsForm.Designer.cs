namespace M2Mod
{
    partial class FixNormalsSettingsForm
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
            this.settingsTextBox = new System.Windows.Forms.TextBox();
            this.saveButton = new System.Windows.Forms.Button();
            this.cancelButton = new System.Windows.Forms.Button();
            this.facialsCheckBox = new System.Windows.Forms.CheckBox();
            this.hairCheckBox = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // settingsTextBox
            // 
            this.settingsTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.settingsTextBox.Location = new System.Drawing.Point(12, 83);
            this.settingsTextBox.Multiline = true;
            this.settingsTextBox.Name = "settingsTextBox";
            this.settingsTextBox.Size = new System.Drawing.Size(355, 177);
            this.settingsTextBox.TabIndex = 0;
            // 
            // saveButton
            // 
            this.saveButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.saveButton.Location = new System.Drawing.Point(109, 266);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(75, 23);
            this.saveButton.TabIndex = 1;
            this.saveButton.Text = "Save";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // cancelButton
            // 
            this.cancelButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.cancelButton.Location = new System.Drawing.Point(190, 266);
            this.cancelButton.Name = "cancelButton";
            this.cancelButton.Size = new System.Drawing.Size(75, 23);
            this.cancelButton.TabIndex = 2;
            this.cancelButton.Text = "Cancel";
            this.cancelButton.UseVisualStyleBackColor = true;
            this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
            // 
            // facialsCheckBox
            // 
            this.facialsCheckBox.AutoSize = true;
            this.facialsCheckBox.Location = new System.Drawing.Point(12, 37);
            this.facialsCheckBox.Name = "facialsCheckBox";
            this.facialsCheckBox.Size = new System.Drawing.Size(59, 17);
            this.facialsCheckBox.TabIndex = 3;
            this.facialsCheckBox.Text = "Facials";
            this.facialsCheckBox.UseVisualStyleBackColor = true;
            // 
            // hairCheckBox
            // 
            this.hairCheckBox.AutoSize = true;
            this.hairCheckBox.Location = new System.Drawing.Point(77, 37);
            this.hairCheckBox.Name = "hairCheckBox";
            this.hairCheckBox.Size = new System.Drawing.Size(45, 17);
            this.hairCheckBox.TabIndex = 4;
            this.hairCheckBox.Text = "Hair";
            this.hairCheckBox.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoEllipsis = true;
            this.label1.Location = new System.Drawing.Point(12, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(355, 21);
            this.label1.TabIndex = 5;
            this.label1.Text = "Select additional body parts, whose normals will be aligned to body meshes";
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label2.AutoEllipsis = true;
            this.label2.Location = new System.Drawing.Point(15, 64);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(352, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Custom mesh ids go here:";
            // 
            // FixNormalsSettingsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(379, 296);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.hairCheckBox);
            this.Controls.Add(this.facialsCheckBox);
            this.Controls.Add(this.cancelButton);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.settingsTextBox);
            this.Name = "FixNormalsSettingsForm";
            this.Text = "FixNormalsSettingsForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox settingsTextBox;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.Button cancelButton;
        private System.Windows.Forms.CheckBox facialsCheckBox;
        private System.Windows.Forms.CheckBox hairCheckBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
    }
}