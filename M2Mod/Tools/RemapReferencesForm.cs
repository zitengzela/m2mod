using System;
using System.IO;
using System.Windows.Forms;
using M2Mod.Config;
using M2Mod.Dialogs;
using M2Mod.Interop;
using M2Mod.Interop.Structures;

namespace M2Mod.Tools
{
    public partial class RemapReferencesForm : Form
    {
        private Imports.LoggerDelegate logDelegate;

        public RemapReferencesForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            logDelegate = Log;
            Imports.AttachLoggerCallback(LogLevel.Custom, logDelegate);
        }

        private void Log(LogLevel logLevel, string message)
        {
            logTextBox.AppendLine(logLevel, message);
        }

        private void closeButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private static bool FilePathHasInvalidChars(string path)
        {
            return (!string.IsNullOrEmpty(path) && path.IndexOfAny(System.IO.Path.GetInvalidPathChars()) >= 0);
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

            var remapPath = remapRelativePathTextBox.Text;
            if (FilePathHasInvalidChars(remapPath))
            {
                MessageBox.Show("Remap path has invalid characters", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Imports.M2_Free(m2);
                return;
            }

            remapPath = remapPath.Trim('\\', '/', ' ', '\t');

            errorStatus = Imports.M2_SetNeedRemapReferences(m2, remapPath);
            if (errorStatus != M2LibError.OK)
            {
                MessageBox.Show(string.Format("Failed to prepare for chunk removal: {0}", Imports.GetErrorText(errorStatus)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Imports.M2_Free(m2);
                return;
            }

            string ExportFileName;
            using (var dialog = new SaveFileDialog())
            {
                dialog.Filter = Filters.M2;
                dialog.FileName = Path.GetFileName(m2PathTextBox.Text);
                dialog.InitialDirectory = settings.OutputDirectory;

                if (dialog.ShowDialog() != DialogResult.OK)
                {
                    Imports.M2_Free(m2);
                    return;
                }

                ExportFileName = dialog.FileName;
            }

            // export M2
            var error = Imports.M2_Save(m2, ExportFileName, SaveMask.All);
            if (error != M2LibError.OK)
            {
                Imports.M2_Free(m2);
                return;
            }

            Imports.M2_Free(m2);
        }

        private void m2BrowseButton_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = Filters.M2;
                dialog.InitialDirectory = m2PathTextBox.Text.Length > 0 ? Path.GetDirectoryName(m2PathTextBox.Text) : ProfileManager.CurrentProfile.Settings.WorkingDirectory;

                var result = dialog.ShowDialog();
                if (result != DialogResult.OK)
                    return;

                m2PathTextBox.Text = dialog.FileName;
            }
        }

        private void RemapReferencesForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Imports.DetachLoggerCallback(LogLevel.Custom, logDelegate);
        }
    }
}
