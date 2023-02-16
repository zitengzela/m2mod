using M2Mod.Controls;

namespace M2Mod
{
    partial class M2ModForm
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
            this.preloadButtonContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.cascTabPage = new System.Windows.Forms.TabPage();
            this.getLatestMappingLinkLabel = new System.Windows.Forms.LinkLabel();
            this.unloadMappingsButton = new System.Windows.Forms.Button();
            this.panel2 = new System.Windows.Forms.Panel();
            this.fileTestLabel = new System.Windows.Forms.Label();
            this.fileTestButton = new System.Windows.Forms.Button();
            this.testOutputTextBox = new System.Windows.Forms.TextBox();
            this.testInputTextBox = new System.Windows.Forms.TextBox();
            this.loadMappingsButton = new System.Windows.Forms.Button();
            this.tabLog = new System.Windows.Forms.TabPage();
            this.logTextBox = new M2Mod.LogTextBox();
            this.clearButton = new System.Windows.Forms.Button();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.settingsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.loadProfilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.checkUpdatesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.compareBonesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.remapReferencesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tXIDRemoverToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabExport = new System.Windows.Forms.TabPage();
            this.splitButton3 = new M2Mod.Controls.SplitButton();
            this.goButtonContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.customMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.splitButton2 = new M2Mod.Controls.SplitButton();
            this.splitButton1 = new M2Mod.Controls.SplitButton();
            this.exportButtonGo = new M2Mod.Controls.SplitButton();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.profilesComboBox = new System.Windows.Forms.ComboBox();
            this.profileLabel = new System.Windows.Forms.Label();
            this.preloadButtonContextMenuStrip.SuspendLayout();
            this.cascTabPage.SuspendLayout();
            this.panel2.SuspendLayout();
            this.tabLog.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.tabExport.SuspendLayout();
            this.goButtonContextMenuStrip.SuspendLayout();
            this.tabControl.SuspendLayout();
            this.SuspendLayout();
            // 
            // preloadButtonContextMenuStrip
            // 
            this.preloadButtonContextMenuStrip.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.preloadButtonContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1});
            this.preloadButtonContextMenuStrip.Name = "goButtonContextMenuStrip";
            this.preloadButtonContextMenuStrip.Size = new System.Drawing.Size(148, 28);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(147, 24);
            this.toolStripMenuItem1.Text = "Customize";
            this.toolStripMenuItem1.Click += new System.EventHandler(this.ToolStripMenuItem1_Click);
            // 
            // cascTabPage
            // 
            this.cascTabPage.Controls.Add(this.getLatestMappingLinkLabel);
            this.cascTabPage.Controls.Add(this.unloadMappingsButton);
            this.cascTabPage.Controls.Add(this.panel2);
            this.cascTabPage.Controls.Add(this.loadMappingsButton);
            this.cascTabPage.Location = new System.Drawing.Point(4, 25);
            this.cascTabPage.Margin = new System.Windows.Forms.Padding(4);
            this.cascTabPage.Name = "cascTabPage";
            this.cascTabPage.Padding = new System.Windows.Forms.Padding(4);
            this.cascTabPage.Size = new System.Drawing.Size(749, 286);
            this.cascTabPage.TabIndex = 3;
            this.cascTabPage.Text = "CASC";
            this.cascTabPage.UseVisualStyleBackColor = true;
            // 
            // getLatestMappingLinkLabel
            // 
            this.getLatestMappingLinkLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.getLatestMappingLinkLabel.AutoSize = true;
            this.getLatestMappingLinkLabel.Location = new System.Drawing.Point(305, 10);
            this.getLatestMappingLinkLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.getLatestMappingLinkLabel.Name = "getLatestMappingLinkLabel";
            this.getLatestMappingLinkLabel.Size = new System.Drawing.Size(134, 17);
            this.getLatestMappingLinkLabel.TabIndex = 12;
            this.getLatestMappingLinkLabel.TabStop = true;
            this.getLatestMappingLinkLabel.Text = "Get latest mappings";
            this.getLatestMappingLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.getLatestMappingLinkLabel_LinkClicked);
            // 
            // unloadMappingsButton
            // 
            this.unloadMappingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.unloadMappingsButton.Location = new System.Drawing.Point(440, 4);
            this.unloadMappingsButton.Margin = new System.Windows.Forms.Padding(4);
            this.unloadMappingsButton.Name = "unloadMappingsButton";
            this.unloadMappingsButton.Size = new System.Drawing.Size(144, 28);
            this.unloadMappingsButton.TabIndex = 11;
            this.unloadMappingsButton.Text = "Unload mappings";
            this.unloadMappingsButton.UseVisualStyleBackColor = true;
            this.unloadMappingsButton.Click += new System.EventHandler(this.LoadMappingsButton_Click);
            // 
            // panel2
            // 
            this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel2.Controls.Add(this.fileTestLabel);
            this.panel2.Controls.Add(this.fileTestButton);
            this.panel2.Controls.Add(this.testOutputTextBox);
            this.panel2.Controls.Add(this.testInputTextBox);
            this.panel2.Location = new System.Drawing.Point(4, 39);
            this.panel2.Margin = new System.Windows.Forms.Padding(4);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(737, 79);
            this.panel2.TabIndex = 10;
            // 
            // fileTestLabel
            // 
            this.fileTestLabel.AutoSize = true;
            this.fileTestLabel.Location = new System.Drawing.Point(8, 10);
            this.fileTestLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.fileTestLabel.Name = "fileTestLabel";
            this.fileTestLabel.Size = new System.Drawing.Size(159, 17);
            this.fileTestLabel.TabIndex = 5;
            this.fileTestLabel.Text = "Input path or FileDataId:";
            // 
            // fileTestButton
            // 
            this.fileTestButton.Location = new System.Drawing.Point(12, 38);
            this.fileTestButton.Margin = new System.Windows.Forms.Padding(4);
            this.fileTestButton.Name = "fileTestButton";
            this.fileTestButton.Size = new System.Drawing.Size(100, 28);
            this.fileTestButton.TabIndex = 8;
            this.fileTestButton.Text = "Test";
            this.fileTestButton.UseVisualStyleBackColor = true;
            this.fileTestButton.Click += new System.EventHandler(this.FileTestButton_Click);
            // 
            // testOutputTextBox
            // 
            this.testOutputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.testOutputTextBox.Location = new System.Drawing.Point(177, 39);
            this.testOutputTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.testOutputTextBox.Name = "testOutputTextBox";
            this.testOutputTextBox.Size = new System.Drawing.Size(553, 22);
            this.testOutputTextBox.TabIndex = 7;
            // 
            // testInputTextBox
            // 
            this.testInputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.testInputTextBox.Location = new System.Drawing.Point(177, 7);
            this.testInputTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.testInputTextBox.Name = "testInputTextBox";
            this.testInputTextBox.Size = new System.Drawing.Size(553, 22);
            this.testInputTextBox.TabIndex = 6;
            this.testInputTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TestInputTextBox_KeyDown);
            // 
            // loadMappingsButton
            // 
            this.loadMappingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.loadMappingsButton.Location = new System.Drawing.Point(592, 4);
            this.loadMappingsButton.Margin = new System.Windows.Forms.Padding(4);
            this.loadMappingsButton.Name = "loadMappingsButton";
            this.loadMappingsButton.Size = new System.Drawing.Size(144, 28);
            this.loadMappingsButton.TabIndex = 3;
            this.loadMappingsButton.Text = "Load mappings";
            this.loadMappingsButton.UseVisualStyleBackColor = true;
            this.loadMappingsButton.Click += new System.EventHandler(this.LoadListfileButton_Click);
            // 
            // tabLog
            // 
            this.tabLog.Controls.Add(this.logTextBox);
            this.tabLog.Controls.Add(this.clearButton);
            this.tabLog.Location = new System.Drawing.Point(4, 25);
            this.tabLog.Margin = new System.Windows.Forms.Padding(4);
            this.tabLog.Name = "tabLog";
            this.tabLog.Padding = new System.Windows.Forms.Padding(4);
            this.tabLog.Size = new System.Drawing.Size(749, 286);
            this.tabLog.TabIndex = 2;
            this.tabLog.Text = "Log";
            this.tabLog.UseVisualStyleBackColor = true;
            // 
            // logTextBox
            // 
            this.logTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.logTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.logTextBox.DetectUrls = false;
            this.logTextBox.InlcudeTimestamp = true;
            this.logTextBox.Location = new System.Drawing.Point(9, 7);
            this.logTextBox.Margin = new System.Windows.Forms.Padding(4);
            this.logTextBox.Name = "logTextBox";
            this.logTextBox.ReadOnly = true;
            this.logTextBox.Size = new System.Drawing.Size(731, 238);
            this.logTextBox.TabIndex = 2;
            this.logTextBox.Text = "";
            this.logTextBox.UseColors = true;
            // 
            // clearButton
            // 
            this.clearButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.clearButton.Location = new System.Drawing.Point(321, 251);
            this.clearButton.Margin = new System.Windows.Forms.Padding(4);
            this.clearButton.Name = "clearButton";
            this.clearButton.Size = new System.Drawing.Size(100, 28);
            this.clearButton.TabIndex = 1;
            this.clearButton.Text = "Clear";
            this.clearButton.UseVisualStyleBackColor = true;
            this.clearButton.Click += new System.EventHandler(this.ClearButton_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.BackColor = System.Drawing.Color.Transparent;
            this.statusStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
            this.statusStrip1.Location = new System.Drawing.Point(0, 348);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Padding = new System.Windows.Forms.Padding(1, 0, 19, 0);
            this.statusStrip1.Size = new System.Drawing.Size(759, 22);
            this.statusStrip1.TabIndex = 34;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(0, 16);
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.SystemColors.Control;
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.toolsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(8, 2, 0, 2);
            this.menuStrip1.Size = new System.Drawing.Size(759, 28);
            this.menuStrip1.TabIndex = 35;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.settingsToolStripMenuItem,
            this.loadProfilesToolStripMenuItem,
            this.checkUpdatesToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(46, 24);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            this.settingsToolStripMenuItem.Size = new System.Drawing.Size(188, 26);
            this.settingsToolStripMenuItem.Text = "Settings";
            this.settingsToolStripMenuItem.Click += new System.EventHandler(this.SettingsToolStripMenuItem_Click);
            // 
            // loadProfilesToolStripMenuItem
            // 
            this.loadProfilesToolStripMenuItem.Name = "loadProfilesToolStripMenuItem";
            this.loadProfilesToolStripMenuItem.Size = new System.Drawing.Size(188, 26);
            this.loadProfilesToolStripMenuItem.Text = "Load profiles";
            this.loadProfilesToolStripMenuItem.Click += new System.EventHandler(this.LoadProfilesToolStripMenuItem_Click);
            // 
            // checkUpdatesToolStripMenuItem
            // 
            this.checkUpdatesToolStripMenuItem.Name = "checkUpdatesToolStripMenuItem";
            this.checkUpdatesToolStripMenuItem.Size = new System.Drawing.Size(188, 26);
            this.checkUpdatesToolStripMenuItem.Text = "Check updates";
            this.checkUpdatesToolStripMenuItem.Click += new System.EventHandler(this.CheckUpdatesToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(188, 26);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.ExitToolStripMenuItem_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.compareBonesToolStripMenuItem,
            this.remapReferencesToolStripMenuItem,
            this.tXIDRemoverToolStripMenuItem});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(58, 24);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // compareBonesToolStripMenuItem
            // 
            this.compareBonesToolStripMenuItem.Name = "compareBonesToolStripMenuItem";
            this.compareBonesToolStripMenuItem.Size = new System.Drawing.Size(211, 26);
            this.compareBonesToolStripMenuItem.Text = "Compare models";
            this.compareBonesToolStripMenuItem.Click += new System.EventHandler(this.CompareModelsToolStripMenuItem_Click);
            // 
            // remapReferencesToolStripMenuItem
            // 
            this.remapReferencesToolStripMenuItem.Name = "remapReferencesToolStripMenuItem";
            this.remapReferencesToolStripMenuItem.Size = new System.Drawing.Size(211, 26);
            this.remapReferencesToolStripMenuItem.Text = "Remap references";
            this.remapReferencesToolStripMenuItem.Click += new System.EventHandler(this.remapReferencesToolStripMenuItem_Click);
            // 
            // tXIDRemoverToolStripMenuItem
            // 
            this.tXIDRemoverToolStripMenuItem.Name = "tXIDRemoverToolStripMenuItem";
            this.tXIDRemoverToolStripMenuItem.Size = new System.Drawing.Size(211, 26);
            this.tXIDRemoverToolStripMenuItem.Text = "TXID remover";
            this.tXIDRemoverToolStripMenuItem.Click += new System.EventHandler(this.tXIDRemoverToolStripMenuItem_Click);
            // 
            // tabExport
            // 
            this.tabExport.Controls.Add(this.splitButton3);
            this.tabExport.Controls.Add(this.splitButton2);
            this.tabExport.Controls.Add(this.splitButton1);
            this.tabExport.Controls.Add(this.exportButtonGo);
            this.tabExport.Location = new System.Drawing.Point(4, 25);
            this.tabExport.Margin = new System.Windows.Forms.Padding(4);
            this.tabExport.Name = "tabExport";
            this.tabExport.Padding = new System.Windows.Forms.Padding(4);
            this.tabExport.Size = new System.Drawing.Size(749, 286);
            this.tabExport.TabIndex = 0;
            this.tabExport.Text = "M2 -> M2I & M2I -> M2";
            this.tabExport.UseVisualStyleBackColor = true;
            // 
            // splitButton3
            // 
            this.splitButton3.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.splitButton3.Enabled = false;
            this.splitButton3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.splitButton3.Location = new System.Drawing.Point(147, 219);
            this.splitButton3.Margin = new System.Windows.Forms.Padding(4);
            this.splitButton3.Menu = this.goButtonContextMenuStrip;
            this.splitButton3.Name = "splitButton3";
            this.splitButton3.Size = new System.Drawing.Size(431, 39);
            this.splitButton3.TabIndex = 29;
            this.splitButton3.Text = "Use Multi-Converter";
            this.toolTip1.SetToolTip(this.splitButton3, "Click this to perform operations.");
            this.splitButton3.UseVisualStyleBackColor = true;
            // 
            // goButtonContextMenuStrip
            // 
            this.goButtonContextMenuStrip.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.goButtonContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.customMenuItem});
            this.goButtonContextMenuStrip.Name = "goButtonContextMenuStrip";
            this.goButtonContextMenuStrip.Size = new System.Drawing.Size(148, 28);
            // 
            // customMenuItem
            // 
            this.customMenuItem.Name = "customMenuItem";
            this.customMenuItem.Size = new System.Drawing.Size(147, 24);
            this.customMenuItem.Text = "Customize";
            this.customMenuItem.Click += new System.EventHandler(this.CustomMenuItem_Click);
            // 
            // splitButton2
            // 
            this.splitButton2.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.splitButton2.Enabled = false;
            this.splitButton2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.splitButton2.Location = new System.Drawing.Point(147, 101);
            this.splitButton2.Margin = new System.Windows.Forms.Padding(4);
            this.splitButton2.Menu = this.goButtonContextMenuStrip;
            this.splitButton2.Name = "splitButton2";
            this.splitButton2.Size = new System.Drawing.Size(431, 39);
            this.splitButton2.TabIndex = 28;
            this.splitButton2.Text = "M2 to WoD";
            this.toolTip1.SetToolTip(this.splitButton2, "Click this to perform operations.");
            this.splitButton2.UseVisualStyleBackColor = true;
            this.splitButton2.Click += new System.EventHandler(this.splitButton2_Click_1);
            // 
            // splitButton1
            // 
            this.splitButton1.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.splitButton1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.splitButton1.Location = new System.Drawing.Point(147, 30);
            this.splitButton1.Margin = new System.Windows.Forms.Padding(4);
            this.splitButton1.Menu = this.goButtonContextMenuStrip;
            this.splitButton1.Name = "splitButton1";
            this.splitButton1.Size = new System.Drawing.Size(431, 64);
            this.splitButton1.TabIndex = 27;
            this.splitButton1.Text = "TXID REMOVER";
            this.toolTip1.SetToolTip(this.splitButton1, "Click this to perform operations.");
            this.splitButton1.UseVisualStyleBackColor = true;
            this.splitButton1.Click += new System.EventHandler(this.splitButton1_Click);
            // 
            // exportButtonGo
            // 
            this.exportButtonGo.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.exportButtonGo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.exportButtonGo.Location = new System.Drawing.Point(147, 148);
            this.exportButtonGo.Margin = new System.Windows.Forms.Padding(4);
            this.exportButtonGo.Menu = this.goButtonContextMenuStrip;
            this.exportButtonGo.Name = "exportButtonGo";
            this.exportButtonGo.Size = new System.Drawing.Size(431, 64);
            this.exportButtonGo.TabIndex = 26;
            this.exportButtonGo.Text = "M2->M2I then M2I->M2\r\n";
            this.toolTip1.SetToolTip(this.exportButtonGo, "Click this to perform operations.");
            this.exportButtonGo.UseVisualStyleBackColor = true;
            this.exportButtonGo.Click += new System.EventHandler(this.ExportButtonGo_Click);
            // 
            // tabControl
            // 
            this.tabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl.Controls.Add(this.tabExport);
            this.tabControl.Controls.Add(this.cascTabPage);
            this.tabControl.Controls.Add(this.tabLog);
            this.tabControl.Location = new System.Drawing.Point(1, 33);
            this.tabControl.Margin = new System.Windows.Forms.Padding(4);
            this.tabControl.Name = "tabControl";
            this.tabControl.Padding = new System.Drawing.Point(10, 3);
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(757, 315);
            this.tabControl.TabIndex = 33;
            // 
            // toolTip1
            // 
            this.toolTip1.AutomaticDelay = 300;
            this.toolTip1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(38)))), ((int)(((byte)(16)))), ((int)(((byte)(4)))));
            this.toolTip1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(242)))), ((int)(((byte)(94)))), ((int)(((byte)(131)))));
            this.toolTip1.UseAnimation = false;
            this.toolTip1.UseFading = false;
            // 
            // profilesComboBox
            // 
            this.profilesComboBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.profilesComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.profilesComboBox.FormattingEnabled = true;
            this.profilesComboBox.Location = new System.Drawing.Point(592, 4);
            this.profilesComboBox.Margin = new System.Windows.Forms.Padding(4);
            this.profilesComboBox.Name = "profilesComboBox";
            this.profilesComboBox.Size = new System.Drawing.Size(160, 24);
            this.profilesComboBox.TabIndex = 36;
            this.profilesComboBox.SelectedIndexChanged += new System.EventHandler(this.ProfilesComboBox_SelectedIndexChanged);
            // 
            // profileLabel
            // 
            this.profileLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.profileLabel.AutoSize = true;
            this.profileLabel.BackColor = System.Drawing.Color.Transparent;
            this.profileLabel.Location = new System.Drawing.Point(536, 7);
            this.profileLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.profileLabel.Name = "profileLabel";
            this.profileLabel.Size = new System.Drawing.Size(48, 17);
            this.profileLabel.TabIndex = 37;
            this.profileLabel.Text = "Profile";
            // 
            // M2ModForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(759, 370);
            this.Controls.Add(this.profileLabel);
            this.Controls.Add(this.profilesComboBox);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.tabControl);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "M2ModForm";
            this.Text = "M2Mod";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.M2Mod_FormClosing);
            this.preloadButtonContextMenuStrip.ResumeLayout(false);
            this.cascTabPage.ResumeLayout(false);
            this.cascTabPage.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.tabLog.ResumeLayout(false);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.tabExport.ResumeLayout(false);
            this.goButtonContextMenuStrip.ResumeLayout(false);
            this.tabControl.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.TabPage cascTabPage;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label fileTestLabel;
        private System.Windows.Forms.Button fileTestButton;
        private System.Windows.Forms.TextBox testOutputTextBox;
        private System.Windows.Forms.TextBox testInputTextBox;
        private System.Windows.Forms.Button loadMappingsButton;
        private System.Windows.Forms.TabPage tabLog;
        private LogTextBox logTextBox;
        private System.Windows.Forms.Button clearButton;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem settingsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem checkUpdatesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem compareBonesToolStripMenuItem;
        private System.Windows.Forms.TabPage tabExport;
        private SplitButton exportButtonGo;
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.ComboBox profilesComboBox;
        private System.Windows.Forms.Label profileLabel;
        private System.Windows.Forms.Button unloadMappingsButton;
        private System.Windows.Forms.ContextMenuStrip goButtonContextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem customMenuItem;
        private System.Windows.Forms.ContextMenuStrip preloadButtonContextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem loadProfilesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem tXIDRemoverToolStripMenuItem;
        private System.Windows.Forms.LinkLabel getLatestMappingLinkLabel;
        private System.Windows.Forms.ToolStripMenuItem remapReferencesToolStripMenuItem;
        private SplitButton splitButton1;
        private SplitButton splitButton2;
        private SplitButton splitButton3;
    }
}

