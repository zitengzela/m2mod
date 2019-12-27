using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using M2Mod.Interop.Structures;

namespace M2Mod
{
    public partial class LogTextBox : RichTextBox
    {
        private const string Category = "Log Formatting";

        public LogTextBox()
        {
            InitializeComponent();
        }

        [Category(Category)]
        public bool InlcudeTimestamp { get; set; } = true;

        [Category(Category)]
        public bool UseColors { get; set; } = true;

        public void AppendLine(LogLevel logLevel, string message)
        {
            Color textColor, backColor;

            switch (logLevel)
            {
                case LogLevel.Error:
                    textColor = Color.Black;
                    backColor = Color.Red;
                    break;
                case LogLevel.Warning:
                    textColor = Color.Black;
                    backColor = Color.Yellow;
                    break;
                default:
                    textColor = backColor = Color.Empty;
                    break;
            }

            AppendLine(message, textColor, backColor);
        }

        public void AppendLine(string text, Color textColor, Color backColor)
        {
            if (!UseColors)
                textColor = backColor = Color.Empty;

            if (InlcudeTimestamp)
                text = $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}]: {text}";

            if (TextLength > 0)
                text = "\r\n" + text;

            if (textColor == Color.Empty)
            {
                AppendText(text);
                return;
            }

            SelectionStart = TextLength;
            SelectionLength = 0;

            SelectionColor = textColor;
            SelectionBackColor = backColor;
            AppendText(text);
            SelectionColor = ForeColor;
            SelectionBackColor = BackColor;

            SelectionStart = TextLength;
            ScrollToCaret();
        }
    }
}
