using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace M2Mod
{
    public partial class FixNormalsSettingsForm : Form
    {
        private static readonly string[] DefaultContent =  new string[] {
            "#lines starting with # are ignored",
            "#each line is separate entry containing 4-digit mesh id",
            "#example: 1234 or 12x4 where 'x' matches any number"
        };

        public string Data
        {
            get => settingsTextBox.Text;
            set => settingsTextBox.Text = string.IsNullOrWhiteSpace(value) ? string.Join("\r\n", DefaultContent) : value;
        }

        public bool IncludeFacials
        {
            get => facialsCheckBox.Checked;
            set => facialsCheckBox.Checked = value;
        }

        public bool IncludeHair
        {
            get => hairCheckBox.Checked;
            set => hairCheckBox.Checked = value;
        }

        public FixNormalsSettingsForm()
        {
            InitializeComponent();
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            try
            {
                ParseSettings(Data);
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

        public static List<uint> ParseSettings(string data)
        {
            var rows = data
                .Split('\r', '\n')
                .Select(_ =>
                {
                    if (_.IndexOf('#') != -1)
                        _ = _.Substring(0, _.IndexOf('#'));

                    return _.Trim();
                })
                .Where(_ => !string.IsNullOrWhiteSpace(_));

            var numbers = rows.Select(_ =>
            {
                if (_.Length > 4)
                    throw new Exception($"Wrong entry format: '{_}'");

                var res = 0u;

                for (var i = 0; i < _.Length; ++i)
                {
                    var c = _[_.Length - i - 1];

                    if (char.IsDigit(c) && uint.TryParse(c.ToString(), out var charVal))
                        res |= charVal << i;
                    else if (_[i] == 'x')
                        res |= 0x1Fu << i;
                    else
                        throw new Exception($"Can't parse '{_}'");
                }

                if (!uint.TryParse(_, out uint val))
                    throw new Exception($"Failed to parse '{_}'");

                return res;
            });

            return numbers.ToList();
        }
    }
}
