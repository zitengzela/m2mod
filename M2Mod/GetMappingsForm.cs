using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace M2Mod
{
    public partial class GetMappingsForm : Form
    {
        private const string nonClickablePart = "Download latest community listfile at: ";
        private const string downloadUrl = "https://wow.tools/files/";

        public GetMappingsForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            linkLabel.Text = nonClickablePart + downloadUrl;
            linkLabel.Links.Add(nonClickablePart.Length, downloadUrl.Length);
        }

        private void linkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start(downloadUrl);
        }

        private void closeButton_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }
    }
}
