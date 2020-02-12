using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using M2Mod.Config;
using M2Mod.Config.Normalization;
using M2Mod.Controls;

namespace M2Mod
{
    public partial class FixNormalsSettingsForm : Form
    {
        private static readonly string[] DefaultDataContent = new string[] {
            "#lines starting with # are ignored",
            "#each line is separate entry containing 4-digit mesh id",
            "#example: 1234 or 12x4 where 'x' matches any number"
        };

        public FixNormalsSettingsForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;
            toolTip1.SetToolTip(this.linkLabel1, string.Join("\r\n", DefaultDataContent));
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            try
            {
                var config = ProduceConfig();
                var rules = SimpleConfig.ParseData(config.Simple.Data);
                foreach (var rule in rules)
                {
                    rule.SourceRules.Serialize();
                    rule.TargetRules.Serialize();
                }

                foreach (var rule in config.Advanced.Rules)
                {
                    rule.SourceRules.Serialize();
                    rule.TargetRules.Serialize();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            DialogResult = DialogResult.OK;
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        private void addRuleButton_Click(object sender, EventArgs e)
        {
            AddControlRule(new NormalizationRule());
        }

        private void AddControlRule(NormalizationRule rule)
        {
            var lastControl = tabPage2.Controls.Cast<Control>().Where(_ => _ is NormalizeRuleControlContainer)
                .OrderByDescending(_ => _.Location.Y).FirstOrDefault();

            int y, x;
            if (lastControl == null)
            {
                x = addRuleButton.Location.X;
                y = addRuleButton.Location.Y + addRuleButton.Height + 2;
            }
            else
            {
                x = lastControl.Location.X;
                y = lastControl.Location.Y + lastControl.Height + 2;
            }

            var control = new NormalizeRuleControlContainer
            {
                Width = lastControl?.Width ?? tabPage2.Width - 15,
                Location = new Point(x, y)
            };
            control.Anchor |= AnchorStyles.Right;

            control.Rules.InitializeFromRuleSet(rule);

            control.OnCrossClicked += (o, args) =>
            {
                tabPage2.Controls.Remove(o as Control);
                ReorderControls();
            };

            tabPage2.Controls.Add(control);
        }

        private void ReorderControls()
        {
            var controls = tabPage2.Controls.Cast<Control>().Where(_ => _ is NormalizeRuleControlContainer)
                .OrderBy(_ => _.Location.Y);

            var y = addRuleButton.Location.Y + addRuleButton.Height + 2;

            foreach (var control in controls)
            {
                control.Location = new Point(control.Location.X, y);

                y += control.Height + 2;
            }
        }

        public NormalizationConfig ProduceConfig()
        {
            return new NormalizationConfig()
            {
                UseAdvanced = tabControl1.SelectedIndex == 1,
                Simple = new SimpleConfig()
                {
                    AlignFacials = facialsCheckBox.Checked,
                    AlignHair = hairCheckBox.Checked,
                    Data =  settingsTextBox.Text,
                },
                Advanced = new AdvancedConfig()
                {
                    Rules = tabPage2.Controls.Cast<Control>()
                        .Where(_ => _ is NormalizeRuleControlContainer)
                        .OrderBy(_ => _.Location.Y)
                        .Select(_ => (_ as NormalizeRuleControlContainer).Rules.ProduceRule()).ToList()
                }
            };
        }

        public void InitializeFromConfig(NormalizationConfig normalizationConfig)
        {
            tabControl1.SelectedIndex = normalizationConfig.UseAdvanced ? 1 : 0;

            facialsCheckBox.Checked = normalizationConfig.Simple.AlignFacials;
            hairCheckBox.Checked = normalizationConfig.Simple.AlignHair;
            settingsTextBox.Text = !string.IsNullOrWhiteSpace(normalizationConfig.Simple.Data)
                ? normalizationConfig.Simple.Data
                : string.Join("\r\n", DefaultDataContent);

            foreach (var rule in normalizationConfig.Advanced.Rules)
                AddControlRule(rule);
        }
    }
}
