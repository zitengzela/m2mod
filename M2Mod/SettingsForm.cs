using System;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using M2Mod.Interop.Structures;

namespace M2Mod
{
    public partial class SettingsForm : Form
    {
        public SettingsForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            forceExpansionComboBox.Items.AddRange(Enum.GetValues(typeof(Expansion)).Cast<object>().ToArray());
            forceExpansionComboBox.SelectedItem = forceExpansionComboBox.Items.Cast<Expansion>()
                .FirstOrDefault(_ => _ == Expansion.None);
        }

        public void Setup(Settings settings)
        {
            forceExpansionComboBox.SelectedItem = forceExpansionComboBox.Items.Cast<Expansion>()
                .FirstOrDefault(_ => _ == settings.ForceLoadExpansion);

            workingDirectoryTextBox.Text = settings.WorkingDirectory;
            outputDirectoryTextBox.Text = settings.OutputDirectory;

            checkBoxMergeBones.Checked = settings.MergeBones;
            checkBoxMergeAttachments.Checked = settings.MergeAttachments;
            checkBoxMergeCameras.Checked = settings.MergeCameras;
            checkBoxFixSeams.Checked = settings.FixSeams;
            checkBoxFixEdgeNormals.Checked = settings.FixEdgeNormals;
            checkBoxIgnoreOriginalMeshIndexes.Checked = settings.IgnoreOriginalMeshIndexes;
            testFixAnimationsCheckBox.Checked = settings.FixAnimationsTest;
        }

        public Settings ProduceSettings()
        {
            return new Settings()
            {
                WorkingDirectory = workingDirectoryTextBox.Text.Trim(),
                OutputDirectory = outputDirectoryTextBox.Text.Trim(),

                MergeBones = checkBoxMergeBones.Checked,
                MergeAttachments = checkBoxMergeAttachments.Checked,
                MergeCameras = checkBoxMergeCameras.Checked,
                FixSeams = checkBoxFixSeams.Checked,
                FixEdgeNormals = checkBoxFixEdgeNormals.Checked,
                IgnoreOriginalMeshIndexes = checkBoxIgnoreOriginalMeshIndexes.Checked,
                FixAnimationsTest = testFixAnimationsCheckBox.Checked,
                ForceLoadExpansion = forceExpansionComboBox.SelectedItem as Expansion? ?? Expansion.None
            };
        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            if (workingDirectoryTextBox.Text.Length > 0)
            {
                if (!Directory.Exists(workingDirectoryTextBox.Text))
                {
                    MessageBox.Show("Working directory does not exist", "M2LibError", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            if (outputDirectoryTextBox.Text.Length > 0)
            {
                if (!Directory.Exists(outputDirectoryTextBox.Text))
                {
                    MessageBox.Show("Output directory does not exist", "M2LibError", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            DialogResult = DialogResult.OK;
        }

        private void WorkingDirectoryBrowseButton_Click(object sender, EventArgs e)
        {
            var path = workingDirectoryTextBox.Text;
            if (path.Length > 0 && Directory.Exists(path))
                directoryBrowserDialog.SelectedPath = path;

            directoryBrowserDialog.Description = "Select working directory:";

            if (directoryBrowserDialog.ShowDialog() == DialogResult.OK)
                workingDirectoryTextBox.Text = directoryBrowserDialog.SelectedPath;
        }

        private void OutputDirectoryBrowseButton_Click(object sender, EventArgs e)
        {
            var path = outputDirectoryTextBox.Text;
            if (path.Length > 0 && Directory.Exists(path))
                directoryBrowserDialog.SelectedPath = path;

            directoryBrowserDialog.Description = "Select output directory:";

            if (directoryBrowserDialog.ShowDialog() == DialogResult.OK)
                outputDirectoryTextBox.Text = directoryBrowserDialog.SelectedPath;
        }

        private void CancelButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }
    }
}
