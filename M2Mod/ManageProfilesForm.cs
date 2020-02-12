using M2Mod.Config;
using System;
using System.Data;
using System.Linq;
using System.Windows.Forms;

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
            var selectedId = SelectedProfile?.Id;

            profilesListBox.Items.Clear();
            profilesListBox.Items.AddRange(ProfileManager.GetProfiles().Cast<object>().ToArray());

            profilesListBox.SelectedIndex = ProfileManager.GetProfiles().FindIndex(_ => _.Id == selectedId);
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

                ProfileManager.AddProfile(new SettingsProfile(form.EnteredName.Trim(), Defaults.Settings, new Configuration()));
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

            EditProfile(SelectedProfile);
        }

        private void EditProfile(SettingsProfile profile)
        {
            using (var form = new EnterNameForm())
            {
                form.EnteredName = profile.Name;
                if (form.ShowDialog() != DialogResult.OK)
                    return;

                var name = form.EnteredName.Trim();
                if (ProfileManager.GetProfiles().Any(_ => _.Name == name && _.Id != profile.Id))
                {
                    MessageBox.Show("Profile with this name already exists", "Error", MessageBoxButtons.OK);
                    return;
                }

                profile.Name = form.EnteredName.Trim();
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

                ProfileManager.AddProfile(new SettingsProfile(name, SelectedProfile.Settings, SelectedProfile.Configuration));
            }

            SetupProfiles();
        }

        private void ProfilesListBox_DoubleClick(object sender, EventArgs e)
        {
            if (SelectedProfile == null)
                return;

            EditProfile(SelectedProfile);
        }

        private void MoveUpButton_Click(object sender, EventArgs e)
        {
            if (SelectedProfile == null)
                return;

            ProfileManager.MoveProfile(SelectedProfile.Id, false);
            SetupProfiles();
        }

        private void MoveDownButton_Click(object sender, EventArgs e)
        {
            if (SelectedProfile == null)
                return;

            ProfileManager.MoveProfile(SelectedProfile.Id, true);
            SetupProfiles();
        }
    }
}
