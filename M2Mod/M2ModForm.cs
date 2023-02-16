using System;
using System.IO;
using System.Linq;
using System.Net;
using System.Reflection;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using M2Mod.Config;
using M2Mod.Dialogs;
using M2Mod.Interop;
using M2Mod.Interop.Structures;
using M2Mod.Tools;
using Newtonsoft.Json.Linq;

namespace M2Mod
{
    public partial class M2ModForm : Form
    {
        private bool _ignoreErrors;
        private bool _ignoreWarnings;

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

        }

        private static string VersionString => $"v{Version.Major}.{Version.Minor}.{Version.Patch}";

        private Imports.LoggerDelegate logDelegate;

        private void InitializeLogger()
        {
            logDelegate = Log;
            Imports.AttachLoggerCallback(LogLevel.AllDefault, logDelegate);
        }

        private void ResetIgnoreWarnings()
        {
            _ignoreErrors = _ignoreWarnings = false;
        }

        private void Log(LogLevel logLevel, string message)
        {
            logTextBox.AppendLine(logLevel, message);

            if (logLevel.HasFlag(LogLevel.Error))
            {
                if (!_ignoreErrors)
                {
                    if (ErrorForm.ShowError(message) == DialogResult.Ignore)
                        _ignoreErrors = true;
                }
            }
            else if (logLevel.HasFlag(LogLevel.Warning))
            {
                if (!_ignoreWarnings)
                {
                    if (ErrorForm.ShowWarning(message) == DialogResult.Ignore)
                        _ignoreWarnings = true;
                }
            }
        }

        private void PreloadTransition(bool On)
        {
            if (!On)
            {
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
            ResetIgnoreWarnings();

            exportButtonGo.Enabled = false;
            exportButtonGo.Refresh();

            string[] allfiles = Directory.GetFiles(ProfileManager.CurrentProfile.Settings.WorkingDirectory, "*.m2", SearchOption.AllDirectories);

            foreach (string f in allfiles)
            {
                FileInfo file = new FileInfo(f);
                string filePath = Path.Combine(file.DirectoryName, file.Name);
                
                string fileEPath = filePath + "i";
                SetStatus("Processing: " + filePath);

                var m2 = Imports.M2_Create(ref ProfileManager.CurrentProfile.Settings);

                // import M2
                var error = Imports.M2_Load(m2, filePath);
                if (error != M2LibError.OK)
                {
                    SetStatus(Imports.GetErrorText(error));
                    exportButtonGo.Enabled = true;
                    Imports.M2_Free(m2);
                    return;
                }

                // export M2I
                error = Imports.M2_ExportM2Intermediate(m2, fileEPath);
                if (error != M2LibError.OK)
                {
                    SetStatus(Imports.GetErrorText(error));
                    exportButtonGo.Enabled = true;
                    Imports.M2_Free(m2);
                    return;
                }

                SetStatus("Export done: " + filePath);

                exportButtonGo.Enabled = true;

                Imports.M2_Free(m2);


                ResetIgnoreWarnings();
                SetStatus("Preloading: " + file.Name);

                if (_preloadM2 != IntPtr.Zero)
                {
                    Imports.M2_Free(_preloadM2);
                    _preloadM2 = IntPtr.Zero;
                }

                _preloadM2 = Imports.M2_Create(ref ProfileManager.CurrentProfile.Settings);

                var Error = Imports.M2_Load(_preloadM2, filePath);
                if (Error != M2LibError.OK)
                {
                    SetStatus(Imports.GetErrorText(Error));
                    PreloadTransition(false);
                    return;
                }


                try
                {
                    foreach (var ruleSet in ProfileManager.CurrentProfile.Configuration.NormalizationConfig.GetRules())
                    {
                        var sourceRules = ruleSet.SourceRules.Serialize().ToArray();
                        var targetRules = ruleSet.TargetRules.Serialize().ToArray();

                        Error = Imports.M2_AddNormalizationRule(_preloadM2,
                            ruleSet.SourceType, sourceRules, sourceRules.Length,
                            ruleSet.TargetType, targetRules, targetRules.Length, ruleSet.PreferSourceDirection);
                        if (Error != M2LibError.OK)
                        {
                            SetStatus(Imports.GetErrorText(Error));
                            PreloadTransition(false);
                            return;
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    SetStatus(ex.Message);
                    PreloadTransition(false);
                    return;
                }

                // import M2I
                Error = Imports.M2_ImportM2Intermediate(_preloadM2, fileEPath);
                if (Error != M2LibError.OK)
                {
                    //SetStatus(Imports.GetErrorText(Error));
                    //PreloadTransition(false);
                    return;
                }

                SetStatus("Preload finished: " + file.Name);

                PreloadTransition(true);




                ResetIgnoreWarnings();

                SetStatus("Importing: " + file.Name);

                if (_preloadM2 == IntPtr.Zero)
                {
                    SetStatus("Error: Model not preloaded");
                    PreloadTransition(false);
                    return;
                }

                var fileName = Path.GetFileName(filePath);
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
                    var outputDirectory = Path.Combine(Path.GetDirectoryName(filePath), "Export");
                    ExportFileName = Path.Combine(outputDirectory, fileName);
                }

                var directory = Directory.GetParent(ExportFileName).FullName;
                if (!Directory.Exists(directory))
                    Directory.CreateDirectory(directory);

                var error1 = Imports.M2_SetSaveMappingsCallback(_preloadM2, () =>
                {
                    var dialog = new SaveFileDialog { Filter = Filters.Txt };
                    try
                    {
                        dialog.Title = "Select mappings file to append entries to";
                        dialog.InitialDirectory = ProfileManager.CurrentProfile.Settings.MappingsDirectory;
                        dialog.FileName = Path.GetFileNameWithoutExtension(filePath) + ".txt";
                    }
                    catch
                    {
                        // ignored
                    }

                    if (dialog.ShowDialog() == DialogResult.OK)
                        return dialog.FileName;

                    return "";
                });

                if (error1 != M2LibError.OK)
                {
                    SetStatus(Imports.GetErrorText(error1));
                    PreloadTransition(false);
                    return;
                }

                // export M2
                error1 = Imports.M2_Save(_preloadM2, ExportFileName, SaveMask.All);
                if (error1 != M2LibError.OK)
                {
                    SetStatus(Imports.GetErrorText(error1));
                    PreloadTransition(false);
                    return;
                }

                SetStatus("Import done: " + file.Name);
                PreloadTransition(false);
            }


            exportButtonGo.Enabled = true;
            // Check fields.

        }

        private void ImportCancelButton_Click(object sender, EventArgs e)
        {
            SetStatus("Cancelled preload.");
            PreloadTransition(false);
        }

        private void LoadListfileButton_Click(object sender, EventArgs e)
        {
            ResetIgnoreWarnings();

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
            ResetIgnoreWarnings();

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
                        Log(LogLevel.Error, $"Failed to check updates: {ex.Message}");
                    }));
                    
