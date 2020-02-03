using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using M2Mod.Interop;

namespace M2Mod.Controls
{
    public partial class CueTextBox : TextBox
    {
        private string cue;

        [DefaultValue(""), Browsable(true), DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), Category("Appearance")]
        public string Cue
        {
            get { return string.IsNullOrWhiteSpace(cue) ? string.Empty : cue; }
            set
            {
                cue = string.IsNullOrWhiteSpace(value) ? string.Empty : value;

                CueProvider.SetCue(this, cue);
            }

        }

        public CueTextBox()
        {
            InitializeComponent();
        }
    }
}
