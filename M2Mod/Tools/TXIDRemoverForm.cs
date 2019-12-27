using M2Mod.Config;
using M2Mod.Interop;
using M2Mod.Interop.Structures;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace M2Mod.Tools
{
    public partial class TXIDRemoverForm : Form
    {
        private const string _m2Filter = "M2 Files|*.m2|All Files|*.*";

        private Imports.LoggerDelegate logDelegate;

        public TXIDRemoverForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            logDelegate = Log;
            Imports.AttachLoggerCallback(LogLevel.Custom, logDelegate);
        }

        private void m2BrowseButton_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = _m2Filter;
                try
                {
                    dialog.InitialDirectory = ProfileManager.CurrentProfile.Settings.WorkingDirectory;
                }
                catch
                {
                    // ignored
                }

                var result = dialog.ShowDialog();
                if (result != DialogResult.OK)
                    return;

                m2PathTextBox.Text = dialog.FileName;
            }
        }

        private void Log(LogLevel logLevel, string message)
        {
            logTextBox.AppendLine(logLevel, message);
        }

        private void convertButton_Click(object sender, EventArgs e)
        {
            if (!File.Exists(m2PathTextBox.Text))
            {
                MessageBox.Show($"No such file: '{m2PathTextBox.Text}'", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            /*var oldSettings = new Settings()
            {
                WorkingDirectory = ProfileManager.CurrentProfile.Settings.WorkingDirectory,
                MappingsDirectory = ProfileManager.CurrentProfile.Settings.MappingsDirectory,
            };*/

            var settings = ProfileManager.CurrentProfile.Settings;

            var m2 = Imports.M2_Create(ref settings);
            var errorStatus = Imports.M2_Load(m2, m2PathTextBox.Text);
            if (errorStatus != M2LibError.OK)
            {
                MessageBox.Show(string.Format("Failed load m2: {0}", Imports.GetErrorText(errorStatus)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Imports.M2_Free(m2);
                return;
            }

            logTextBox.Clear();

            errorStatus = Imports.M2_SetNeedRemoveTXIDChunk(m2);
            if (errorStatus != M2LibError.OK)
            {
                MessageBox.Show(string.Format("Failed to prepare for chunk removal: {0}", Imports.GetErrorText(errorStatus)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Imports.M2_Free(m2);
                return;
            }

            string ExportFileName;
            using (var dialog = new SaveFileDialog())
            {
                dialog.Filter = _m2Filter;
                try
                {
                    dialog.InitialDirectory = settings.OutputDirectory;
                    dialog.FileName = Path.GetFileName(m2PathTextBox.Text);
                }
                catch
                {
                    // ignored
                }

                if (dialog.ShowDialog() != DialogResult.OK)
                {
                    Imports.M2_Free(m2);
                    return;
                }

                ExportFileName = dialog.FileName;
            }

            // export M2
            var error = Imports.M2_Save(m2, ExportFileName, SaveMask.M2);
            if (error != M2LibError.OK)
            {
                Imports.M2_Free(m2);
                return;
            }

            Imports.M2_Free(m2);
        }

        private void closeButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void TXIDRemover_FormClosing(object sender, FormClosingEventArgs e)
        {
            Imports.DetachLoggerCallback(LogLevel.Custom, logDelegate);
        }
    }
}
