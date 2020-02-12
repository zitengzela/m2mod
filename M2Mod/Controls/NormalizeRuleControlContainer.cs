using System;
using System.Windows.Forms;

namespace M2Mod.Controls
{
    public partial class NormalizeRuleControlContainer : UserControl
    {
        public NormalizeRuleControlContainer()
        {
            InitializeComponent();
        }

        public event EventHandler OnCrossClicked;

        public NormalizationRuleControl Rules => normalizationRuleControl1;

        private void button1_Click(object sender, EventArgs e)
        {
            OnCrossClicked?.Invoke(this, e);
        }
    }
}
