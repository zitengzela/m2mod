using System;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using M2Mod.Interop;
using M2Mod.Interop.Structures;
using M2Mod.Registry;
using Newtonsoft.Json.Linq;

namespace M2Mod
{
    public partial class M2Mod : Form
    {
        private const string _m2Filter = "M2 Files|*.m2|All Files|*.*";
        private const string _m2IFilter = "M2I Files|*.m2i|All Files|*.*";

        private Settings _settings = Defaults.Settings;
        private bool _ignoreErrors = false;
        private IntPtr preloadM2 = IntPtr.Zero;

        public M2Mod()
        {
            InitializeComponent();

            this.Icon = Properties.Resources.Icon;

            var assemblyVersion = Assembly.GetExecutingAssembly().GetName().Version;
            var buildDate = (new DateTime(2000, 1, 1)).AddDays(assemblyVersion.Build)
                .AddSeconds(assemblyVersion.Revision * 2);

            Text = $"M2Mod {VersionString} built at {buildDate}";

            InitializeLogger();
            LoadSettingsFromRegistry();
        }

        private static string VersionString => $"v{Version.Major}.{Version.Minor}.{Version.Patch}";

        private Imports.LoggerDelegate dlg;

        private void InitializeLogger()
        {
            dlg = Log;
            Imports.AttachLoggerCallback(dlg);
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

        private void LoadSettingsFromRegistry()
        {
            foreach (var enumVal in Enum.GetValues(typeof(RegistryValue)).Cast<RegistryValue>())
            {
                string value = null;
                try
                {
                    value = RegistryStore.GetValue(enumVal).ToString();
                }
                catch
                {
                    continue;
                }

                if (string.IsNullOrEmpty(value))
                    continue;

                try
                {
                    switch (enumVal)
                    {
                        case RegistryValue.ExportM2:
                            textBoxInputM2Exp.Text = value;
                            break;
                        case RegistryValue.ExportM2I:
                            textBoxOutputM2I.Text = value;
                            break;
                        case RegistryValue.ImportInM2:
                            textBoxInputM2Imp.Text = value;
                            break;
                        case RegistryValue.ImportM2I:
                            textBoxInputM2I.Text = value;
                            break;
                        case RegistryValue.ImportOutM2:
                            break;

                        case RegistryValue.ImportReplaceM2:
                            textBoxReplaceM2.Text = value;
                            break;
                        case RegistryValue.ReplaceM2Checked:
                            checkBoxReplaceM2.Checked = bool.Parse(value);
                            break;

                        case RegistryValue.MergeBones:
                            _settings.MergeBones = bool.Parse(value);
                            break;
                        case RegistryValue.MergeAttachments:
                            _settings.MergeAttachments = bool.Parse(value);
                            break;
                        case RegistryValue.MergeCameras:
                            _settings.MergeCameras = bool.Parse(value);
                            break;
                        case RegistryValue.FixSeams:
                            _settings.FixSeams = bool.Parse(value);
                            break;
                        case RegistryValue.FixEdgeNormals:
                            _settings.FixEdgeNormals = bool.Parse(value);
                            break;
                        case RegistryValue.ForceExportExpansion:
                            _settings.ForceLoadExpansion = (Expansion) Enum.Parse(typeof(Expansion), value);
                            break;
                        case RegistryValue.IgnoreOriginalMeshIndexes:
                            _settings.IgnoreOriginalMeshIndexes = bool.Parse(value);
                            break;

                        case RegistryValue.FixAnimationsTest:
                            _settings.FixAnimationsTest = bool.Parse(value);
                            break;

                        case RegistryValue.WorkingDirectory:
                            _settings.WorkingDirectory = value;
                            break;

                        case RegistryValue.OutputDirectory:
                            _settings.OutputDirectory = value;
                            break;

                        case RegistryValue.OldCompareM2:
                        case RegistryValue.NewCompareM2:
                        case RegistryValue.CompareWeightThreshold:
                            break;
                    }
                }
                catch
                {
                    Log((int) LogLevel.Warning, $"Failed to parse {enumVal} registry value");
                }
            }
        }

        private void ButtonInputM2ExpBrowse_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = _m2Filter;

            try
            {
                openFileDialog1.FileName = textBoxInputM2Exp.Text;
                openFileDialog1.InitialDirectory = Path.GetDirectoryName(textBoxInputM2Exp.Text);
            }
            catch
            {
                // ignored
            }

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBoxInputM2Exp.Text = textBoxInputM2Imp.Text = openFileDialog1.FileName;

                textBoxOutputM2I.Text = textBoxInputM2I.Text = Path.ChangeExtension(openFileDialog1.FileName, "m2i");
            }
        }

