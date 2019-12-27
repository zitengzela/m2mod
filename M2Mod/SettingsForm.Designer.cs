namespace M2Mod
{
    partial class SettingsForm
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
            this.components = new System.ComponentModel.Container();
            this.outputDirectoryBrowseButton = new System.Windows.Forms.Button();
            this.outputDirectoryGroupBox = new System.Windows.Forms.GroupBox();
            this.outputDirectoryTextBox = new System.Windows.Forms.TextBox();
            this.workingDirectoryTextBox = new System.Windows.Forms.TextBox();
            this.checkBoxFixEdgeNormals = new System.Windows.Forms.CheckBox();
            this.checkBoxIgnoreOriginalMeshIndexes = new System.Windows.Forms.CheckBox();
            this.testFixAnimationsCheckBox = new System.Windows.Forms.CheckBox();
            this.checkBoxMergeCameras = new System.Windows.Forms.CheckBox();
            this.workingDirectoryBrowseButton = new System.Windows.Forms.Button();
            this.workingDirectoryGroupBox = new System.Windows.Forms.GroupBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.checkBoxMergeAttachments = new System.Windows.Forms.CheckBox();
            this.checkBoxMergeBones = new System.Windows.Forms.CheckBox();
            this.forceExpansionComboBox = new System.Windows.Forms.ComboBox();
            this.mappingsDirectoryGroupBox = new System.Windows.Forms.GroupBox();
            this.mappingsDirectoryButton = new System.Windows.Forms.Button();
            this.mappingsDirectoryTextBox = new System.Windows.Forms.TextBox();
            this.saveButton = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.closeButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.profilesComboBox = new System.Windows.Forms.ComboBox();
            this.profilesLabel = new System.Windows.Forms.Label();
            this.editProfilesButton = new System.Windows.Forms.Button();
            this.customFilesStartIndexTextBox = new System.Windows.Forms.TextBox();
            this.customFilesStartIndexLabel = new System.Windows.Forms.Label();
            this.outputDirectoryGroupBox.SuspendLayout();
            this.workingDirectoryGroupBox.SuspendLayout();
            this.mappingsDirectoryGroupBox.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // outputDirectoryBrowseButton
            // 
            this.outputDirectoryBrowseButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.outputDirectoryBrowseButton.Location = new System.Drawing.Point(265, 20);
            this.outputDirectoryBrowseButton.Name = "outputDirectoryBrowseButton";
            this.outputDirectoryBrowseButton.Size = new System.Drawing.Size(37, 22);
            this.outputDirectoryBrowseButton.TabIndex = 6;
            this.outputDirectoryBrowseButton.Text = "...";
            this.outputDirectoryBrowseButton.UseVisualStyleBackColor = true;
            this.outputDirectoryBrowseButton.Click += new System.EventHandler(this.OutputDirectoryBrowseButton_Click);
            // 
            // outputDirectoryGroupBox
            // 
            this.outputDirectoryGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.outputDirectoryGroupBox.Controls.Add(this.outputDirectoryBrowseButton);
            this.outputDirectoryGroupBox.Controls.Add(this.outputDirectoryTextBox);
            this.outputDirectoryGroupBox.Location = new System.Drawing.Point(12, 103);
            this.outputDirectoryGroupBox.Name = "outputDirectoryGroupBox";
            this.outputDirectoryGroupBox.Size = new System.Drawing.Size(307, 56);
            this.outputDirectoryGroupBox.TabIndex = 30;
            this.outputDirectoryGroupBox.TabStop = false;
            this.outputDirectoryGroupBox.Text = "Output directory";
            this.toolTip1.SetToolTip(this.outputDirectoryGroupBox, "M2Mod output files will be placed in this directory");
            // 
            // outputDirectoryTextBox
            // 
            this.outputDirectoryTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.outputDirectoryTextBox.Location = new System.Drawing.Point(9, 21);
            this.outputDirectoryTextBox.Name = "outputDirectoryTextBox";
            this.outputDirectoryTextBox.Size = new System.Drawing.Size(250, 20);
            this.outputDirectoryTextBox.TabIndex = 5;
            // 
            // workingDirectoryTextBox
            // 
            this.workingDirectoryTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.workingDirectoryTextBox.Location = new System.Drawing.Point(9, 21);
            this.workingDirectoryTextBox.Name = "workingDirectoryTextBox";
            this.workingDirectoryTextBox.Size = new System.Drawing.Size(250, 20);
            this.workingDirectoryTextBox.TabIndex = 5;
            // 
            // checkBoxFixEdgeNormals
            // 
            this.checkBoxFixEdgeNormals.AutoSize = true;
            this.checkBoxFixEdgeNormals.Location = new System.Drawing.Point(131, 19);
            this.checkBoxFixEdgeNormals.Name = "checkBoxFixEdgeNormals";
            this.checkBoxFixEdgeNormals.Size = new System.Drawing.Size(108, 17);
            this.checkBoxFixEdgeNormals.TabIndex = 22;
            this.checkBoxFixEdgeNormals.Text = "Fix Edge Normals";
            this.toolTip1.SetToolTip(this.checkBoxFixEdgeNormals, "Will fix visual shading difference on sharp edges");
            this.checkBoxFixEdgeNormals.UseVisualStyleBackColor = true;
            // 
            // checkBoxIgnoreOriginalMeshIndexes
            // 
            this.checkBoxIgnoreOriginalMeshIndexes.AutoSize = true;
            this.checkBoxIgnoreOriginalMeshIndexes.Location = new System.Drawing.Point(131, 42);
            this.checkBoxIgnoreOriginalMeshIndexes.Name = "checkBoxIgnoreOriginalMeshIndexes";
            this.checkBoxIgnoreOriginalMeshIndexes.Size = new System.Drawing.Size(159, 17);
            this.checkBoxIgnoreOriginalMeshIndexes.TabIndex = 21;
            this.checkBoxIgnoreOriginalMeshIndexes.Text = "Ignore original mesh indexes";
            this.toolTip1.SetToolTip(this.checkBoxIgnoreOriginalMeshIndexes, "Materials for meshes will be selected comparing centers of mass between original " +
        "and modified meshes, not by index tunneled from original M2\r\n");
            this.checkBoxIgnoreOriginalMeshIndexes.UseVisualStyleBackColor = true;
            // 
            // testFixAnimationsCheckBox
            // 
            this.testFixAnimationsCheckBox.AutoSize = true;
            this.testFixAnimationsCheckBox.Location = new System.Drawing.Point(131, 65);
            this.testFixAnimationsCheckBox.Name = "testFixAnimationsCheckBox";
            this.testFixAnimationsCheckBox.Size = new System.Drawing.Size(94, 17);
            this.testFixAnimationsCheckBox.TabIndex = 20;
            this.testFixAnimationsCheckBox.Text = "Test Fix Anims";
            this.toolTip1.SetToolTip(this.testFixAnimationsCheckBox, "Internal testing. Check only if you know what you do");
            this.testFixAnimationsCheckBox.UseVisualStyleBackColor = true;
            // 
            // checkBoxMergeCameras
            // 
            this.checkBoxMergeCameras.AutoSize = true;
            this.checkBoxMergeCameras.Checked = true;
            this.checkBoxMergeCameras.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxMergeCameras.Location = new System.Drawing.Point(9, 65);
            this.checkBoxMergeCameras.Name = "checkBoxMergeCameras";
            this.checkBoxMergeCameras.Size = new System.Drawing.Size(100, 17);
            this.checkBoxMergeCameras.TabIndex = 18;
            this.checkBoxMergeCameras.Text = "Merge Cameras";
            this.toolTip1.SetToolTip(this.checkBoxMergeCameras, "Merge cameras from M2I to result model");
            this.checkBoxMergeCameras.UseVisualStyleBackColor = true;
            // 
            // workingDirectoryBrowseButton
            // 
            this.workingDirectoryBrowseButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.workingDirectoryBrowseButton.Location = new System.Drawing.Point(265, 20);
            this.workingDirectoryBrowseButton.Name = "workingDirectoryBrowseButton";
            this.workingDirectoryBrowseButton.Size = new System.Drawing.Size(37, 22);
            this.workingDirectoryBrowseButton.TabIndex = 6;
            this.workingDirectoryBrowseButton.Text = "...";
            this.workingDirectoryBrowseButton.UseVisualStyleBackColor = true;
            this.workingDirectoryBrowseButton.Click += new System.EventHandler(this.WorkingDirectoryBrowseButton_Click);
            // 
            // workingDirectoryGroupBox
            // 
            this.workingDirectoryGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.workingDirectoryGroupBox.Controls.Add(this.workingDirectoryBrowseButton);
            this.workingDirectoryGroupBox.Controls.Add(this.workingDirectoryTextBox);
            this.workingDirectoryGroupBox.Location = new System.Drawing.Point(12, 41);
            this.workingDirectoryGroupBox.Name = "workingDirectoryGroupBox";
            this.workingDirectoryGroupBox.Size = new System.Drawing.Size(307, 56);
            this.workingDirectoryGroupBox.TabIndex = 29;
            this.workingDirectoryGroupBox.TabStop = false;
            this.workingDirectoryGroupBox.Text = "Working directory";
            this.toolTip1.SetToolTip(this.workingDirectoryGroupBox, "Directory containing extracted Wow files");
            // 
            // checkBoxMergeAttachments
            // 
            this.checkBoxMergeAttachments.AutoSize = true;
            this.checkBoxMergeAttachments.Checked = true;
            this.checkBoxMergeAttachments.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxMergeAttachments.Location = new System.Drawing.Point(9, 42);
            this.checkBoxMergeAttachments.Name = "checkBoxMergeAttachments";
            this.checkBoxMergeAttachments.Size = new System.Drawing.Size(118, 17);
            this.checkBoxMergeAttachments.TabIndex = 17;
            this.checkBoxMergeAttachments.Text = "Merge Attachments";
            this.toolTip1.SetToolTip(this.checkBoxMergeAttachments, "Merge attachments from M2I to result model");
            this.checkBoxMergeAttachments.UseVisualStyleBackColor = true;
            // 
            // checkBoxMergeBones
            // 
            this.checkBoxMergeBones.AutoSize = true;
            this.checkBoxMergeBones.Checked = true;
            this.checkBoxMergeBones.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxMergeBones.Location = new System.Drawing.Point(9, 19);
            this.checkBoxMergeBones.Name = "checkBoxMergeBones";
            this.checkBoxMergeBones.Size = new System.Drawing.Size(89, 17);
            this.checkBoxMergeBones.TabIndex = 16;
            this.checkBoxMergeBones.Text = "Merge Bones";
            this.toolTip1.SetToolTip(this.checkBoxMergeBones, "Merge bones from M2I to result model");
            this.checkBoxMergeBones.UseVisualStyleBackColor = true;
            // 
            // forceExpansionComboBox
            // 
            this.forceExpansionComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.forceExpansionComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.forceExpansionComboBox.FormattingEnabled = true;
            this.forceExpansionComboBox.Location = new System.Drawing.Point(179, 18);
            this.forceExpansionComboBox.Name = "forceExpansionComboBox";
            this.forceExpansionComboBox.Size = new System.Drawing.Size(122, 21);
            this.forceExpansionComboBox.TabIndex = 0;
            this.toolTip1.SetToolTip(this.forceExpansionComboBox, "All input M2\'s will be treated accordingly");
            // 
            // mappingsDirectoryGroupBox
            // 
            this.mappingsDirectoryGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mappingsDirectoryGroupBox.Controls.Add(this.customFilesStartIndexLabel);
            this.mappingsDirectoryGroupBox.Controls.Add(this.customFilesStartIndexTextBox);
            this.mappingsDirectoryGroupBox.Controls.Add(this.mappingsDirectoryButton);
            this.mappingsDirectoryGroupBox.Controls.Add(this.mappingsDirectoryTextBox);
            this.mappingsDirectoryGroupBox.Location = new System.Drawing.Point(12, 165);
            this.mappingsDirectoryGroupBox.Name = "mappingsDirectoryGroupBox";
            this.mappingsDirectoryGroupBox.Size = new System.Drawing.Size(307, 80);
            this.mappingsDirectoryGroupBox.TabIndex = 31;
            this.mappingsDirectoryGroupBox.TabStop = false;
            this.mappingsDirectoryGroupBox.Text = "Mappings directory";
            this.toolTip1.SetToolTip(this.mappingsDirectoryGroupBox, "Directory where *.csv files are");
            // 
            // mappingsDirectoryButton
            // 
            this.mappingsDirectoryButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.mappingsDirectoryButton.Location = new System.Drawing.Point(265, 20);
            this.mappingsDirectoryButton.Name = "mappingsDirectoryButton";
            this.mappingsDirectoryButton.Size = new System.Drawing.Size(37, 22);
            this.mappingsDirectoryButton.TabIndex = 6;
            this.mappingsDirectoryButton.Text = "...";
            this.mappingsDirectoryButton.UseVisualStyleBackColor = true;
            this.mappingsDirectoryButton.Click += new System.EventHandler(this.MappingsDirectoryButton_Click);
            // 
            // mappingsDirectoryTextBox
            // 
            this.mappingsDirectoryTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mappingsDirectoryTextBox.Location = new System.Drawing.Point(9, 21);
            this.mappingsDirectoryTextBox.Name = "mappingsDirectoryTextBox";
            this.mappingsDirectoryTextBox.Size = new System.Drawing.Size(250, 20);
            this.mappingsDirectoryTextBox.TabIndex = 5;
            // 
            // saveButton
            // 
            this.saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.saveButton.Location = new System.Drawing.Point(151, 404);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(81, 29);
            this.saveButton.TabIndex = 27;
            this.saveButton.Text = "Save";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.SaveButton_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.checkBoxFixEdgeNormals);
            this.groupBox3.Controls.Add(this.checkBoxIgnoreOriginalMeshIndexes);
            this.groupBox3.Controls.Add(this.testFixAnimationsCheckBox);
            this.groupBox3.Controls.Add(this.checkBoxMergeCameras);
            this.groupBox3.Controls.Add(this.checkBoxMergeAttachments);
            this.groupBox3.Controls.Add(this.checkBoxMergeBones);
            this.groupBox3.Location = new System.Drawing.Point(12, 306);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(307, 90);
            this.groupBox3.TabIndex = 26;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Import Settings";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(6, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(135, 19);
            this.label1.TabIndex = 1;
            this.label1.Text = "Force Input M2 Expansion:";
            // 
            // closeButton
            // 
            this.closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.closeButton.Location = new System.Drawing.Point(238, 404);
            this.closeButton.Name = "closeButton";
            this.closeButton.Size = new System.Drawing.Size(81, 29);
            this.closeButton.TabIndex = 28;
            this.closeButton.Text = "Close";
            this.closeButton.UseVisualStyleBackColor = true;
            this.closeButton.Click += new System.EventHandler(this.CloseButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.forceExpansionComboBox);
            this.groupBox1.Location = new System.Drawing.Point(12, 251);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(307, 53);
            this.groupBox1.TabIndex = 25;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Export Settings";
            // 
            // profilesComboBox
            // 
            this.profilesComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.profilesComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.profilesComboBox.FormattingEnabled = true;
            this.profilesComboBox.Location = new System.Drawing.Point(76, 12);
            this.profilesComboBox.Name = "profilesComboBox";
            this.profilesComboBox.Size = new System.Drawing.Size(195, 21);
            this.profilesComboBox.TabIndex = 32;
            this.profilesComboBox.SelectedIndexChanged += new System.EventHandler(this.ProfilesComboBox_SelectedIndexChanged);
            // 
            // profilesLabel
            // 
            this.profilesLabel.AutoSize = true;
            this.profilesLabel.Location = new System.Drawing.Point(34, 15);
            this.profilesLabel.Name = "profilesLabel";
            this.profilesLabel.Size = new System.Drawing.Size(36, 13);
            this.profilesLabel.TabIndex = 33;
            this.profilesLabel.Text = "Profile";
            this.profilesLabel.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // editProfilesButton
            // 
            this.editProfilesButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.editProfilesButton.Location = new System.Drawing.Point(277, 12);
            this.editProfilesButton.Name = "editProfilesButton";
            this.editProfilesButton.Size = new System.Drawing.Size(37, 22);
            this.editProfilesButton.TabIndex = 7;
            this.editProfilesButton.Text = "Edit";
            this.editProfilesButton.UseVisualStyleBackColor = true;
            this.editProfilesButton.Click += new System.EventHandler(this.EditProfilesButton_Click);
            // 
            // customFilesStartIndexTextBox
            // 
            this.customFilesStartIndexTextBox.Location = new System.Drawing.Point(9, 48);
            this.customFilesStartIndexTextBox.Name = "customFilesStartIndexTextBox";
            this.customFilesStartIndexTextBox.Size = new System.Drawing.Size(100, 20);
            this.customFilesStartIndexTextBox.TabIndex = 7;
            // 
            // customFilesStartIndexLabel
            // 
            this.customFilesStartIndexLabel.AutoSize = true;
            this.customFilesStartIndexLabel.Location = new System.Drawing.Point(115, 51);
            this.customFilesStartIndexLabel.Name = "customFilesStartIndexLabel";
            this.customFilesStartIndexLabel.Size = new System.Drawing.Size(114, 13);
            this.customFilesStartIndexLabel.TabIndex = 8;
            this.customFilesStartIndexLabel.Text = "Custom files start index";
            // 
            // SettingsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(331, 439);
            this.Controls.Add(this.mappingsDirectoryGroupBox);
            this.Controls.Add(this.editProfilesButton);
            this.Controls.Add(this.profilesLabel);
            this.Controls.Add(this.profilesComboBox);
            this.Controls.Add(this.outputDirectoryGroupBox);
            this.Controls.Add(this.workingDirectoryGroupBox);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.closeButton);
            this.Controls.Add(this.groupBox1);
            this.Name = "SettingsForm";
            this.Text = "Settings";
            this.outputDirectoryGroupBox.ResumeLayout(false);
            this.outputDirectoryGroupBox.PerformLayout();
            this.workingDirectoryGroupBox.ResumeLayout(false);
            this.workingDirectoryGroupBox.PerformLayout();
            this.mappingsDirectoryGroupBox.ResumeLayout(false);
            this.mappingsDirectoryGroupBox.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button outputDirectoryBrowseButton;
        private System.Windows.Forms.GroupBox outputDirectoryGroupBox;
        private System.Windows.Forms.TextBox outputDirectoryTextBox;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.TextBox workingDirectoryTextBox;
        private System.Windows.Forms.CheckBox checkBoxFixEdgeNormals;
        private System.Windows.Forms.CheckBox checkBoxIgnoreOriginalMeshIndexes;
        private System.Windows.Forms.CheckBox testFixAnimationsCheckBox;
        private System.Windows.Forms.CheckBox checkBoxMergeCameras;
        private System.Windows.Forms.Button workingDirectoryBrowseButton;
        private System.Windows.Forms.GroupBox workingDirectoryGroupBox;
        private System.Windows.Forms.CheckBox checkBoxMergeAttachments;
        private System.Windows.Forms.CheckBox checkBoxMergeBones;
        private System.Windows.Forms.ComboBox forceExpansionComboBox;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button closeButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox profilesComboBox;
        private System.Windows.Forms.Label profilesLabel;
        private System.Windows.Forms.Button editProfilesButton;
        private System.Windows.Forms.GroupBox mappingsDirectoryGroupBox;
        private System.Windows.Forms.Button mappingsDirectoryButton;
        private System.Windows.Forms.TextBox mappingsDirectoryTextBox;
        private System.Windows.Forms.Label customFilesStartIndexLabel;
        private System.Windows.Forms.TextBox customFilesStartIndexTextBox;
    }
}