                    MessageBox.Show("Failed to check updates", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            });
        }

        private void CompareModelsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ResetIgnoreWarnings();

            using (var form = new CompareModelsForm())
            {
                form.ShowDialog();
            }
        }

        private void ClearButton_Click(object sender, EventArgs e)
        {
            logTextBox.Text = "";
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
        }

        private void LoadProfilesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            using (var dialog = new OpenFileDialog())
            {
                dialog.Filter = Filters.Profile;
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

        private void tXIDRemoverToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ResetIgnoreWarnings();

            using (var form = new TXIDRemoverForm())
            {
                form.ShowDialog();
            }
        }

        private void getLatestMappingLinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            using (var form = new GetMappingsForm())
            {
                form.ShowDialog();
            }
        }

        private void remapReferencesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ResetIgnoreWarnings();

            using (var form = new RemapReferencesForm())
            {
                form.ShowDialog();
            }
        }

        private void splitButton2_Click(object sender, EventArgs e)
        {

        }

        private void splitButton1_Click(object sender, EventArgs e)
        {
            //string[] allfiles = Directory.GetFiles(ProfileManager.CurrentProfile.Settings.WorkingDirectory, "*.m2", SearchOption.AllDirectories);
            string[] allfiles = Directory.GetFiles(ProfileManager.CurrentProfile.Settings.WorkingDirectory, "*.m2", SearchOption.AllDirectories);
            
            foreach (string f in allfiles)
            {
                FileInfo file = new FileInfo(f);
                string filePath = Path.Combine(file.DirectoryName, file.Name);
                //var settings = new Settings()
                //{
                //    WorkingDirectory = file.Directory.FullName
                //};

                var settings = ProfileManager.CurrentProfile.Settings;

                SetStatus("Processing: " + filePath);

                var m2 = Imports.M2_Create(ref settings);
                var errorStatus = Imports.M2_Load(m2, filePath);
                if (errorStatus != M2LibError.OK)
                {
                    MessageBox.Show(string.Format("Failed load m2: {0}", Imports.GetErrorText(errorStatus)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    Imports.M2_Free(m2);
                    return;
                }

                logTextBox.Clear();

                errorStatus = Imports.M2_SetNeedRemoveTXIDChunk(m2);
                if (errorStatus != M2LibError.OK)
                {
                    MessageBox.Show(string.Format("Failed to prepare for chunk removal: {0}", Imports.GetErrorText(errorStatus)), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    Imports.M2_Free(m2);
                    return;
                }

                // export M2
                var error = Imports.M2_Save(m2, filePath, SaveMask.M2);
                if (error != M2LibError.OK)
                {
                    Imports.M2_Free(m2);
                    return;
                }

                SetStatus("TXID Removed: " + file.Name);
                Imports.M2_Free(m2);
            }

            SetStatus("All Files TXID removed! ");
        }

        private void splitButton2_Click_1(object sender, EventArgs e)
        {

        }
    }
}