        private void ButtonOutputM2IBrowse_Click(object sender, EventArgs e)
        {
            saveFileDialog1.Filter = _m2IFilter;
            try
            {
                saveFileDialog1.FileName = textBoxOutputM2I.Text;
                saveFileDialog1.InitialDirectory = Path.GetDirectoryName(textBoxOutputM2I.Text);
            }
            catch
            {
                // ignored
            }

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                textBoxOutputM2I.Text = saveFileDialog1.FileName;
        }

        private void ButtonInputM2IBrowse_Click(object sender, EventArgs e)
        {
            openFileDialog1.Filter = _m2IFilter;
            try
            {
                openFileDialog1.FileName = textBoxInputM2I.Text;
                openFileDialog1.InitialDirectory = Path.GetDirectoryName(textBoxInputM2I.Text);
            }
            catch
            {
                // ignored
            }

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
                textBoxInputM2I.Text = openFileDialog1.FileName;
        }

        private void ImportButtonPreload_Click(object sender, EventArgs e)
        {
            _ignoreErrors = false;
            importButtonPreload.Enabled = false;
            importButtonPreload.Refresh();
            SetStatus("Preloading...");

            if (preloadM2 != IntPtr.Zero)
            {
                Imports.M2_Free(preloadM2);
                preloadM2 = IntPtr.Zero;
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

            preloadM2 = Imports.M2_Create(ref _settings);

            var Error = Imports.M2_Load(preloadM2, textBoxInputM2Imp.Text);
            if (Error != M2LibError.OK)
            {
                SetStatus(Imports.GetErrorText(Error));
                PreloadTransition(false);
                return;
            }

            if (checkBoxReplaceM2.Checked)
            {
                Error = Imports.M2_SetReplaceM2(preloadM2, textBoxReplaceM2.Text);
                if (Error != M2LibError.OK)
                {
                    SetStatus(Imports.GetErrorText(Error));
                    PreloadTransition(false);
                    return;
                }
            }

            // import M2I
            Error = Imports.M2_ImportM2Intermediate(preloadM2, textBoxInputM2I.Text);
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

                if (preloadM2 != IntPtr.Zero)
                {
                    Imports.M2_Free(preloadM2);
                    preloadM2 = IntPtr.Zero;
                }
            }
        }

        private void SettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (var form = new SettingsForm())
            {
                form.Setup(_settings);
                if (form.ShowDialog() == DialogResult.OK)
                    _settings = form.ProduceSettings();
            }
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

