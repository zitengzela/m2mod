using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using M2Mod.Config;

namespace M2Mod
{
    public partial class ManageProfilesForm : Form
    {
        private SettingsProfile SelectedProfile => profilesListBox.SelectedItem as SettingsProfile;

        public ManageProfilesForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            SetupProfiles();
        }

        private void SetupProfiles()
        {
            profilesListBox.Items.Clear();
            profilesListBox.Items.AddRange(ProfileManager.GetProfiles().Cast<object>().ToArray());
        }

        private void CloseButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void AddButton_Click(object sender, EventArgs e)
        {
            using (var form = new EnterNameForm())
            {
                if (form.ShowDialog() != DialogResult.OK)
                    return;

                ProfileManager.AddProfile(new SettingsProfile(form.EnteredName.Trim(), Defaults.Settings, new FormData()));
            }

            SetupProfiles();
        }

        private void RemoveButton_Click(object sender, EventArgs e)
        {
            if (SelectedProfile == null)
                return;

            if (profilesListBox.Items.Count <= 1)
            {
                MessageBox.Show("You must have at least one profile remaining", "", MessageBoxButtons.OK,
                    MessageBoxIcon.Warning);
                return;
            }

            if (MessageBox.Show($"Are you sure you want to delete profile '{SelectedProfile.Name}'?", "",
                    MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
            {
                return;
            }

            ProfileManager.RemoveProfile(SelectedProfile.Id);
            SetupProfiles();
        }

        private void EditButton_Click(object sender, EventArgs e)
        {
            if (SelectedProfile == null)
                return;

            using (var form = new EnterNameForm())
            {
                form.EnteredName = SelectedProfile.Name;
                if (form.ShowDialog() != DialogResult.OK)
                    return;

                var name = form.EnteredName.Trim();
                if (ProfileManager.GetProfiles().Any(_ => _.Name == name))
                {
                    MessageBox.Show("Profile with this name already exists", "Error", MessageBoxButtons.OK);
                    return;
                }

                SelectedProfile.Name = form.EnteredName.Trim();
            }

            SetupProfiles();
        }

        private void DuplicateButton_Click(object sender, EventArgs e)
        {
            if (SelectedProfile == null)
                return;

            using (var form = new EnterNameForm())
            {
                form.EnteredName = SelectedProfile.Name;
                if (form.ShowDialog() != DialogResult.OK)
                    return;

                var name = form.EnteredName.Trim();
                if (ProfileManager.GetProfiles().Any(_ => _.Name == name))
                {
                    MessageBox.Show("Profile with this name already exists", "Error", MessageBoxButtons.OK);
                    return;
                }

                ProfileManager.AddProfile(new SettingsProfile(name, SelectedProfile.Settings, SelectedProfile.FormData));
            }

            SetupProfiles();
        }
    }
}
