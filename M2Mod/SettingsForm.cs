using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows.Forms;
using M2Mod.Config;
using M2Mod.Interop.Structures;

namespace M2Mod
{
    public partial class SettingsForm : Form
    {
        private SettingsProfile _lastProfile;

        private SettingsProfile SelectedProfile => profilesComboBox.SelectedItem as SettingsProfile;

        private string FixNormalSettings;
        private bool IncludeFacials;
        private bool IncludeHair;

        public SettingsForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            forceExpansionComboBox.Items.AddRange(Enum.GetValues(typeof(Expansion)).Cast<object>().ToArray());
            forceExpansionComboBox.SelectedItem = forceExpansionComboBox.Items.Cast<Expansion>()
                .FirstOrDefault(_ => _ == Expansion.None);

            _lastProfile = ProfileManager.CurrentProfile;
            SetupProfiles();
            SetupControls(_lastProfile);
        }

        private void SetupProfiles()
        {
            profilesComboBox.Items.Clear();
            profilesComboBox.Items.AddRange(ProfileManager.GetProfiles().Cast<object>().ToArray());
            profilesComboBox.SelectedItem = _lastProfile != null
                ? profilesComboBox.Items.Cast<SettingsProfile>()
                      .FirstOrDefault(_ => _.Id == _lastProfile.Id) ?? profilesComboBox.Items[0]
                : profilesComboBox.Items[0];
        }

        private void SetupControls(SettingsProfile profile)
        {
            var settings = profile.Settings;

            forceExpansionComboBox.SelectedItem = forceExpansionComboBox.Items.Cast<Expansion>()
                .FirstOrDefault(_ => _ == settings.ForceLoadExpansion);

            workingDirectoryTextBox.Text = settings.WorkingDirectory;
            outputDirectoryTextBox.Text = settings.OutputDirectory;
            mappingsDirectoryTextBox.Text = settings.MappingsDirectory;

            checkBoxMergeBones.Checked = settings.MergeBones;
            checkBoxMergeAttachments.Checked = settings.MergeAttachments;
            checkBoxMergeCameras.Checked = settings.MergeCameras;
            // = settings.FixSeams;
            checkBoxFixEdgeNormals.Checked = settings.FixEdgeNormals;
            checkBoxIgnoreOriginalMeshIndexes.Checked = settings.IgnoreOriginalMeshIndexes;
            testFixAnimationsCheckBox.Checked = settings.FixAnimationsTest;
            customFilesStartIndexTextBox.Text = settings.CustomFilesStartIndex > 0 ? settings.CustomFilesStartIndex.ToString() : "";

            FixNormalSettings = profile.FormData.FixNormalSettings;
            IncludeFacials = profile.FormData.IncludeFacials;
            IncludeHair = profile.FormData.IncludeHair;
        }

