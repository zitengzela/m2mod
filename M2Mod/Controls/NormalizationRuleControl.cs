using System;
using System.ComponentModel;
using System.Linq;
using System.Windows.Forms;

namespace M2Mod.Controls
{
    public partial class NormalizationRuleControl : UserControl
    {
        private const string RuleTooltip = "Must contain space-separated entries, each is 4-digit mesh id, example: 1234 or 12x4 where 'x' matches any number";

        public NormalizationRuleControl()
        {
            InitializeComponent();

            var items = Enum.GetValues(typeof(NormalizeRuleType)).Cast<object>().ToArray();

            sourceTypeComboBox.Items.Clear();
            sourceTypeComboBox.Items.AddRange(items);
            sourceTypeComboBox.SelectedIndex = 1;

            targetTypeComboBox.Items.Clear();
            targetTypeComboBox.Items.AddRange(items);
            targetTypeComboBox.SelectedIndex = 1;

            toolTip1.SetToolTip(sourceTextBox, RuleTooltip);
            toolTip1.SetToolTip(targetTextBox, RuleTooltip);
        }

        [DefaultValue(NormalizeRuleType.Body), Browsable(true), DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), Category("Appearance")]
        public NormalizeRuleType SourceType
        {
            get => sourceTypeComboBox.SelectedItem as NormalizeRuleType? ?? NormalizeRuleType.Body;
            set
            {
                sourceTypeComboBox.SelectedItem =
                    sourceTypeComboBox.Items.Cast<NormalizeRuleType>().FirstOrDefault(_ => _ == value);
            }
        }

        [DefaultValue(""), Browsable(true), DesignerSerializationVisibility(DesignerSerializationVisibility.Visible),
         Category("Appearance")]
        public string SourceRuleText
        {
            get => sourceTextBox.Text;
            set => sourceTextBox.Text = value;
        }

        [DefaultValue(NormalizeRuleType.Body), Browsable(true), DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), Category("Appearance")]
        public NormalizeRuleType TargetType
        {
            get => targetTypeComboBox.SelectedItem as NormalizeRuleType? ?? NormalizeRuleType.Body;
            set
            {
                targetTypeComboBox.SelectedItem =
                    sourceTypeComboBox.Items.Cast<NormalizeRuleType>().FirstOrDefault(_ => _ == value);
            }
        }

        [DefaultValue(""), Browsable(true), DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), Category("Appearance")]
        public string TargetRuleText
        {
            get => targetTextBox.Text;
            set => targetTextBox.Text = value;
        }

        [DefaultValue(false), Browsable(true), DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), Category("Appearance")]
        public bool PreferSource
        {
            get => preferSourceCheckBox.Checked;
            set => preferSourceCheckBox.Checked = value;
        }

        private void sourceTypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (sourceTextBox != null)
            {
                sourceTextBox.Visible = sourceTypeComboBox.SelectedItem != null &&
                                        (sourceTypeComboBox.SelectedItem as NormalizeRuleType?) == NormalizeRuleType.Custom;
            }
        }

        private void targetTypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (targetTextBox == null)
                return;

            targetTextBox.Visible = targetTypeComboBox.SelectedItem != null &&
                                    (targetTypeComboBox.SelectedItem as NormalizeRuleType?) == NormalizeRuleType.Custom;
        }

        public void InitializeFromRuleSet(NormalizationRule rule)
        {
            SourceType = rule.SourceType;
            TargetType = rule.TargetType;
            SourceRuleText = string.Join(",", rule.SourceRules.Rules);
            TargetRuleText = string.Join(",", rule.TargetRules.Rules);
            preferSourceCheckBox.Checked = rule.PreferSourceDirection;
            enabledCheckBox.Checked = rule.Enabled;
        }

        public NormalizationRule ProduceRule()
        {
            if (SourceType == NormalizeRuleType.Custom && string.IsNullOrWhiteSpace(SourceRuleText))
                throw new Exception("Custom source rule with empty rule data");
            if (TargetType == NormalizeRuleType.Custom && string.IsNullOrWhiteSpace(TargetRuleText))
                throw new Exception("Custom target rule with empty rule data");

            return new NormalizationRule()
            {
                SourceType = this.SourceType,
                SourceRules = new RuleList(SourceRuleText),
                TargetType = this.TargetType,
                TargetRules = new RuleList(TargetRuleText),
                PreferSourceDirection = preferSourceCheckBox.Checked,
                Enabled = enabledCheckBox.Checked,
            };
        }

        private void enabledCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            sourceTypeComboBox.Enabled = enabledCheckBox.Checked;
            targetTypeComboBox.Enabled = enabledCheckBox.Checked;
            sourceTextBox.Enabled = enabledCheckBox.Checked;
            targetTextBox.Enabled = enabledCheckBox.Checked;
            preferSourceCheckBox.Enabled = enabledCheckBox.Checked;
        }
    }
}