            var m2 = Imports.M2_Create(ref _settings);

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
            openFileDialog1.Filter = _m2Filter;
            try
            {
                openFileDialog1.FileName = textBoxReplaceM2.Text;
                openFileDialog1.InitialDirectory = Path.GetDirectoryName(textBoxReplaceM2.Text);
            }
            catch
            {
                // ignored
            }

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
                textBoxReplaceM2.Text = openFileDialog1.FileName;
        }

        private void LoadListfileButton_Click(object sender, EventArgs e)
        {
            _ignoreErrors = false;

            using (var dialog = new OpenFileDialog())
            {
                dialog.InitialDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly()?.Location ?? "");
                dialog.FileName = "listfile.csv";
                dialog.Filter = "WoW Listfile|*.csv";
                var result = dialog.ShowDialog();
                if (result != DialogResult.OK)
                    return;

                Imports.LoadFileStorage(dialog.FileName);
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

            if (uint.TryParse(testInputTextBox.Text, out var fileDataId))
            {
                var info = Imports.GetFileInfoByFileDataId(fileDataId);
                if (info != IntPtr.Zero)
                    testOutputTextBox.Text = Imports.FileInfo_GetPath(info);
                else
                    testOutputTextBox.Text = "Not found in storage";
            }
            else
            {
                var info = Imports.GetFileInfoByPartialPath(testInputTextBox.Text);
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

        private void M2Mod_FormClosing(object sender, FormClosingEventArgs e)
        {
            foreach (var enumVal in Enum.GetValues(typeof(RegistryValue)).Cast<RegistryValue>())
            {
                object value = null;
                switch (enumVal)
                {
                    case RegistryValue.ExportM2:
                        value = textBoxInputM2Exp.Text;
                        break;
                    case RegistryValue.ExportM2I:
                        value = this.textBoxOutputM2I.Text;
                        break;
                    case RegistryValue.ImportInM2:
                        value = this.textBoxInputM2Imp.Text;
                        break;
                    case RegistryValue.ImportM2I:
                        value = this.textBoxInputM2I.Text;
                        break;
                    case RegistryValue.ImportReplaceM2:
                        value = this.textBoxReplaceM2.Text;
                        break;
                    case RegistryValue.ReplaceM2Checked:
                        value = this.checkBoxReplaceM2.Checked;
                        break;
                    case RegistryValue.WorkingDirectory:
                        value = _settings.WorkingDirectory;
                        break;
                    case RegistryValue.OutputDirectory:
                        value = _settings.OutputDirectory;
                        break;
                    case RegistryValue.ForceExportExpansion:
                        value = _settings.ForceLoadExpansion;
                        break;
                    case RegistryValue.MergeAttachments:
                        value = _settings.MergeAttachments;
                        break;
                    case RegistryValue.MergeBones:
                        value = _settings.MergeBones;
                        break;
                    case RegistryValue.MergeCameras:
                        value = _settings.MergeCameras;
                        break;
                    case RegistryValue.FixSeams:
                        value = _settings.FixSeams;
                        break;
                    case RegistryValue.FixEdgeNormals:
                        value = _settings.FixEdgeNormals;
                        break;
                    case RegistryValue.IgnoreOriginalMeshIndexes:
                        value = _settings.IgnoreOriginalMeshIndexes;
                        break;
                    case RegistryValue.FixAnimationsTest:
                        value = _settings.FixAnimationsTest;
                        break;
                }

                if (value == null)
                    continue;

                try
                {
                    RegistryStore.SetValue(enumVal, value);
                }
                catch
                {
                    // ignored
                }
            }
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
                form.Settings = _settings;
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

            if (preloadM2 == IntPtr.Zero)
            {
                SetStatus("Error: Model not preloaded");
                PreloadTransition(false);
                return;
            }

            var fileName = Path.GetFileName(checkBoxReplaceM2.Checked ? textBoxReplaceM2.Text : textBoxInputM2Imp.Text);
            string ExportFileName;
            if (_settings.OutputDirectory.Length > 0)
            {
                var outputDirectory = _settings.OutputDirectory;

                var info = Imports.GetFileInfoByPartialPath(fileName);
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
            var error = Imports.M2_Save(preloadM2, ExportFileName);
            if (error != M2LibError.OK)
            {
                SetStatus(Imports.GetErrorText(error));
                PreloadTransition(false);
                return;
            }

            SetStatus("Import done.");
            PreloadTransition(false);
        }
    }
}
