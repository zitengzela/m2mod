using System;
using System.IO;
using System.Windows.Forms;
using M2Mod.Interop;
using M2Mod.Interop.Structures;
using M2Mod.Registry;

namespace M2Mod
{
    public partial class CompareBonesForm : Form
    {
        private const string _m2Filter = "M2 Files|*.m2|All Files|*.*";
        private const string _txtFilter = "Txt Files|*.txt|All Files|*.*";

        public Settings Settings = Defaults.Settings;

        public CompareBonesForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            oldM2TextBox.Text = (RegistryStore.GetValue(RegistryValue.OldCompareM2) ?? "").ToString();
            newM2TextBox.Text = (RegistryStore.GetValue(RegistryValue.NewCompareM2) ?? "").ToString();
            weightThresholdTextBox.Text = (RegistryStore.GetValue(RegistryValue.CompareWeightThreshold) ?? "").ToString();
        }

        private void OldM2BrowseButton_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = _m2Filter;
                try
                {
                    if (oldM2TextBox.Text.Length > 0)
                        dialog.InitialDirectory = Path.GetDirectoryName(oldM2TextBox.Text);
                }
                catch
                {
                    // ignored
                }

                var result = dialog.ShowDialog();
                if (result != DialogResult.OK)
                    return;

                oldM2TextBox.Text = dialog.FileName;
            }
        }

        private void NewM2BrowseButton_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = _m2Filter;
                try
                {
                    if (newM2TextBox.Text.Length > 0)
                        dialog.InitialDirectory = Path.GetDirectoryName(newM2TextBox.Text);
                }
                catch
                {
                    // ignored
                }

                var result = dialog.ShowDialog();
                if (result != DialogResult.OK)
                    return;

                newM2TextBox.Text = dialog.FileName;
            }
        }

        private void CompareButton_Click(object sender, EventArgs e)
        {
            Compare();
        }

        private void Compare()
        {
            if (!File.Exists(oldM2TextBox.Text))
            {
                MessageBox.Show($"No such file: '{oldM2TextBox.Text}'", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            if (!File.Exists(newM2TextBox.Text))
            {
                MessageBox.Show($"No such file: '{newM2TextBox.Text}'", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            RegistryStore.SetValue(RegistryValue.OldCompareM2, oldM2TextBox.Text);
            RegistryStore.SetValue(RegistryValue.NewCompareM2, newM2TextBox.Text);
            RegistryStore.SetValue(RegistryValue.CompareWeightThreshold, weightThresholdTextBox.Text);

            if (!float.TryParse(weightThresholdTextBox.Text, out var weightThreshold))
            {
                weightThreshold = 0.0f;
                weightThresholdTextBox.Text = "0";
            }

            IntPtr wrapper = Imports.Wrapper_Create(oldM2TextBox.Text, newM2TextBox.Text, weightThreshold, true, ref this.Settings);

            var errorStatus = Imports.Wrapper_GetErrorStatus(wrapper);
            if (errorStatus != M2LibError.OK)
            {
                MessageBox.Show(String.Format("Failed to compare: {0}", Imports.GetErrorText(errorStatus)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Imports.Wrapper_Free(wrapper);
                return;
            }

            resultsTextBox.Text = "";
            if (Imports.Wrapper_DiffSize(wrapper) == 0)
            {
                MessageBox.Show("Empty result from bone comparator");
                Imports.Wrapper_Free(wrapper);
                return;
            }

            resultsTextBox.Text = Imports.Wrapper_GetStringResult(wrapper);
            Imports.Wrapper_Free(wrapper);
        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            using (var dialog = new SaveFileDialog())
            {
                dialog.CheckPathExists = true;
                dialog.Filter = _txtFilter;

                if (oldM2TextBox.Text.Length > 0)
                {
                    var name = Path.GetFileNameWithoutExtension(oldM2TextBox.Text);
                    if (name.Length > 0)
                        dialog.FileName = $"bone_migration_{name}.txt";
                }

                var result = dialog.ShowDialog();
                if (result != DialogResult.OK)
                    return;

                File.WriteAllText(dialog.FileName, resultsTextBox.Text);
            }
        }

        private void ResultsTextBox_TextChanged(object sender, EventArgs e)
        {
            saveButton.Enabled = resultsTextBox.Text.Length > 0;
        }
    }
}
