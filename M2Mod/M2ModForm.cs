using System;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using M2Mod.Config;
using M2Mod.Interop;
using M2Mod.Interop.Structures;
using Newtonsoft.Json.Linq;

namespace M2Mod
{
    public partial class M2ModForm : Form
    {
        private const string M2Filter = "M2 Files|*.m2|All Files|*.*";
        private const string M2IFilter = "M2I Files|*.m2i|All Files|*.*";
        private const string JsonFilter = "Profiles Files|*.json|All Files|*.*";

        private bool _ignoreErrors;
        private IntPtr _preloadM2 = IntPtr.Zero;

        public M2ModForm()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            var assemblyVersion = Assembly.GetExecutingAssembly().GetName().Version;
            var buildDate = (new DateTime(2000, 1, 1)).AddDays(assemblyVersion.Build)
                .AddSeconds(assemblyVersion.Revision * 2);

            Text = $"M2Mod {VersionString} built at {buildDate}";

            InitializeLogger();

            ProfileManager.Load("", true);

            InitializeProfiles();
            InitializeFormData();

            formInitialized = true;
        }

        private void InitializeFormData()
        {
            textBoxInputM2Exp.Text = ProfileManager.CurrentProfile.FormData.InputM2Exp;
            textBoxOutputM2I.Text = ProfileManager.CurrentProfile.FormData.OutputM2I;
            textBoxInputM2Imp.Text = ProfileManager.CurrentProfile.FormData.InputM2Imp;
            textBoxInputM2I.Text = ProfileManager.CurrentProfile.FormData.InputM2I;
            textBoxReplaceM2.Text = ProfileManager.CurrentProfile.FormData.ReplaceM2;
            checkBoxReplaceM2.Checked = ProfileManager.CurrentProfile.FormData.ReplaceM2Checked;
        }

        private static string VersionString => $"v{Version.Major}.{Version.Minor}.{Version.Patch}";

        private Imports.LoggerDelegate logDelegate;

        private void InitializeLogger()
        {
            logDelegate = Log;
            Imports.AttachLoggerCallback(logDelegate);
        }

        private void Log(int logLevel, string message)
        {
            Color textColor, backColor;
            switch ((LogLevel) logLevel)
            {
                case LogLevel.Error:
                    textColor = Color.Black;
                    backColor = Color.Red;
                    break;
                case LogLevel.Warning:
                    textColor = Color.Black;
                    backColor = Color.Yellow;
                    break;
                default:
                    textColor = backColor = Color.Empty;
                    break;
            }

            AppendLine(message, textColor, backColor);

            if (!_ignoreErrors)
            {
                if ((LogLevel) logLevel == LogLevel.Error)
                {
                    if (ErrorForm.ShowError(message) == DialogResult.Ignore)
                        _ignoreErrors = true;
                }
                else if ((LogLevel) logLevel == LogLevel.Warning)
                {
                    if (ErrorForm.ShowWarning(message) == DialogResult.Ignore)
                        _ignoreErrors = true;
                }
            }
        }

