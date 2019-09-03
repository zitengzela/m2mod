using System;
using System.Drawing;
using System.Windows.Forms;

namespace M2Mod
{
    public partial class ErrorForm : Form
    {
        private MessageBoxIcon _messageIcon;

        public ErrorForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;
        }

        private void Button1_Click(object sender, EventArgs e)
        {
            this.DialogResult = checkBox1.Checked ? DialogResult.Ignore : DialogResult.OK;
        }

        private void ErrorForm_SizeChanged(object sender, EventArgs e)
        {
            UpdateTextbox(richTextBox1.Text);
        }

        private void ErrorForm_Resize(object sender, EventArgs e)
        {
            UpdateTextbox(richTextBox1.Text);
        }

        private static Size GetTextDimensions(Control control, Font font, string stringData)
        {
            using (var g = control.CreateGraphics())
            {
                var sizeF = g.MeasureString(stringData, font, control.Width);

                return new Size((int) Math.Ceiling(sizeF.Width), (int) Math.Ceiling(sizeF.Height));
            }
        }

        private void UpdateTextbox(string text)
        {
            var dimensions = GetTextDimensions(richTextBox1, richTextBox1.Font, text);

            var maxHeight = ClientSize.Height - 20 - 35;

            richTextBox1.Size = new Size(richTextBox1.Size.Width, Math.Min(dimensions.Height, maxHeight));

            richTextBox1.Location = new Point(richTextBox1.Location.X, 20 + maxHeight / 2 - richTextBox1.Size.Height / 2);

            richTextBox1.ScrollBars = dimensions.Height > richTextBox1.Height ? RichTextBoxScrollBars.Vertical : RichTextBoxScrollBars.None;
        }

        private void ErrorForm_Load(object sender, EventArgs e)
        {
            ActiveControl = button1;
        }

        private MessageBoxIcon MessageIcon
        {
            set
            {
                _messageIcon = value;

                Icon icon;
                switch (_messageIcon)
                {
                    case MessageBoxIcon.Warning:
                        icon = SystemIcons.Warning;
                        break;
                    case MessageBoxIcon.Error:
                        icon = SystemIcons.Error;
                        break;
                    case MessageBoxIcon.Question:
                        icon = SystemIcons.Question;
                        break;
                    case MessageBoxIcon.Asterisk:
                        icon = SystemIcons.Asterisk;
                        break;
                    default:
                        icon = SystemIcons.Asterisk;
                        break;
                }

                if (icon != null)
                {
                    pictureBox1.Image = icon.ToBitmap();
                    Icon = icon;
                }
            }
        }

        public static DialogResult Show(string text, string title, MessageBoxIcon icon)
        {
            var form = new ErrorForm();
            if (string.IsNullOrEmpty(title))
            {
                switch (icon)
                {
                    case MessageBoxIcon.Warning:
                        title = "Warning";
                        break;
                    case MessageBoxIcon.Error:
                        title = "Error";
                        break;
                    case MessageBoxIcon.Question:
                        title = "Question";
                        break;
                    case MessageBoxIcon.Asterisk:
                        title = "Information";
                        break;
                    default:
                        title = "Information";
                        break;
                }
            }

            form.Text = title;
            form.MessageIcon = icon;

            form.UpdateTextbox(text);

            form.richTextBox1.AppendText(text);
            form.richTextBox1.SelectionStart = form.richTextBox1.TextLength;
            form.richTextBox1.ScrollToCaret();

            form.richTextBox1.SelectAll();
            form.richTextBox1.SelectionAlignment = HorizontalAlignment.Center;
            form.richTextBox1.DeselectAll();

            form.StartPosition = FormStartPosition.CenterScreen;

            if (icon != MessageBoxIcon.Error && icon != MessageBoxIcon.Warning)
                form.checkBox1.Hide();

            return form.ShowDialog();
        }

        public static DialogResult Show(string text)
        {
            return Show(text, null, MessageBoxIcon.None);
        }

        public static DialogResult Show(string text, string title)
        {
            return Show(text, title, MessageBoxIcon.None);
        }

        public static DialogResult Show(string text, MessageBoxIcon icon)
        {
            return Show(text, null, icon);
        }

        public static DialogResult ShowError(string text)
        {
            return Show(text, null, MessageBoxIcon.Error);
        }

        public static DialogResult ShowError(string text, string title)
        {
            return Show(text, title, MessageBoxIcon.Error);
        }

        public static DialogResult ShowWarning(string text)
        {
            return Show(text, null, MessageBoxIcon.Warning);
        }

        public static DialogResult ShowWarning(string text, string title)
        {
            return Show(text, title, MessageBoxIcon.Warning);
        }

        public static DialogResult ShowInfo(string text)
        {
            return Show(text, null, MessageBoxIcon.Information);
        }

        public static DialogResult ShowInfo(string text, string title)
        {
            return Show(text, title, MessageBoxIcon.Information);
        }
    }
}
