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
            this.textBoxOutputM2I = new System.Windows.Forms.TextBox();
            this.panelImputM2Exp = new System.Windows.Forms.Panel();
            this.textBoxInputM2Exp = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.buttonInputM2ExpBrowse = new System.Windows.Forms.Button();
            this.panelReplaceM2 = new System.Windows.Forms.Panel();
            this.textBoxReplaceM2 = new System.Windows.Forms.TextBox();
            this.buttonReplaceM2Browse = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.importButtonGo = new System.Windows.Forms.Button();
            this.preloadButtonContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.importCancelButton = new System.Windows.Forms.Button();
            this.extraworkPanel = new System.Windows.Forms.Panel();
            this.panelInputM2Import = new System.Windows.Forms.Panel();
            this.textBoxInputM2Imp = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.buttonInputM2ImpBrowse = new System.Windows.Forms.Button();
            this.panelInputM2I = new System.Windows.Forms.Panel();
            this.label3 = new System.Windows.Forms.Label();
            this.buttonInputM2IBrowse = new System.Windows.Forms.Button();
            this.textBoxInputM2I = new System.Windows.Forms.TextBox();
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
            this.tabImport = new System.Windows.Forms.TabPage();
            this.checkBoxReplaceM2 = new System.Windows.Forms.CheckBox();
            this.panelOutputM2I = new System.Windows.Forms.Panel();
            this.buttonOutputM2IBrowse = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.tabExport = new System.Windows.Forms.TabPage();
            this.goButtonContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.customMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.profilesComboBox = new System.Windows.Forms.ComboBox();
            this.profileLabel = new System.Windows.Forms.Label();
            this.exportButtonGo = new M2Mod.Controls.SplitButton();
            this.importButtonPreload = new M2Mod.Controls.SplitButton();
            this.logTextBox = new M2Mod.LogTextBox();
            this.panelImputM2Exp.SuspendLayout();
            this.panelReplaceM2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.preloadButtonContextMenuStrip.SuspendLayout();
            this.panelInputM2Import.SuspendLayout();
            this.panelInputM2I.SuspendLayout();
            this.cascTabPage.SuspendLayout();
            this.panel2.SuspendLayout();
            this.tabLog.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.tabImport.SuspendLayout();
            this.panelOutputM2I.SuspendLayout();
            this.tabExport.SuspendLayout();
            this.goButtonContextMenuStrip.SuspendLayout();
            this.tabControl.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBoxOutputM2I
            // 
            this.textBoxOutputM2I.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxOutputM2I.Location = new System.Drawing.Point(89, 3);
            this.textBoxOutputM2I.Name = "textBoxOutputM2I";
            this.textBoxOutputM2I.Size = new System.Drawing.Size(376, 20);
            this.textBoxOutputM2I.TabIndex = 7;
            // 
            // panelImputM2Exp
            // 
            this.panelImputM2Exp.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelImputM2Exp.Controls.Add(this.textBoxInputM2Exp);
            this.panelImputM2Exp.Controls.Add(this.label1);
            this.panelImputM2Exp.Controls.Add(this.buttonInputM2ExpBrowse);
            this.panelImputM2Exp.Location = new System.Drawing.Point(6, 9);
            this.panelImputM2Exp.Name = "panelImputM2Exp";
            this.panelImputM2Exp.Size = new System.Drawing.Size(529, 24);
            this.panelImputM2Exp.TabIndex = 19;
            // 
            // textBoxInputM2Exp
            // 
            this.textBoxInputM2Exp.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxInputM2Exp.Location = new System.Drawing.Point(88, 3);
            this.textBoxInputM2Exp.Name = "textBoxInputM2Exp";
            this.textBoxInputM2Exp.Size = new System.Drawing.Size(376, 20);
            this.textBoxInputM2Exp.TabIndex = 4;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 6);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Source M2";
            this.toolTip1.SetToolTip(this.label1, "Required. Select an M2 for M2Mod to work with.");
            // 
            // buttonInputM2ExpBrowse
            // 
            this.buttonInputM2ExpBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonInputM2ExpBrowse.Location = new System.Drawing.Point(470, 3);
            this.buttonInputM2ExpBrowse.Name = "buttonInputM2ExpBrowse";
            this.buttonInputM2ExpBrowse.Size = new System.Drawing.Size(55, 20);
            this.buttonInputM2ExpBrowse.TabIndex = 5;
            this.buttonInputM2ExpBrowse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonInputM2ExpBrowse, "Browse...");
            this.buttonInputM2ExpBrowse.UseVisualStyleBackColor = true;
            this.buttonInputM2ExpBrowse.Click += new System.EventHandler(this.ButtonInputM2ExpBrowse_Click);
            // 
            // panelReplaceM2
            // 
            this.panelReplaceM2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelReplaceM2.Controls.Add(this.textBoxReplaceM2);
            this.panelReplaceM2.Controls.Add(this.buttonReplaceM2Browse);
            this.panelReplaceM2.Enabled = false;
            this.panelReplaceM2.Location = new System.Drawing.Point(91, 70);
            this.panelReplaceM2.Name = "panelReplaceM2";
            this.panelReplaceM2.Size = new System.Drawing.Size(446, 25);
            this.panelReplaceM2.TabIndex = 25;
            // 
            // textBoxReplaceM2
            // 
            this.textBoxReplaceM2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxReplaceM2.Location = new System.Drawing.Point(3, 3);
            this.textBoxReplaceM2.Name = "textBoxReplaceM2";
            this.textBoxReplaceM2.Size = new System.Drawing.Size(378, 20);
            this.textBoxReplaceM2.TabIndex = 13;
            this.toolTip1.SetToolTip(this.textBoxReplaceM2, "Destination path for M2");
            // 
            // buttonReplaceM2Browse
            // 
            this.buttonReplaceM2Browse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonReplaceM2Browse.Location = new System.Drawing.Point(387, 2);
            this.buttonReplaceM2Browse.Name = "buttonReplaceM2Browse";
            this.buttonReplaceM2Browse.Size = new System.Drawing.Size(55, 20);
            this.buttonReplaceM2Browse.TabIndex = 14;
            this.buttonReplaceM2Browse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonReplaceM2Browse, "Browse...");
            this.buttonReplaceM2Browse.UseVisualStyleBackColor = true;
            this.buttonReplaceM2Browse.Click += new System.EventHandler(this.ButtonReplaceM2Browse_Click);
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.importButtonGo);
            this.panel1.Controls.Add(this.importButtonPreload);
            this.panel1.Controls.Add(this.importCancelButton);
            this.panel1.Location = new System.Drawing.Point(7, 179);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(527, 61);
            this.panel1.TabIndex = 31;
            // 
            // importButtonGo
            // 
            this.importButtonGo.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.importButtonGo.Enabled = false;
            this.importButtonGo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.importButtonGo.Location = new System.Drawing.Point(207, 9);
            this.importButtonGo.Name = "importButtonGo";
            this.importButtonGo.Size = new System.Drawing.Size(142, 38);
            this.importButtonGo.TabIndex = 27;
            this.importButtonGo.Text = "Go!";
            this.toolTip1.SetToolTip(this.importButtonGo, "Produce M2");
            this.importButtonGo.UseVisualStyleBackColor = true;
            this.importButtonGo.Click += new System.EventHandler(this.ImportButtonGo_Click);
            // 
            // preloadButtonContextMenuStrip
            // 
            this.preloadButtonContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1});
            this.preloadButtonContextMenuStrip.Name = "goButtonContextMenuStrip";
            this.preloadButtonContextMenuStrip.Size = new System.Drawing.Size(131, 26);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(130, 22);
            this.toolStripMenuItem1.Text = "Customize";
            this.toolStripMenuItem1.Click += new System.EventHandler(this.ToolStripMenuItem1_Click);
            // 
            // importCancelButton
            // 
            this.importCancelButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.importCancelButton.Enabled = false;
            this.importCancelButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.importCancelButton.Location = new System.Drawing.Point(359, 9);
            this.importCancelButton.Name = "importCancelButton";
            this.importCancelButton.Size = new System.Drawing.Size(142, 38);
            this.importCancelButton.TabIndex = 29;
            this.importCancelButton.Text = "Cancel";
            this.toolTip1.SetToolTip(this.importCancelButton, "Cancel preload");
            this.importCancelButton.UseVisualStyleBackColor = true;
            this.importCancelButton.Click += new System.EventHandler(this.ImportCancelButton_Click);
            // 
            // extraworkPanel
            // 
            this.extraworkPanel.Enabled = false;
            this.extraworkPanel.Location = new System.Drawing.Point(7, 101);
            this.extraworkPanel.Name = "extraworkPanel";
            this.extraworkPanel.Size = new System.Drawing.Size(530, 73);
            this.extraworkPanel.TabIndex = 30;
            // 
            // panelInputM2Import
            // 
            this.panelInputM2Import.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelInputM2Import.Controls.Add(this.textBoxInputM2Imp);
            this.panelInputM2Import.Controls.Add(this.label5);
            this.panelInputM2Import.Controls.Add(this.buttonInputM2ImpBrowse);
            this.panelInputM2Import.Location = new System.Drawing.Point(6, 9);
            this.panelInputM2Import.Name = "panelInputM2Import";
            this.panelInputM2Import.Size = new System.Drawing.Size(531, 24);
            this.panelInputM2Import.TabIndex = 26;
            // 
            // textBoxInputM2Imp
            // 
            this.textBoxInputM2Imp.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxInputM2Imp.Location = new System.Drawing.Point(88, 3);
            this.textBoxInputM2Imp.Name = "textBoxInputM2Imp";
            this.textBoxInputM2Imp.Size = new System.Drawing.Size(378, 20);
            this.textBoxInputM2Imp.TabIndex = 4;
            this.toolTip1.SetToolTip(this.textBoxInputM2Imp, "Path to source M2");
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 6);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(59, 13);
            this.label5.TabIndex = 3;
            this.label5.Text = "Source M2";
            this.toolTip1.SetToolTip(this.label5, "Required. Select an M2 for M2Mod to work with.");
            // 
            // buttonInputM2ImpBrowse
            // 
            this.buttonInputM2ImpBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonInputM2ImpBrowse.Location = new System.Drawing.Point(472, 3);
            this.buttonInputM2ImpBrowse.Name = "buttonInputM2ImpBrowse";
            this.buttonInputM2ImpBrowse.Size = new System.Drawing.Size(55, 20);
            this.buttonInputM2ImpBrowse.TabIndex = 5;
            this.buttonInputM2ImpBrowse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonInputM2ImpBrowse, "Browse...");
            this.buttonInputM2ImpBrowse.UseVisualStyleBackColor = true;
            this.buttonInputM2ImpBrowse.Click += new System.EventHandler(this.ButtonInputM2ExpBrowse_Click);
            // 
            // panelInputM2I
            // 
            this.panelInputM2I.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelInputM2I.Controls.Add(this.label3);
            this.panelInputM2I.Controls.Add(this.buttonInputM2IBrowse);
            this.panelInputM2I.Controls.Add(this.textBoxInputM2I);
            this.panelInputM2I.Location = new System.Drawing.Point(6, 36);
            this.panelInputM2I.Name = "panelInputM2I";
            this.panelInputM2I.Size = new System.Drawing.Size(531, 30);
            this.panelInputM2I.TabIndex = 23;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 8);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(62, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Source M2I";
            this.toolTip1.SetToolTip(this.label3, "Optional. If set, M2Mod will merge InputM2 with InputM2I to create a modified M2 " +
        "which will be saved to OutputM2.");
            // 
            // buttonInputM2IBrowse
            // 
            this.buttonInputM2IBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonInputM2IBrowse.Location = new System.Drawing.Point(472, 5);
            this.buttonInputM2IBrowse.Name = "buttonInputM2IBrowse";
            this.buttonInputM2IBrowse.Size = new System.Drawing.Size(55, 20);
            this.buttonInputM2IBrowse.TabIndex = 11;
            this.buttonInputM2IBrowse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonInputM2IBrowse, "Browse...");
            this.buttonInputM2IBrowse.UseVisualStyleBackColor = true;
            this.buttonInputM2IBrowse.Click += new System.EventHandler(this.ButtonInputM2IBrowse_Click);
            // 
            // textBoxInputM2I
            // 
            this.textBoxInputM2I.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxInputM2I.Location = new System.Drawing.Point(88, 5);
            this.textBoxInputM2I.Name = "textBoxInputM2I";
            this.textBoxInputM2I.Size = new System.Drawing.Size(378, 20);
            this.textBoxInputM2I.TabIndex = 10;
            this.toolTip1.SetToolTip(this.textBoxInputM2I, "Path to M2I");
            // 
            // cascTabPage
            // 
            this.cascTabPage.Controls.Add(this.getLatestMappingLinkLabel);
            this.cascTabPage.Controls.Add(this.unloadMappingsButton);
            this.cascTabPage.Controls.Add(this.panel2);
            this.cascTabPage.Controls.Add(this.loadMappingsButton);
            this.cascTabPage.Location = new System.Drawing.Point(4, 22);
            this.cascTabPage.Name = "cascTabPage";
            this.cascTabPage.Padding = new System.Windows.Forms.Padding(3);
            this.cascTabPage.Size = new System.Drawing.Size(560, 230);
            this.cascTabPage.TabIndex = 3;
            this.cascTabPage.Text = "CASC";
            this.cascTabPage.UseVisualStyleBackColor = true;
            // 
            // getLatestMappingLinkLabel
            // 
            this.getLatestMappingLinkLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.getLatestMappingLinkLabel.AutoSize = true;
            this.getLatestMappingLinkLabel.Location = new System.Drawing.Point(229, 8);
            this.getLatestMappingLinkLabel.Name = "getLatestMappingLinkLabel";
            this.getLatestMappingLinkLabel.Size = new System.Drawing.Size(100, 13);
            this.getLatestMappingLinkLabel.TabIndex = 12;
            this.getLatestMappingLinkLabel.TabStop = true;
            this.getLatestMappingLinkLabel.Text = "Get latest mappings";
            this.getLatestMappingLinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.getLatestMappingLinkLabel_LinkClicked);
            // 
            // unloadMappingsButton
            // 
            this.unloadMappingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.unloadMappingsButton.Location = new System.Drawing.Point(330, 3);
            this.unloadMappingsButton.Name = "unloadMappingsButton";
            this.unloadMappingsButton.Size = new System.Drawing.Size(108, 23);
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
            this.panel2.Location = new System.Drawing.Point(3, 32);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(553, 64);
            this.panel2.TabIndex = 10;
            // 
            // fileTestLabel
            // 
            this.fileTestLabel.AutoSize = true;
            this.fileTestLabel.Location = new System.Drawing.Point(6, 8);
            this.fileTestLabel.Name = "fileTestLabel";
            this.fileTestLabel.Size = new System.Drawing.Size(121, 13);
            this.fileTestLabel.TabIndex = 5;
            this.fileTestLabel.Text = "Input path or FileDataId:";
            // 
            // fileTestButton
            // 
            this.fileTestButton.Location = new System.Drawing.Point(9, 31);
            this.fileTestButton.Name = "fileTestButton";
            this.fileTestButton.Size = new System.Drawing.Size(75, 23);
            this.fileTestButton.TabIndex = 8;
            this.fileTestButton.Text = "Test";
            this.fileTestButton.UseVisualStyleBackColor = true;
            this.fileTestButton.Click += new System.EventHandler(this.FileTestButton_Click);
            // 
            // testOutputTextBox
            // 
            this.testOutputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.testOutputTextBox.Location = new System.Drawing.Point(133, 32);
            this.testOutputTextBox.Name = "testOutputTextBox";
            this.testOutputTextBox.Size = new System.Drawing.Size(416, 20);
            this.testOutputTextBox.TabIndex = 7;
            // 
            // testInputTextBox
            // 
            this.testInputTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.testInputTextBox.Location = new System.Drawing.Point(133, 6);
            this.testInputTextBox.Name = "testInputTextBox";
            this.testInputTextBox.Size = new System.Drawing.Size(416, 20);
            this.testInputTextBox.TabIndex = 6;
            this.testInputTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TestInputTextBox_KeyDown);
            // 
            // loadMappingsButton
            // 
            this.loadMappingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.loadMappingsButton.Location = new System.Drawing.Point(444, 3);
            this.loadMappingsButton.Name = "loadMappingsButton";
            this.loadMappingsButton.Size = new System.Drawing.Size(108, 23);
            this.loadMappingsButton.TabIndex = 3;
            this.loadMappingsButton.Text = "Load mappings";
            this.loadMappingsButton.UseVisualStyleBackColor = true;
            this.loadMappingsButton.Click += new System.EventHandler(this.LoadListfileButton_Click);
            // 
            // tabLog
            // 
            this.tabLog.Controls.Add(this.logTextBox);
            this.tabLog.Controls.Add(this.clearButton);
            this.tabLog.Location = new System.Drawing.Point(4, 22);
            this.tabLog.Name = "tabLog";
            this.tabLog.Padding = new System.Windows.Forms.Padding(3);
            this.tabLog.Size = new System.Drawing.Size(560, 230);
            this.tabLog.TabIndex = 2;
            this.tabLog.Text = "Log";
            this.tabLog.UseVisualStyleBackColor = true;
            // 
            // clearButton
            // 
            this.clearButton.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.clearButton.Location = new System.Drawing.Point(241, 204);
            this.clearButton.Name = "clearButton";
            this.clearButton.Size = new System.Drawing.Size(75, 23);
            this.clearButton.TabIndex = 1;
            this.clearButton.Text = "Clear";
            this.clearButton.UseVisualStyleBackColor = true;
            this.clearButton.Click += new System.EventHandler(this.ClearButton_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.BackColor = System.Drawing.Color.Transparent;
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
            this.statusStrip1.Location = new System.Drawing.Point(0, 279);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(569, 22);
            this.statusStrip1.TabIndex = 34;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(0, 17);
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.SystemColors.Control;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.toolsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(569, 24);
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
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // settingsToolStripMenuItem
            // 
            this.settingsToolStripMenuItem.Name = "settingsToolStripMenuItem";
            this.settingsToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.settingsToolStripMenuItem.Text = "Settings";
            this.settingsToolStripMenuItem.Click += new System.EventHandler(this.SettingsToolStripMenuItem_Click);
            // 
            // loadProfilesToolStripMenuItem
            // 
            this.loadProfilesToolStripMenuItem.Name = "loadProfilesToolStripMenuItem";
            this.loadProfilesToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.loadProfilesToolStripMenuItem.Text = "Load profiles";
            this.loadProfilesToolStripMenuItem.Click += new System.EventHandler(this.LoadProfilesToolStripMenuItem_Click);
            // 
            // checkUpdatesToolStripMenuItem
            // 
            this.checkUpdatesToolStripMenuItem.Name = "checkUpdatesToolStripMenuItem";
            this.checkUpdatesToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.checkUpdatesToolStripMenuItem.Text = "Check updates";
            this.checkUpdatesToolStripMenuItem.Click += new System.EventHandler(this.CheckUpdatesToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
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
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(46, 20);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // compareBonesToolStripMenuItem
            // 
            this.compareBonesToolStripMenuItem.Name = "compareBonesToolStripMenuItem";
            this.compareBonesToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.compareBonesToolStripMenuItem.Text = "Compare models";
            this.compareBonesToolStripMenuItem.Click += new System.EventHandler(this.CompareModelsToolStripMenuItem_Click);
            // 
            // remapReferencesToolStripMenuItem
            // 
            this.remapReferencesToolStripMenuItem.Name = "remapReferencesToolStripMenuItem";
            this.remapReferencesToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.remapReferencesToolStripMenuItem.Text = "Remap references";
            this.remapReferencesToolStripMenuItem.Click += new System.EventHandler(this.remapReferencesToolStripMenuItem_Click);
            // 
            // tXIDRemoverToolStripMenuItem
            // 
            this.tXIDRemoverToolStripMenuItem.Name = "tXIDRemoverToolStripMenuItem";
            this.tXIDRemoverToolStripMenuItem.Size = new System.Drawing.Size(168, 22);
            this.tXIDRemoverToolStripMenuItem.Text = "TXID remover";
            this.tXIDRemoverToolStripMenuItem.Click += new System.EventHandler(this.tXIDRemoverToolStripMenuItem_Click);
            // 
            // tabImport
            // 
            this.tabImport.Controls.Add(this.checkBoxReplaceM2);
            this.tabImport.Controls.Add(this.panelReplaceM2);
            this.tabImport.Controls.Add(this.panel1);
            this.tabImport.Controls.Add(this.extraworkPanel);
            this.tabImport.Controls.Add(this.panelInputM2Import);
            this.tabImport.Controls.Add(this.panelInputM2I);
            this.tabImport.Location = new System.Drawing.Point(4, 22);
            this.tabImport.Name = "tabImport";
            this.tabImport.Padding = new System.Windows.Forms.Padding(3);
            this.tabImport.Size = new System.Drawing.Size(560, 230);
            this.tabImport.TabIndex = 1;
            this.tabImport.Text = "Import";
            this.tabImport.UseVisualStyleBackColor = true;
            // 
            // checkBoxReplaceM2
            // 
            this.checkBoxReplaceM2.AutoSize = true;
            this.checkBoxReplaceM2.Location = new System.Drawing.Point(7, 73);
            this.checkBoxReplaceM2.Name = "checkBoxReplaceM2";
            this.checkBoxReplaceM2.Size = new System.Drawing.Size(84, 17);
            this.checkBoxReplaceM2.TabIndex = 15;
            this.checkBoxReplaceM2.Text = "Replace M2";
            this.toolTip1.SetToolTip(this.checkBoxReplaceM2, "If you are swapping models, select target M2 that you would like to swap from.");
            this.checkBoxReplaceM2.UseVisualStyleBackColor = true;
            this.checkBoxReplaceM2.CheckedChanged += new System.EventHandler(this.CheckBoxReplaceM2_CheckedChanged);
            // 
            // panelOutputM2I
            // 
            this.panelOutputM2I.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelOutputM2I.Controls.Add(this.buttonOutputM2IBrowse);
            this.panelOutputM2I.Controls.Add(this.textBoxOutputM2I);
            this.panelOutputM2I.Controls.Add(this.label7);
            this.panelOutputM2I.Location = new System.Drawing.Point(5, 38);
            this.panelOutputM2I.Name = "panelOutputM2I";
            this.panelOutputM2I.Size = new System.Drawing.Size(530, 26);
            this.panelOutputM2I.TabIndex = 20;
            // 
            // buttonOutputM2IBrowse
            // 
            this.buttonOutputM2IBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonOutputM2IBrowse.Location = new System.Drawing.Point(471, 3);
            this.buttonOutputM2IBrowse.Name = "buttonOutputM2IBrowse";
            this.buttonOutputM2IBrowse.Size = new System.Drawing.Size(55, 20);
            this.buttonOutputM2IBrowse.TabIndex = 8;
            this.buttonOutputM2IBrowse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonOutputM2IBrowse, "Browse...");
            this.buttonOutputM2IBrowse.UseVisualStyleBackColor = true;
            this.buttonOutputM2IBrowse.Click += new System.EventHandler(this.ButtonOutputM2IBrowse_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(8, 6);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(59, 13);
            this.label7.TabIndex = 6;
            this.label7.Text = "Target M2I";
            this.toolTip1.SetToolTip(this.label7, "Optional. If set, M2Mod will convert InputM2 to an M2I and save it here.");
            // 
            // tabExport
            // 
            this.tabExport.Controls.Add(this.panelImputM2Exp);
            this.tabExport.Controls.Add(this.panelOutputM2I);
            this.tabExport.Controls.Add(this.exportButtonGo);
            this.tabExport.Location = new System.Drawing.Point(4, 22);
            this.tabExport.Name = "tabExport";
            this.tabExport.Padding = new System.Windows.Forms.Padding(3);
            this.tabExport.Size = new System.Drawing.Size(560, 230);
            this.tabExport.TabIndex = 0;
            this.tabExport.Text = "Export";
            this.tabExport.UseVisualStyleBackColor = true;
            // 
            // goButtonContextMenuStrip
            // 
            this.goButtonContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.customMenuItem});
            this.goButtonContextMenuStrip.Name = "goButtonContextMenuStrip";
            this.goButtonContextMenuStrip.Size = new System.Drawing.Size(131, 26);
            // 
            // customMenuItem
            // 
            this.customMenuItem.Name = "customMenuItem";
            this.customMenuItem.Size = new System.Drawing.Size(130, 22);
            this.customMenuItem.Text = "Customize";
            this.customMenuItem.Click += new System.EventHandler(this.CustomMenuItem_Click);
            // 
            // tabControl
            // 
            this.tabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl.Controls.Add(this.tabExport);
            this.tabControl.Controls.Add(this.tabImport);
            this.tabControl.Controls.Add(this.cascTabPage);
            this.tabControl.Controls.Add(this.tabLog);
            this.tabControl.Location = new System.Drawing.Point(1, 27);
            this.tabControl.Name = "tabControl";
            this.tabControl.Padding = new System.Drawing.Point(10, 3);
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(568, 256);
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
            this.profilesComboBox.Location = new System.Drawing.Point(444, 3);
            this.profilesComboBox.Name = "profilesComboBox";
            this.profilesComboBox.Size = new System.Drawing.Size(121, 21);
            this.profilesComboBox.TabIndex = 36;
            this.profilesComboBox.SelectedIndexChanged += new System.EventHandler(this.ProfilesComboBox_SelectedIndexChanged);
            // 
            // profileLabel
            // 
            this.profileLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.profileLabel.AutoSize = true;
            this.profileLabel.BackColor = System.Drawing.Color.Transparent;
            this.profileLabel.Location = new System.Drawing.Point(402, 6);
            this.profileLabel.Name = "profileLabel";
            this.profileLabel.Size = new System.Drawing.Size(36, 13);
            this.profileLabel.TabIndex = 37;
            this.profileLabel.Text = "Profile";
            // 
            // exportButtonGo
            // 
            this.exportButtonGo.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.exportButtonGo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.exportButtonGo.Location = new System.Drawing.Point(214, 188);
            this.exportButtonGo.Menu = this.goButtonContextMenuStrip;
            this.exportButtonGo.Name = "exportButtonGo";
            this.exportButtonGo.Size = new System.Drawing.Size(142, 38);
            this.exportButtonGo.TabIndex = 26;
            this.exportButtonGo.Text = "Go!";
            this.toolTip1.SetToolTip(this.exportButtonGo, "Click this to perform operations.");
            this.exportButtonGo.UseVisualStyleBackColor = true;
            this.exportButtonGo.Click += new System.EventHandler(this.ExportButtonGo_Click);
            // 
            // importButtonPreload
            // 
            this.importButtonPreload.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.importButtonPreload.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.importButtonPreload.Location = new System.Drawing.Point(54, 9);
            this.importButtonPreload.Menu = this.preloadButtonContextMenuStrip;
            this.importButtonPreload.Name = "importButtonPreload";
            this.importButtonPreload.Size = new System.Drawing.Size(142, 38);
            this.importButtonPreload.TabIndex = 28;
            this.importButtonPreload.Text = "Preload";
            this.toolTip1.SetToolTip(this.importButtonPreload, "Load source M2 and import M2I");
            this.importButtonPreload.UseVisualStyleBackColor = true;
            this.importButtonPreload.Click += new System.EventHandler(this.ImportButtonPreload_Click);
            // 
            // logTextBox
            // 
            this.logTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.logTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.logTextBox.DetectUrls = false;
            this.logTextBox.InlcudeTimestamp = true;
            this.logTextBox.Location = new System.Drawing.Point(7, 6);
            this.logTextBox.Name = "logTextBox";
            this.logTextBox.ReadOnly = true;
            this.logTextBox.Size = new System.Drawing.Size(549, 194);
            this.logTextBox.TabIndex = 2;
            this.logTextBox.Text = "";
            this.logTextBox.UseColors = true;
            // 
            // M2ModForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(569, 301);
            this.Controls.Add(this.profileLabel);
            this.Controls.Add(this.profilesComboBox);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.tabControl);
            this.Name = "M2ModForm";
            this.Text = "M2Mod";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.M2Mod_FormClosing);
            this.panelImputM2Exp.ResumeLayout(false);
            this.panelImputM2Exp.PerformLayout();
            this.panelReplaceM2.ResumeLayout(false);
            this.panelReplaceM2.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.preloadButtonContextMenuStrip.ResumeLayout(false);
            this.panelInputM2Import.ResumeLayout(false);
            this.panelInputM2Import.PerformLayout();
            this.panelInputM2I.ResumeLayout(false);
            this.panelInputM2I.PerformLayout();
            this.cascTabPage.ResumeLayout(false);
            this.cascTabPage.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.tabLog.ResumeLayout(false);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.tabImport.ResumeLayout(false);
            this.tabImport.PerformLayout();
            this.panelOutputM2I.ResumeLayout(false);
            this.panelOutputM2I.PerformLayout();
            this.tabExport.ResumeLayout(false);
            this.goButtonContextMenuStrip.ResumeLayout(false);
            this.tabControl.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxOutputM2I;
        private System.Windows.Forms.Panel panelImputM2Exp;
        private System.Windows.Forms.TextBox textBoxInputM2Exp;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.Button buttonInputM2ExpBrowse;
        private System.Windows.Forms.Panel panelReplaceM2;
        private System.Windows.Forms.TextBox textBoxReplaceM2;
        private System.Windows.Forms.Button buttonReplaceM2Browse;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button importButtonGo;
        private SplitButton importButtonPreload;
        private System.Windows.Forms.Button importCancelButton;
        private System.Windows.Forms.Panel extraworkPanel;
        private System.Windows.Forms.Panel panelInputM2Import;
        private System.Windows.Forms.TextBox textBoxInputM2Imp;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button buttonInputM2ImpBrowse;
        private System.Windows.Forms.Panel panelInputM2I;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button buttonInputM2IBrowse;
        private System.Windows.Forms.TextBox textBoxInputM2I;
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
        private System.Windows.Forms.TabPage tabImport;
        private System.Windows.Forms.CheckBox checkBoxReplaceM2;
        private System.Windows.Forms.Panel panelOutputM2I;
        private System.Windows.Forms.Button buttonOutputM2IBrowse;
        private System.Windows.Forms.Label label7;
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
    }
}