        private void AppendLine(string text, Color textColor, Color backColor)
        {
            text = $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}]: {text}";

            if (logTextBox.TextLength > 0)
                text = "\r\n" + text;

            if (textColor == Color.Empty)
            {
                logTextBox.AppendText(text);
                return;
            }

            logTextBox.SelectionStart = logTextBox.TextLength;
            logTextBox.SelectionLength = 0;

            logTextBox.SelectionColor = textColor;
            logTextBox.SelectionBackColor = backColor;
            logTextBox.AppendText(text);
            logTextBox.SelectionColor = logTextBox.ForeColor;
            logTextBox.SelectionBackColor = logTextBox.BackColor;

            logTextBox.SelectionStart = logTextBox.TextLength;
            logTextBox.ScrollToCaret();
        }

        private void ButtonInputM2ExpBrowse_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = M2Filter;

                try
                {
                    dialog.FileName = textBoxInputM2Exp.Text;
                    dialog.InitialDirectory = Path.GetDirectoryName(textBoxInputM2Exp.Text);
                }
                catch
                {
                    // ignored
                }

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    textBoxInputM2Exp.Text = textBoxInputM2Imp.Text = dialog.FileName;
                    textBoxOutputM2I.Text = textBoxInputM2I.Text = Path.ChangeExtension(dialog.FileName, "m2i");
                }
            }
        }

        private void ButtonOutputM2IBrowse_Click(object sender, EventArgs e)
        {
            using (var dialog = new SaveFileDialog())
            {
                dialog.Filter = M2IFilter;
                try
                {
                    dialog.FileName = textBoxOutputM2I.Text;
                    dialog.InitialDirectory = Path.GetDirectoryName(textBoxOutputM2I.Text);
                }
                catch
                {
                    // ignored
                }

                if (dialog.ShowDialog() == DialogResult.OK)
                    textBoxOutputM2I.Text = dialog.FileName;
            }
        }

        private void ButtonInputM2IBrowse_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = M2IFilter;
                try
                {
                    dialog.FileName = textBoxInputM2I.Text;
                    dialog.InitialDirectory = Path.GetDirectoryName(textBoxInputM2I.Text);
                }
                catch
                {
                    // ignored
                }

                if (dialog.ShowDialog() == DialogResult.OK)
                    textBoxInputM2I.Text = dialog.FileName;
            }
        }

        private void ImportButtonPreload_Click(object sender, EventArgs e)
        {
            _ignoreErrors = false;
            importButtonPreload.Enabled = false;
            importButtonPreload.Refresh();
            SetStatus("Preloading...");

            if (_preloadM2 != IntPtr.Zero)
            {
                Imports.M2_Free(_preloadM2);
                _preloadM2 = IntPtr.Zero;
            }

            // Check fields.
            if (textBoxInputM2I.Text.Length == 0)
            {
                SetStatus("Error: No input M2I file Specified.");
                PreloadTransition(false);
                return;
            }

            // import M2
            if (textBoxInputM2Imp.Text.Length == 0)
            {
                SetStatus("Error: No input M2 file Specified.");
                PreloadTransition(false);
                return;
            }

            _preloadM2 = Imports.M2_Create(ref ProfileManager.CurrentProfile.Settings);

            var Error = Imports.M2_Load(_preloadM2, textBoxInputM2Imp.Text);
            if (Error != M2LibError.OK)
            {
                SetStatus(Imports.GetErrorText(Error));
                PreloadTransition(false);
                return;
            }

            if (checkBoxReplaceM2.Checked)
            {
                Error = Imports.M2_SetReplaceM2(_preloadM2, textBoxReplaceM2.Text);
                if (Error != M2LibError.OK)
                {
                    SetStatus(Imports.GetErrorText(Error));
                    PreloadTransition(false);
                    return;
                }
            }

            // import M2I
            Error = Imports.M2_ImportM2Intermediate(_preloadM2, textBoxInputM2I.Text);
            if (Error != M2LibError.OK)
            {
                SetStatus(Imports.GetErrorText(Error));
                PreloadTransition(false);
                return;
            }

            SetStatus("Preload finished.");

            PreloadTransition(true);
        }

        private void PreloadTransition(bool On)
        {
            if (On)
            {
                panelInputM2Import.Enabled = false;
                panelInputM2I.Enabled = false;

                importButtonPreload.Enabled = false;
                importButtonGo.Enabled = true;
                extraworkPanel.Enabled = true;
                importCancelButton.Enabled = true;

                checkBoxReplaceM2.Enabled = false;
                panelReplaceM2.Enabled = false;
            }
            else
            {
                panelInputM2Import.Enabled = true;
                panelInputM2I.Enabled = true;

                importButtonPreload.Enabled = true;
                importButtonGo.Enabled = false;
                extraworkPanel.Enabled = false;
                importCancelButton.Enabled = false;

                checkBoxReplaceM2.Enabled = true;
                panelReplaceM2.Enabled = checkBoxReplaceM2.Checked;

                if (_preloadM2 != IntPtr.Zero)
                {
                    Imports.M2_Free(_preloadM2);
                    _preloadM2 = IntPtr.Zero;
                }
            }
        }

        private void SettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenSettings();
        }

        private void OpenSettings()
        {
            using (var form = new SettingsForm())
                form.ShowDialog();

            InitializeProfiles();
            InitializeFormData();
        }

        private void InitializeProfiles()
        {
            var profileGuid = ProfileManager.CurrentProfile?.Id ?? Guid.Empty;

            profilesComboBox.Items.Clear();
            profilesComboBox.Items.AddRange(ProfileManager.GetProfiles().Cast<object>().ToArray());
            profilesComboBox.SelectedItem = ProfileManager.GetProfiles().FirstOrDefault(_ => _.Id == profileGuid) ??
                                            profilesComboBox.Items[0];
        }

        private void ExitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void SetStatus(string status)
        {
            toolStripStatusLabel1.Text = status;
            statusStrip1.Refresh();
        }

        private void ExportButtonGo_Click(object sender, EventArgs e)
        {
            _ignoreErrors = false;
            exportButtonGo.Enabled = false;
            exportButtonGo.Refresh();
            SetStatus("Working...");

            // Check fields.
            if (textBoxInputM2Exp.Text.Length == 0)
            {
                SetStatus("M2LibError: No input M2 file Specified.");
                exportButtonGo.Enabled = true;
                return;
            }

            if (textBoxOutputM2I.Text.Length == 0)
            {
                SetStatus("M2LibError: No output M2I file Specified.");
                exportButtonGo.Enabled = true;
                return;
            }

            var m2 = Imports.M2_Create(ref ProfileManager.CurrentProfile.Settings);

            // import M2
            var error = Imports.M2_Load(m2, textBoxInputM2Exp.Text);
            if (error != M2LibError.OK)
            {
                SetStatus(Imports.GetErrorText(error));
                exportButtonGo.Enabled = true;
                Imports.M2_Free(m2);
                return;
            }

            // export M2I
            error = Imports.M2_ExportM2Intermediate(m2, textBoxOutputM2I.Text);
            if (error != M2LibError.OK)
            {
                SetStatus(Imports.GetErrorText(error));
                exportButtonGo.Enabled = true;
                Imports.M2_Free(m2);
                return;
            }

            SetStatus("Export done.");

            exportButtonGo.Enabled = true;

            Imports.M2_Free(m2);
        }

        private void ImportCancelButton_Click(object sender, EventArgs e)
        {
            SetStatus("Cancelled preload.");
            PreloadTransition(false);
        }

        private void CheckBoxReplaceM2_CheckedChanged(object sender, EventArgs e)
        {
            panelReplaceM2.Enabled = checkBoxReplaceM2.Checked;
        }

        private void ButtonReplaceM2Browse_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = M2Filter;
                try
                {
                    dialog.FileName = textBoxReplaceM2.Text;
                    dialog.InitialDirectory = Path.GetDirectoryName(textBoxReplaceM2.Text);
                }
                catch
                {
                    // ignored
                }

                if (dialog.ShowDialog() == DialogResult.OK)
                    textBoxReplaceM2.Text = dialog.FileName;
            }
        }

        private void LoadListfileButton_Click(object sender, EventArgs e)
        {
            _ignoreErrors = false;

            using (var dialog = new FolderBrowserDialog())
            {
                dialog.ShowNewFolderButton = false;
                dialog.SelectedPath = Path.GetDirectoryName(Assembly.GetEntryAssembly()?.Location ?? "");
                var result = dialog.ShowDialog();
                if (result != DialogResult.OK)
                    return;

                ProfileManager.CurrentProfile.Settings.MappingsDirectory = dialog.SelectedPath;
            }
        }

        private void TestInputTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode != Keys.Enter)
                return;

            TestFiles();
        }

        private void TestFiles()
        {
            testOutputTextBox.Text = "";
            if (testInputTextBox.Text.Length == 0)
                return;

            var fileStorage = Imports.FileStorage_Get(ProfileManager.CurrentProfile.Settings.MappingsDirectory);
            if (uint.TryParse(testInputTextBox.Text, out var fileDataId))
            {
                var info = Imports.FileStorage_GetFileInfoByFileDataId(fileStorage, fileDataId);
                if (info != IntPtr.Zero)
                    testOutputTextBox.Text = Imports.FileInfo_GetPath(info);
                else
                    testOutputTextBox.Text = "Not found in storage";
            }
            else
            {
                var info = Imports.FileStorage_GetFileInfoByPartialPath(fileStorage, testInputTextBox.Text);
                if (info != IntPtr.Zero)
                {
                    testInputTextBox.Text = Imports.FileInfo_GetPath(info);
                    testOutputTextBox.Text = Imports.FileInfo_GetFileDataId(info).ToString();
                }
                else
                    testOutputTextBox.Text = "Not found in storage";
            }

            testOutputTextBox.Focus();
            testOutputTextBox.SelectAll();
        }

        private void FileTestButton_Click(object sender, EventArgs e)
        {
            TestFiles();
        }

        private void SaveFormDataToProfile(SettingsProfile profile)
        {
            profile.FormData.InputM2Exp = textBoxInputM2Exp.Text;
            profile.FormData.OutputM2I = this.textBoxOutputM2I.Text;
            profile.FormData.InputM2Imp = this.textBoxInputM2Imp.Text;
            profile.FormData.InputM2I = this.textBoxInputM2I.Text;
            profile.FormData.ReplaceM2 = this.textBoxReplaceM2.Text;
            profile.FormData.ReplaceM2Checked = this.checkBoxReplaceM2.Checked;
        }

        private void M2Mod_FormClosing(object sender, FormClosingEventArgs e)
        {
            SaveFormDataToProfile(ProfileManager.CurrentProfile);

            ProfileManager.Save();
        }

        private string GetTags(string url)
        {
            var request = (HttpWebRequest) WebRequest.Create(url);
            request.Timeout = 20000;
            request.ReadWriteTimeout = 20000;
            var rest = (HttpWebResponse) request.GetResponse();

            using (var reader = new StreamReader(rest.GetResponseStream()))
                return reader.ReadToEnd();
        }

        private void CheckUpdates()
        {
            _ignoreErrors = true;
            var data = GetTags(
                @"https://bitbucket.org/!api/2.0/repositories/suncurio/m2mod/refs/tags?pagelen=30&q=name%20~%20%22v%22&sort=name");

            var json = JObject.Parse(data);

            var lastTag = json["values"]
                .Select(_ => _["name"].ToString())
                .Where(_ => Regex.IsMatch(_, @"v\d+\.\d+\.\d+"))
                .OrderBy(_ => _).Reverse()
                .FirstOrDefault();

            if (string.IsNullOrEmpty(lastTag))
                throw new Exception("Failed to get tags, empty result");

            if (!IsTagGreater(lastTag, VersionString))
            {
                ErrorForm.Show("Up to date", MessageBoxIcon.Information);
                return;
            }

            ErrorForm.ShowInfo(
                $"New version {lastTag} is available at https://bitbucket.org/suncurio/m2mod/downloads/");
        }

        private bool IsTagGreater(string source, string target)
        {
            Func<string, uint> f = tag =>
            {
                var match = Regex.Match(tag, @"(\d)+\.(\d+)\.(\d+)");
                if (!match.Success)
                    return 0;

                return uint.Parse(match.Groups[1].ToString()) << 16 |
                       uint.Parse(match.Groups[2].ToString()) << 8 |
                       uint.Parse(match.Groups[3].ToString());
            };

            return f(source) > f(target);
        }

        private Task _updateTask;
        private bool formInitialized;

        private void CheckUpdatesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (_updateTask != null && !_updateTask.IsCompleted)
                return;

            _updateTask = Task.Run(() =>
            {
                try
                {
                    CheckUpdates();
                }
                catch (Exception ex)
                {
                    this.Invoke(new Action(() => {
                        Log((int)LogLevel.Error, $"Failed to check updates: {ex.Message}");
                    }));
                    
                    MessageBox.Show("Failed to check updates", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            });
        }

        private void CompareBonesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _ignoreErrors = false;
            using (var form = new CompareBonesForm())
            {
                form.ShowDialog();
            }
        }

        private void ClearButton_Click(object sender, EventArgs e)
        {
            logTextBox.Text = "";
        }

        private void ImportButtonGo_Click(object sender, EventArgs e)
        {
            _ignoreErrors = false;
            importButtonPreload.Enabled = false;
            importButtonPreload.Refresh();
            SetStatus("Importing...");

            if (_preloadM2 == IntPtr.Zero)
            {
                SetStatus("Error: Model not preloaded");
                PreloadTransition(false);
                return;
            }

            var fileName = Path.GetFileName(checkBoxReplaceM2.Checked ? textBoxReplaceM2.Text : textBoxInputM2Imp.Text);
            string ExportFileName;
            if (ProfileManager.CurrentProfile.Settings.OutputDirectory.Length > 0)
            {
                var outputDirectory = ProfileManager.CurrentProfile.Settings.OutputDirectory;

                var fileStorage = Imports.FileStorage_Get(ProfileManager.CurrentProfile.Settings.MappingsDirectory);
                var info = Imports.FileStorage_GetFileInfoByPartialPath(fileStorage, fileName);
                if (info == IntPtr.Zero)
                {
                    SetStatus("Failed to determine model relative path in storage");
                    PreloadTransition(false);
                    return;
                }

                ExportFileName = Path.Combine(outputDirectory, Imports.FileInfo_GetPath(info));
            }
            else
            {
                var outputDirectory = Path.Combine(Path.GetDirectoryName(textBoxInputM2Imp.Text), "Export");
                ExportFileName = Path.Combine(outputDirectory, fileName);
            }

            var directory = Directory.GetParent(ExportFileName).FullName;
            if (!Directory.Exists(directory))
                Directory.CreateDirectory(directory);

            // export M2
            var error = Imports.M2_Save(_preloadM2, ExportFileName);
            if (error != M2LibError.OK)
            {
                SetStatus(Imports.GetErrorText(error));
                PreloadTransition(false);
                return;
            }

            SetStatus("Import done.");
            PreloadTransition(false);
        }

        private void ProfilesComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (formInitialized && ProfileManager.CurrentProfile != null)
                SaveFormDataToProfile(ProfileManager.CurrentProfile);

            ProfileManager.CurrentProfile = profilesComboBox.SelectedItem as SettingsProfile;
            InitializeFormData();
        }

        private void LoadMappingsButton_Click(object sender, EventArgs e)
        {
            Imports.FileStorage_Clear();
        }

        private void CustomMenuItem_Click(object sender, EventArgs e)
        {
            OpenSettings();
            ExportButtonGo_Click(this, e);
        }

        private void ToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            OpenSettings();
            ImportButtonPreload_Click(this, e);
        }

        private void LoadProfilesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = JsonFilter;
                try
                {
                    dialog.FileName = "profiles.json";
                    dialog.InitialDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
                }
                catch
                {
                    // ignored
                }

                if (dialog.ShowDialog() == DialogResult.OK && ProfileManager.Load(dialog.FileName, false))
                {
                    formInitialized = false;
                    InitializeProfiles();
                    InitializeFormData();
                    formInitialized = true;
                }
            }
        }
    }
}
