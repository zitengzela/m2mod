namespace M2ModRedux
{
    using M2Lib;
    using System;
    using System.ComponentModel;
    using System.Drawing;
    using System.Runtime.CompilerServices;
    using System.Runtime.ExceptionServices;
    using System.Runtime.InteropServices;
    using System.Windows.Forms;

    public class Form1 : Form
    {
        private Button buttonGo;
        private Button buttonInputM2Browse;
        private Button buttonInputM2IBrowse;
        private Button buttonOutputM2Browse;
        private Button buttonOutputM2IBrowse;
        private CheckBox checkBoxMergeAttachments;
        private CheckBox checkBoxMergeBones;
        private CheckBox checkBoxMergeCameras;
        private IContainer components;
        private GroupBox groupBox2;
        private Label label1;
        private Label label2;
        private Label label3;
        private Label label7;
        private OpenFileDialog openFileDialog1;
        private SaveFileDialog saveFileDialog1;
        private TextBox textBoxInputM2;
        private TextBox textBoxInputM2I;
        private TextBox textBoxOutputM2;
        private TextBox textBoxOutputM2I;
        private ToolTip toolTip1;

        public Form1()
        {
            try
            {
                this.InitializeComponent();
            }
            fault
            {
                base.Dispose(true);
            }
        }

        private void ~Form1()
        {
            IContainer components = this.components;
            if (components != null)
            {
                components.Dispose();
            }
        }

        private unsafe void buttonGo_Click(object sender, EventArgs e)
        {
            M2* modopt(IsConst) modopt(IsConst) mPtr;
            string text = this.buttonGo.Text;
            this.buttonGo.Enabled = false;
            this.buttonGo.Text = "Working...";
            this.buttonGo.Refresh();
            M2* mPtr2 = @new(0xc54);
            try
            {
                if (mPtr2 != null)
                {
                    mPtr = M2Lib.M2.{ctor}(mPtr2);
                }
                else
                {
                    mPtr = 0;
                }
            }
            fault
            {
                delete((void*) mPtr2);
            }
            if (this.textBoxInputM2.Text.Length > 0)
            {
                IntPtr hglobal = Marshal.StringToHGlobalUni(this.textBoxInputM2.Text);
                EError error4 = M2Lib.M2.Load(mPtr, (char modopt(IsConst)*) hglobal.ToPointer());
                Marshal.FreeHGlobal(hglobal);
                if (error4 != ((EError) 0))
                {
                    MessageBox.Show(new string(M2Lib.GetErrorText(error4)));
                    this.buttonGo.Enabled = true;
                    return;
                }
            }
            else
            {
                MessageBox.Show("Error: No input M2 file Specified.");
            }
            if (this.textBoxOutputM2I.Text.Length > 0)
            {
                IntPtr ptr3 = Marshal.StringToHGlobalUni(this.textBoxOutputM2I.Text);
                EError error3 = M2Lib.M2.ExportM2Intermediate(mPtr, (char*) ptr3.ToPointer());
                Marshal.FreeHGlobal(ptr3);
                if (error3 != ((EError) 0))
                {
                    MessageBox.Show(new string(M2Lib.GetErrorText(error3)));
                    this.buttonGo.Enabled = true;
                    return;
                }
            }
            if (this.textBoxInputM2I.Text.Length > 0)
            {
                IntPtr ptr2 = Marshal.StringToHGlobalUni(this.textBoxInputM2I.Text);
                byte num3 = (byte) !this.checkBoxMergeCameras.Checked;
                byte num2 = (byte) !this.checkBoxMergeAttachments.Checked;
                byte num = (byte) !this.checkBoxMergeBones.Checked;
                EError error2 = M2Lib.M2.ImportM2Intermediate(mPtr, (char*) ptr2.ToPointer(), (bool) num, (bool) num2, (bool) num3);
                Marshal.FreeHGlobal(ptr2);
                if (error2 != ((EError) 0))
                {
                    MessageBox.Show(new string(M2Lib.GetErrorText(error2)));
                    this.buttonGo.Enabled = true;
                    return;
                }
            }
            if (this.textBoxOutputM2.Text.Length > 0)
            {
                IntPtr ptr = Marshal.StringToHGlobalUni(this.textBoxOutputM2.Text);
                EError error = M2Lib.M2.Save(mPtr, (char modopt(IsConst)*) ptr.ToPointer());
                Marshal.FreeHGlobal(ptr);
                if (error != ((EError) 0))
                {
                    MessageBox.Show(new string(M2Lib.GetErrorText(error)));
                    this.buttonGo.Enabled = true;
                    return;
                }
            }
            M2* mPtr3 = mPtr;
            if (mPtr != null)
            {
                try
                {
                    M2Lib.M2Element.{dtor}(mPtr + 0xc3c);
                }
                fault
                {
                    ___CxxCallUnwindVecDtor((void *(void*, uint, int, void *(void*))) __ehvec_dtor, (void*) (mPtr3 + 0x938), 0x18, 0x1f, M2Lib.M2Element.{dtor});
                }
                __ehvec_dtor(mPtr + 0x938, 0x18, 0x1f, M2Lib.M2Element.{dtor});
                delete(mPtr);
            }
            this.buttonGo.Text = text;
            this.buttonGo.Enabled = true;
        }

        private void buttonInputM2Browse_Click(object sender, EventArgs e)
        {
            this.openFileDialog1.Filter = "M2 Files|*.m2|All Files|*.*";
            this.openFileDialog1.FileName = this.textBoxInputM2.Text;
            if (this.openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                this.textBoxInputM2.Text = this.openFileDialog1.FileName;
            }
        }

        private void buttonInputM2IBrowse_Click(object sender, EventArgs e)
        {
            this.openFileDialog1.Filter = "M2I Files|*.m2i|All Files|*.*";
            this.openFileDialog1.FileName = this.textBoxInputM2I.Text;
            if (this.openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                this.textBoxInputM2I.Text = this.openFileDialog1.FileName;
            }
        }

        private void buttonOutputM2Browse_Click(object sender, EventArgs e)
        {
            this.saveFileDialog1.Filter = "M2 Files|*.m2|All Files|*.*";
            this.saveFileDialog1.FileName = this.textBoxOutputM2.Text;
            if (this.saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                this.textBoxOutputM2.Text = this.saveFileDialog1.FileName;
            }
        }

        private void buttonOutputM2IBrowse_Click(object sender, EventArgs e)
        {
            this.saveFileDialog1.Filter = "M2I Files|*.m2i|All Files|*.*";
            this.saveFileDialog1.FileName = this.textBoxOutputM2I.Text;
            if (this.saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                this.textBoxOutputM2I.Text = this.saveFileDialog1.FileName;
            }
        }

        [HandleProcessCorruptedStateExceptions]
        protected override void Dispose([MarshalAs(UnmanagedType.U1)] bool flag1)
        {
            if (flag1)
            {
                try
                {
                    this.~Form1();
                    return;
                }
                finally
                {
                    base.Dispose(true);
                }
            }
            base.Dispose(false);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void InitializeComponent()
        {
            this.components = new Container();
            this.label1 = new Label();
            this.textBoxInputM2 = new TextBox();
            this.buttonInputM2Browse = new Button();
            this.buttonOutputM2Browse = new Button();
            this.textBoxOutputM2 = new TextBox();
            this.label2 = new Label();
            this.buttonInputM2IBrowse = new Button();
            this.textBoxInputM2I = new TextBox();
            this.label3 = new Label();
            this.buttonGo = new Button();
            this.buttonOutputM2IBrowse = new Button();
            this.textBoxOutputM2I = new TextBox();
            this.label7 = new Label();
            this.checkBoxMergeBones = new CheckBox();
            this.checkBoxMergeAttachments = new CheckBox();
            this.checkBoxMergeCameras = new CheckBox();
            this.groupBox2 = new GroupBox();
            this.openFileDialog1 = new OpenFileDialog();
            this.saveFileDialog1 = new SaveFileDialog();
            this.toolTip1 = new ToolTip(this.components);
            this.groupBox2.SuspendLayout();
            base.SuspendLayout();
            this.label1.AutoSize = true;
            Point point17 = new Point(0x1a, 0x19);
            this.label1.Location = point17;
            this.label1.Name = "label1";
            Size size19 = new Size(0x2e, 13);
            this.label1.Size = size19;
            this.label1.TabIndex = 0;
            this.label1.Text = "InputM2";
            this.toolTip1.SetToolTip(this.label1, "Required. Select an M2 for M2Mod to work with.");
            this.textBoxInputM2.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Top;
            Point point16 = new Point(0x4e, 0x16);
            this.textBoxInputM2.Location = point16;
            this.textBoxInputM2.Name = "textBoxInputM2";
            Size size18 = new Size(0x176, 20);
            this.textBoxInputM2.Size = size18;
            this.textBoxInputM2.TabIndex = 1;
            this.buttonInputM2Browse.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            Point point15 = new Point(0x1ca, 0x16);
            this.buttonInputM2Browse.Location = point15;
            this.buttonInputM2Browse.Name = "buttonInputM2Browse";
            Size size17 = new Size(0x4b, 20);
            this.buttonInputM2Browse.Size = size17;
            this.buttonInputM2Browse.TabIndex = 2;
            this.buttonInputM2Browse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonInputM2Browse, "Browse...");
            this.buttonInputM2Browse.UseVisualStyleBackColor = true;
            this.buttonInputM2Browse.Click += new EventHandler(this.buttonInputM2Browse_Click);
            this.buttonOutputM2Browse.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            Point point14 = new Point(0x1ca, 100);
            this.buttonOutputM2Browse.Location = point14;
            this.buttonOutputM2Browse.Name = "buttonOutputM2Browse";
            Size size16 = new Size(0x4b, 20);
            this.buttonOutputM2Browse.Size = size16;
            this.buttonOutputM2Browse.TabIndex = 11;
            this.buttonOutputM2Browse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonOutputM2Browse, "Browse...");
            this.buttonOutputM2Browse.UseVisualStyleBackColor = true;
            this.buttonOutputM2Browse.Click += new EventHandler(this.buttonOutputM2Browse_Click);
            this.textBoxOutputM2.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Top;
            Point point13 = new Point(0x4e, 100);
            this.textBoxOutputM2.Location = point13;
            this.textBoxOutputM2.Name = "textBoxOutputM2";
            Size size15 = new Size(0x176, 20);
            this.textBoxOutputM2.Size = size15;
            this.textBoxOutputM2.TabIndex = 10;
            this.label2.AutoSize = true;
            Point point12 = new Point(0x12, 0x67);
            this.label2.Location = point12;
            this.label2.Name = "label2";
            Size size14 = new Size(0x36, 13);
            this.label2.Size = size14;
            this.label2.TabIndex = 9;
            this.label2.Text = "OutputM2";
            this.toolTip1.SetToolTip(this.label2, "Optional. If set, this is where M2Mod will save the modified M2.");
            this.buttonInputM2IBrowse.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            Point point11 = new Point(0x1ca, 0x4a);
            this.buttonInputM2IBrowse.Location = point11;
            this.buttonInputM2IBrowse.Name = "buttonInputM2IBrowse";
            Size size13 = new Size(0x4b, 20);
            this.buttonInputM2IBrowse.Size = size13;
            this.buttonInputM2IBrowse.TabIndex = 8;
            this.buttonInputM2IBrowse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonInputM2IBrowse, "Browse...");
            this.buttonInputM2IBrowse.UseVisualStyleBackColor = true;
            this.buttonInputM2IBrowse.Click += new EventHandler(this.buttonInputM2IBrowse_Click);
            this.textBoxInputM2I.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Top;
            Point point10 = new Point(0x4e, 0x4a);
            this.textBoxInputM2I.Location = point10;
            this.textBoxInputM2I.Name = "textBoxInputM2I";
            Size size12 = new Size(0x176, 20);
            this.textBoxInputM2I.Size = size12;
            this.textBoxInputM2I.TabIndex = 7;
            this.label3.AutoSize = true;
            Point point9 = new Point(0x17, 0x4d);
            this.label3.Location = point9;
            this.label3.Name = "label3";
            Size size11 = new Size(0x31, 13);
            this.label3.Size = size11;
            this.label3.TabIndex = 6;
            this.label3.Text = "InputM2I";
            this.toolTip1.SetToolTip(this.label3, "Optional. If set, M2Mod will merge InputM2 with InputM2I to create a modified M2 which will be saved to OutputM2.");
            this.buttonGo.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Top;
            this.buttonGo.Font = new Font("Microsoft Sans Serif", 8.25f, FontStyle.Bold, GraphicsUnit.Point, 0);
            Point point8 = new Point(0x4e, 0xce);
            this.buttonGo.Location = point8;
            this.buttonGo.Name = "buttonGo";
            Size size10 = new Size(0x176, 0x26);
            this.buttonGo.Size = size10;
            this.buttonGo.TabIndex = 15;
            this.buttonGo.Text = "Go!";
            this.toolTip1.SetToolTip(this.buttonGo, "Click this to perform operations.");
            this.buttonGo.UseVisualStyleBackColor = true;
            this.buttonGo.Click += new EventHandler(this.buttonGo_Click);
            this.buttonOutputM2IBrowse.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            Point point7 = new Point(0x1ca, 0x30);
            this.buttonOutputM2IBrowse.Location = point7;
            this.buttonOutputM2IBrowse.Name = "buttonOutputM2IBrowse";
            Size size9 = new Size(0x4b, 20);
            this.buttonOutputM2IBrowse.Size = size9;
            this.buttonOutputM2IBrowse.TabIndex = 5;
            this.buttonOutputM2IBrowse.Text = "...";
            this.toolTip1.SetToolTip(this.buttonOutputM2IBrowse, "Browse...");
            this.buttonOutputM2IBrowse.UseVisualStyleBackColor = true;
            this.buttonOutputM2IBrowse.Click += new EventHandler(this.buttonOutputM2IBrowse_Click);
            this.textBoxOutputM2I.Anchor = AnchorStyles.Right | AnchorStyles.Left | AnchorStyles.Top;
            Point point6 = new Point(0x4e, 0x30);
            this.textBoxOutputM2I.Location = point6;
            this.textBoxOutputM2I.Name = "textBoxOutputM2I";
            Size size8 = new Size(0x176, 20);
            this.textBoxOutputM2I.Size = size8;
            this.textBoxOutputM2I.TabIndex = 4;
            this.label7.AutoSize = true;
            Point point5 = new Point(15, 0x33);
            this.label7.Location = point5;
            this.label7.Name = "label7";
            Size size7 = new Size(0x39, 13);
            this.label7.Size = size7;
            this.label7.TabIndex = 3;
            this.label7.Text = "OutputM2I";
            this.toolTip1.SetToolTip(this.label7, "Optional. If set, M2Mod will convert InputM2 to an M2I and save it here.");
            this.checkBoxMergeBones.AutoSize = true;
            this.checkBoxMergeBones.Checked = true;
            this.checkBoxMergeBones.CheckState = CheckState.Checked;
            Point point4 = new Point(0x4e, 0x7e);
            this.checkBoxMergeBones.Location = point4;
            this.checkBoxMergeBones.Name = "checkBoxMergeBones";
            Size size6 = new Size(0x56, 0x11);
            this.checkBoxMergeBones.Size = size6;
            this.checkBoxMergeBones.TabIndex = 12;
            this.checkBoxMergeBones.Text = "MergeBones";
            this.toolTip1.SetToolTip(this.checkBoxMergeBones, "Check to overwrite bones from InputM2 with those from InputM2I.");
            this.checkBoxMergeBones.UseVisualStyleBackColor = true;
            this.checkBoxMergeAttachments.AutoSize = true;
            this.checkBoxMergeAttachments.Checked = true;
            this.checkBoxMergeAttachments.CheckState = CheckState.Checked;
            Point point3 = new Point(0x4e, 0x95);
            this.checkBoxMergeAttachments.Location = point3;
            this.checkBoxMergeAttachments.Name = "checkBoxMergeAttachments";
            Size size5 = new Size(0x73, 0x11);
            this.checkBoxMergeAttachments.Size = size5;
            this.checkBoxMergeAttachments.TabIndex = 13;
            this.checkBoxMergeAttachments.Text = "MergeAttachments";
            this.toolTip1.SetToolTip(this.checkBoxMergeAttachments, "Check to overwrite attachments from InputM2 with those from InputM2I.");
            this.checkBoxMergeAttachments.UseVisualStyleBackColor = true;
            this.checkBoxMergeCameras.AutoSize = true;
            this.checkBoxMergeCameras.Checked = true;
            this.checkBoxMergeCameras.CheckState = CheckState.Checked;
            Point point2 = new Point(0x4e, 0xac);
            this.checkBoxMergeCameras.Location = point2;
            this.checkBoxMergeCameras.Name = "checkBoxMergeCameras";
            Size size4 = new Size(0x61, 0x11);
            this.checkBoxMergeCameras.Size = size4;
            this.checkBoxMergeCameras.TabIndex = 14;
            this.checkBoxMergeCameras.Text = "MergeCameras";
            this.toolTip1.SetToolTip(this.checkBoxMergeCameras, "Check to overwrite cameras from InputM2 with those from InputM2I.");
            this.checkBoxMergeCameras.UseVisualStyleBackColor = true;
            this.groupBox2.Controls.Add(this.textBoxInputM2);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.buttonOutputM2IBrowse);
            this.groupBox2.Controls.Add(this.checkBoxMergeCameras);
            this.groupBox2.Controls.Add(this.textBoxOutputM2I);
            this.groupBox2.Controls.Add(this.textBoxOutputM2);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.buttonInputM2IBrowse);
            this.groupBox2.Controls.Add(this.textBoxInputM2I);
            this.groupBox2.Controls.Add(this.checkBoxMergeAttachments);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.buttonInputM2Browse);
            this.groupBox2.Controls.Add(this.buttonOutputM2Browse);
            this.groupBox2.Controls.Add(this.checkBoxMergeBones);
            this.groupBox2.Controls.Add(this.buttonGo);
            this.groupBox2.Controls.Add(this.label2);
            Point point = new Point(12, 12);
            this.groupBox2.Location = point;
            this.groupBox2.Name = "groupBox2";
            Size size3 = new Size(0x221, 0x102);
            this.groupBox2.Size = size3;
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Configuration";
            this.openFileDialog1.FileName = "openFileDialog1";
            this.toolTip1.AutoPopDelay = 0x1388;
            Color color2 = Color.FromArgb(0x26, 0x10, 4);
            this.toolTip1.BackColor = color2;
            Color color = Color.FromArgb(0xf2, 0x5e, 0x83);
            this.toolTip1.ForeColor = color;
            this.toolTip1.InitialDelay = 1;
            this.toolTip1.ReshowDelay = 1;
            this.toolTip1.UseAnimation = false;
            this.toolTip1.UseFading = false;
            SizeF ef = new SizeF(6f, 13f);
            base.AutoScaleDimensions = ef;
            base.AutoScaleMode = AutoScaleMode.Font;
            Size size2 = new Size(0x239, 0x11a);
            base.ClientSize = size2;
            base.Controls.Add(this.groupBox2);
            Size size = new Size(500, 320);
            this.MinimumSize = size;
            base.Name = "Form1";
            this.Text = "M2Mod Redux - v4.5";
            base.Load += new EventHandler(this.Form1_Load);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            base.ResumeLayout(false);
        }
    }
}