        private Settings ProduceSettings()
        {
            if (!uint.TryParse(customFilesStartIndexTextBox.Text, out var customFilesStartIndex))
                customFilesStartIndex = 0;

            return new Settings()
            {
                WorkingDirectory = workingDirectoryTextBox.Text.Trim(),
                OutputDirectory = outputDirectoryTextBox.Text.Trim(),
                MappingsDirectory = mappingsDirectoryTextBox.Text.Trim(),

                MergeBones = checkBoxMergeBones.Checked,
                MergeAttachments = checkBoxMergeAttachments.Checked,
                MergeCameras = checkBoxMergeCameras.Checked,
                FixSeams = false,
                FixEdgeNormals = checkBoxFixEdgeNormals.Checked,
                IgnoreOriginalMeshIndexes = checkBoxIgnoreOriginalMeshIndexes.Checked,
                FixAnimationsTest = testFixAnimationsCheckBox.Checked,
                ForceLoadExpansion = forceExpansionComboBox.SelectedItem as Expansion? ?? Expansion.None,
                CustomFilesStartIndex = customFilesStartIndex
            };
        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            if (workingDirectoryTextBox.Text.Length > 0)
            {
                if (!Directory.Exists(workingDirectoryTextBox.Text))
                {
                    MessageBox.Show("Working directory does not exist", "Error", MessageBoxButtons.OK,
                        MessageBoxIcon.Error);
                    return;
                }
            }

            if (outputDirectoryTextBox.Text.Length > 0)
            {
                if (!Directory.Exists(outputDirectoryTextBox.Text))
                {
                    MessageBox.Show("Output directory does not exist", "Error", MessageBoxButtons.OK,
                        MessageBoxIcon.Error);
                    return;
                }
            }

            if (mappingsDirectoryTextBox.Text.Length > 0)
            {
                if (!Directory.Exists(mappingsDirectoryTextBox.Text))
                {
                    MessageBox.Show("Mappings directory does not exist", "Error", MessageBoxButtons.OK,
                        MessageBoxIcon.Error);
                    return;
                }
            }

            if (!uint.TryParse(customFilesStartIndexTextBox.Text, out var customFilesStartIndex))
            {
                MessageBox.Show("Custom file index is not a number", "Error", MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }

            SelectedProfile.Settings = ProduceSettings();
            SelectedProfile.FormData.FixNormalSettings = FixNormalSettings;
            SelectedProfile.FormData.IncludeFacials = IncludeFacials;
            SelectedProfile.FormData.IncludeHair = IncludeHair;
        }

        private void WorkingDirectoryBrowseButton_Click(object sender, EventArgs e)
        {
            using (var dialog = new FolderBrowserDialog())
            {
                var path = workingDirectoryTextBox.Text;
                if (path.Length > 0 && Directory.Exists(path))
                    dialog.SelectedPath = path;
                else
                    dialog.SelectedPath = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

                dialog.Description = "Select working directory:";

                if (dialog.ShowDialog() == DialogResult.OK)
                    workingDirectoryTextBox.Text = dialog.SelectedPath;
            }
        }

        private void OutputDirectoryBrowseButton_Click(object sender, EventArgs e)
        {
            using (var dialog = new FolderBrowserDialog())
            {
                var path = outputDirectoryTextBox.Text;
                if (path.Length > 0 && Directory.Exists(path))
                    dialog.SelectedPath = path;
                else
                    dialog.SelectedPath = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

                dialog.Description = "Select output directory:";

                if (dialog.ShowDialog() == DialogResult.OK)
                    outputDirectoryTextBox.Text = dialog.SelectedPath;
            }
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            if (SettingsChanged(SelectedProfile))
            {
                if (DialogResult.OK != MessageBox.Show("Your unsaved changes will be lost. Are you sure?", "Warning",
                        MessageBoxButtons.OKCancel, MessageBoxIcon.Warning))
                    return;
            }

            DialogResult = DialogResult.Cancel;
        }

        private void ProfilesComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (_lastProfile == profilesComboBox.SelectedItem)
                return;

            if (_lastProfile != null && SettingsChanged(_lastProfile))
            {
                var res = MessageBox.Show("Your unsaved changes will be lost. Are you sure?", "Warning",
                    MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);
                if (res == DialogResult.Cancel)
                {
                    profilesComboBox.SelectedItem = _lastProfile;
                    return;
                }
            }

            this.SetupControls(profilesComboBox.SelectedItem as SettingsProfile);

            this._lastProfile = SelectedProfile;
        }

        private bool SettingsChanged(SettingsProfile old)
        {
            var New = ProduceSettings();

            return
                old.Settings.WorkingDirectory != New.WorkingDirectory ||
                old.Settings.OutputDirectory != New.OutputDirectory ||
                old.Settings.MappingsDirectory != New.MappingsDirectory ||
                old.Settings.MergeBones != New.MergeBones ||
                old.Settings.MergeAttachments != New.MergeAttachments ||
                old.Settings.MergeCameras != New.MergeCameras ||
                old.Settings.FixSeams != New.FixSeams ||
                old.Settings.FixEdgeNormals != New.FixEdgeNormals ||
                old.Settings.IgnoreOriginalMeshIndexes != New.IgnoreOriginalMeshIndexes ||
                old.Settings.FixAnimationsTest != New.FixAnimationsTest ||
                old.Settings.ForceLoadExpansion != New.ForceLoadExpansion ||
                old.Settings.CustomFilesStartIndex != New.CustomFilesStartIndex ||
                old.FormData.IncludeFacials != IncludeFacials ||
                old.FormData.IncludeHair != IncludeHair ||
                old.FormData.FixNormalSettings != FixNormalSettings;
        }

        private void EditProfilesButton_Click(object sender, EventArgs e)
        {
            using (var form = new ManageProfilesForm())
                form.ShowDialog();

            SetupProfiles();
        }

        private void MappingsDirectoryButton_Click(object sender, EventArgs e)
        {
            using (var dialog = new FolderBrowserDialog())
            {
                var path = mappingsDirectoryTextBox.Text;
                if (path.Length > 0 && Directory.Exists(path))
                    dialog.SelectedPath = path;
                else
                    dialog.SelectedPath = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

                dialog.Description = "Select mappings directory:";

                if (dialog.ShowDialog() == DialogResult.OK)
                    mappingsDirectoryTextBox.Text = dialog.SelectedPath;
            }
        }

        private void checkBoxFixEdgeNormals_CheckedChanged(object sender, EventArgs e)
        {
            buttonEdgeConfigure.Enabled = checkBoxFixEdgeNormals.Checked;
        }

        private void buttonEdgeConfigure_Click(object sender, EventArgs e)
        {
            using (var form = new FixNormalsSettingsForm())
            {
                form.Data = FixNormalSettings;
                form.IncludeFacials = IncludeFacials;
                form.IncludeHair= IncludeHair;

                var result = form.ShowDialog();
                if (result == DialogResult.OK)
                {
                    FixNormalSettings = form.Data;
                    IncludeFacials = form.IncludeFacials;
                    IncludeHair = form.IncludeHair;
                }
            }
        }
    }
}
