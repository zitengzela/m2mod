#pragma once

#ifndef _DEFINE_FILTERS
# define _DEFINE_FILTERS
# define M2Filter L"M2 Files|*.m2|All Files|*.*"
# define M2IFilter L"M2I Files|*.m2i|All Files|*.*"
#endif

#include "Casc.h"
#include "Logger.h"
#include "ElementManagementForm.h"
#include "MeshInfoControl.h"
#include "RegistryStore.h"
#include "Version.h"
#include "Updater.h"
#include "Settings.h"
#include "SettingsForm.h"
#include "CompareBonesForm.h"
#include "StringHelpers.h"

namespace M2ModRedux
{

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading::Tasks;
	using namespace System::IO;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			Instance = this;

			InitializeComponent();

			this->Text = String::Format(L"M2Mod Redux {0}.{1}.{2}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
			settings = new M2Lib::GlobalSettings();

			InitializeLogger();
			LoadSettingsFromRegistry();
			AnalyzeCasc();

			auto updater = gcnew Updater();
			auto func = gcnew Func<int>(updater, &Updater::Update);
			Task::Run(func);

		}
	private: System::Windows::Forms::TabPage^  cascTabPage;
	private: System::Windows::Forms::Label^  cascInfoLabel;
	private: System::Windows::Forms::TextBox^  cascInfoTextBox;

	private: System::Windows::Forms::Button^  loadListfileButton;

	private: System::Windows::Forms::TextBox^  testOutputTextBox;

	private: System::Windows::Forms::TextBox^  testInputTextBox;

	private: System::Windows::Forms::Label^  fileTestLabel;
	private: System::Windows::Forms::Button^  fileTestButton;
	private: System::Windows::Forms::Button^  refreshButton;


	public:

		static Form1^ Instance = nullptr;

		private: System::Void InitializeLogger()
		{
			auto delegate = gcnew LoggerDelegate(Log);
			GCHandle gch = GCHandle::Alloc(delegate);
			IntPtr ip = Marshal::GetFunctionPointerForDelegate(delegate);
			auto callback = static_cast<M2Lib::LoggerCallback>(ip.ToPointer());
			GC::Collect();

			sLogger.AttachCallback(callback);
		}

		private: void AnalyzeCasc()
		{
			std::string text = "Storage path: ";
			if (settings && settings->WowPath.length())
				text += settings->WowPath + "\r\n";
			else
				text += "<not specified>\r\n";

			auto casc = GetCasc();
			text += "Casc status: ";
			if (settings->WowPath.empty())
				text += "<storage not specified>\r\n";
			else if (!casc->StorageInitialized())
				text += "<not loaded>\r\n";
			else
				text += "<loaded>\r\n";

			text += "Cache size: ";
			if (!casc->CacheLoaded())
				text += "<not loaded>\r\n";
			else
				text += std::to_string(casc->GetCacheSize()) + " files\r\n";

			cascInfoTextBox->Text = gcnew String(text.c_str());
		}

		private: void TestFiles()
		{
			testOutputTextBox->Text = "";
			if (!testInputTextBox->Text->Length)
				return;

			auto casc = GetCasc();
			System::UInt32 FileDataId;
			if (System::UInt32::TryParse(testInputTextBox->Text, FileDataId))
			{
				auto path = casc->GetFileByFileDataId(FileDataId);
				if (!path.empty())
					testOutputTextBox->Text = gcnew String(path.c_str());
				else
					testOutputTextBox->Text = "Not cached";
			}
			else
			{
				FileDataId = casc->GetFileDataIdByFile(StringConverter(testInputTextBox->Text).ToStringA());
				if (!FileDataId)
					testOutputTextBox->Text = "Not found in storage";
				else
					testOutputTextBox->Text = FileDataId.ToString();
			}

			AnalyzeCasc();

			testOutputTextBox->Focus();
			testOutputTextBox->SelectAll();
		}

		private: System::Void LoadSettingsFromRegistry()
		{
			try
			{
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::ExportM2))
					this->textBoxInputM2Exp->Text = (String^)value;

				if (auto value = RegistyStore::GetValue(RegistyStore::Value::ExportM2I))
					this->textBoxOutputM2I->Text = (String^)value;

				if (auto value = RegistyStore::GetValue(RegistyStore::Value::ImportInM2))
					this->textBoxInputM2Imp->Text = (String^)value;
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::ImportM2I))
					this->textBoxInputM2I->Text = (String^)value;
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::ImportReplaceM2))
					this->textBoxReplaceM2->Text = (String^)value;

				if (auto value = RegistyStore::GetValue(RegistyStore::Value::WowPath))
					settings->WowPath = StringConverter((String^)value).ToStringA();
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::UnloadCascOnDereference))
					settings->UnloadCascOnDereference = Boolean::Parse(value->ToString());

				if (auto value = RegistyStore::GetValue(RegistyStore::Value::ForceExportExpansion))
					settings->ForceLoadExpansion = (M2Lib::Expansion)Int32::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::MergeAttachments))
					settings->MergeAttachments = Boolean::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::MergeBones))
					settings->MergeBones = Boolean::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::MergeCameras))
					settings->MergeCameras = Boolean::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::FixSeams))
					settings->FixSeams = Boolean::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::IgnoreOriginalMeshIndexes))
					settings->IgnoreOriginalMeshIndexes = Boolean::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::FixAnimationsTest))
					settings->FixAnimationsTest = Boolean::Parse(value->ToString());
			}
			catch (...)
			{
			}
		}

	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {

		RegistyStore::SetValue(RegistyStore::Value::ExportM2, textBoxInputM2Exp->Text);

		RegistyStore::SetValue(RegistyStore::Value::ExportM2I, this->textBoxOutputM2I->Text);

		RegistyStore::SetValue(RegistyStore::Value::ImportInM2, this->textBoxInputM2Imp->Text);
		RegistyStore::SetValue(RegistyStore::Value::ImportM2I, this->textBoxInputM2I->Text);
		RegistyStore::SetValue(RegistyStore::Value::ImportReplaceM2, this->textBoxReplaceM2->Text);

		RegistyStore::SetValue(RegistyStore::Value::WowPath, gcnew String(settings->WowPath.c_str()));
		RegistyStore::SetValue(RegistyStore::Value::UnloadCascOnDereference, settings->UnloadCascOnDereference);

		RegistyStore::SetValue(RegistyStore::Value::ForceExportExpansion, (SInt32)settings->ForceLoadExpansion);
		RegistyStore::SetValue(RegistyStore::Value::MergeAttachments, settings->MergeAttachments);
		RegistyStore::SetValue(RegistyStore::Value::MergeBones, settings->MergeBones);
		RegistyStore::SetValue(RegistyStore::Value::MergeCameras, settings->MergeCameras);
		RegistyStore::SetValue(RegistyStore::Value::FixSeams, settings->FixSeams);
		RegistyStore::SetValue(RegistyStore::Value::IgnoreOriginalMeshIndexes, settings->IgnoreOriginalMeshIndexes);
		RegistyStore::SetValue(RegistyStore::Value::FixAnimationsTest, settings->FixAnimationsTest);
	}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
			delete settings;
		}

	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::ToolTip^  toolTip1;
private: System::Windows::Forms::TabControl^  tabControl;



	private: System::Windows::Forms::TabPage^  tabExport;
	private: System::Windows::Forms::Panel^  panelImputM2Exp;
	private: System::Windows::Forms::TextBox^  textBoxInputM2Exp;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  buttonInputM2ExpBrowse;
	private: System::Windows::Forms::Panel^  panelOutputM2I;
	private: System::Windows::Forms::Button^  buttonOutputM2IBrowse;
	private: System::Windows::Forms::TextBox^  textBoxOutputM2I;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::TabPage^  tabImport;
	private: System::Windows::Forms::Panel^  panelInputM2Import;
	private: System::Windows::Forms::TextBox^  textBoxInputM2Imp;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Button^  buttonInputM2ImpBrowse;
	private: System::Windows::Forms::Panel^  panelInputM2I;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  buttonInputM2IBrowse;
	private: System::Windows::Forms::TextBox^  textBoxInputM2I;




	private: System::Windows::Forms::Button^  exportButtonGo;

	private: System::Windows::Forms::Button^  importButtonGo;
	private: System::Windows::Forms::Button^  importButtonPreload;
	private: System::Windows::Forms::Button^  importCancelButton;
	private: System::Windows::Forms::Panel^  extraworkPanel;
	private: System::Windows::Forms::Button^  manageMeshesButton;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::CheckBox^  checkBoxReplaceM2;

	private: System::Windows::Forms::Panel^  panelReplaceM2;
	private: System::Windows::Forms::TextBox^  textBoxReplaceM2;
	private: System::Windows::Forms::Button^  buttonReplaceM2Browse;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  settingsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  compareBonesToolStripMenuItem;

private: System::Windows::Forms::TabPage^  tabLog;
private: System::Windows::Forms::TextBox^  logTextBox;
private: System::Windows::Forms::Button^  clearButton;






	private: System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
			 /// <summary>
			 /// Required method for Designer support - do not modify
			 /// the contents of this method with the code editor.
			 /// </summary>
			 void InitializeComponent(void)
			 {
				 this->components = (gcnew System::ComponentModel::Container());
				 System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
				 this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
				 this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
				 this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
				 this->label1 = (gcnew System::Windows::Forms::Label());
				 this->buttonInputM2ExpBrowse = (gcnew System::Windows::Forms::Button());
				 this->buttonOutputM2IBrowse = (gcnew System::Windows::Forms::Button());
				 this->label7 = (gcnew System::Windows::Forms::Label());
				 this->label3 = (gcnew System::Windows::Forms::Label());
				 this->buttonInputM2IBrowse = (gcnew System::Windows::Forms::Button());
				 this->exportButtonGo = (gcnew System::Windows::Forms::Button());
				 this->label5 = (gcnew System::Windows::Forms::Label());
				 this->buttonInputM2ImpBrowse = (gcnew System::Windows::Forms::Button());
				 this->importButtonGo = (gcnew System::Windows::Forms::Button());
				 this->importButtonPreload = (gcnew System::Windows::Forms::Button());
				 this->importCancelButton = (gcnew System::Windows::Forms::Button());
				 this->manageMeshesButton = (gcnew System::Windows::Forms::Button());
				 this->textBoxInputM2Imp = (gcnew System::Windows::Forms::TextBox());
				 this->textBoxInputM2I = (gcnew System::Windows::Forms::TextBox());
				 this->textBoxReplaceM2 = (gcnew System::Windows::Forms::TextBox());
				 this->buttonReplaceM2Browse = (gcnew System::Windows::Forms::Button());
				 this->checkBoxReplaceM2 = (gcnew System::Windows::Forms::CheckBox());
				 this->tabControl = (gcnew System::Windows::Forms::TabControl());
				 this->tabExport = (gcnew System::Windows::Forms::TabPage());
				 this->panelImputM2Exp = (gcnew System::Windows::Forms::Panel());
				 this->textBoxInputM2Exp = (gcnew System::Windows::Forms::TextBox());
				 this->panelOutputM2I = (gcnew System::Windows::Forms::Panel());
				 this->textBoxOutputM2I = (gcnew System::Windows::Forms::TextBox());
				 this->tabImport = (gcnew System::Windows::Forms::TabPage());
				 this->panelReplaceM2 = (gcnew System::Windows::Forms::Panel());
				 this->panel1 = (gcnew System::Windows::Forms::Panel());
				 this->extraworkPanel = (gcnew System::Windows::Forms::Panel());
				 this->panelInputM2Import = (gcnew System::Windows::Forms::Panel());
				 this->panelInputM2I = (gcnew System::Windows::Forms::Panel());
				 this->cascTabPage = (gcnew System::Windows::Forms::TabPage());
				 this->refreshButton = (gcnew System::Windows::Forms::Button());
				 this->fileTestButton = (gcnew System::Windows::Forms::Button());
				 this->testOutputTextBox = (gcnew System::Windows::Forms::TextBox());
				 this->testInputTextBox = (gcnew System::Windows::Forms::TextBox());
				 this->fileTestLabel = (gcnew System::Windows::Forms::Label());
				 this->loadListfileButton = (gcnew System::Windows::Forms::Button());
				 this->cascInfoTextBox = (gcnew System::Windows::Forms::TextBox());
				 this->cascInfoLabel = (gcnew System::Windows::Forms::Label());
				 this->tabLog = (gcnew System::Windows::Forms::TabPage());
				 this->clearButton = (gcnew System::Windows::Forms::Button());
				 this->logTextBox = (gcnew System::Windows::Forms::TextBox());
				 this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
				 this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
				 this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
				 this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->settingsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->toolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->compareBonesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				 this->tabControl->SuspendLayout();
				 this->tabExport->SuspendLayout();
				 this->panelImputM2Exp->SuspendLayout();
				 this->panelOutputM2I->SuspendLayout();
				 this->tabImport->SuspendLayout();
				 this->panelReplaceM2->SuspendLayout();
				 this->panel1->SuspendLayout();
				 this->extraworkPanel->SuspendLayout();
				 this->panelInputM2Import->SuspendLayout();
				 this->panelInputM2I->SuspendLayout();
				 this->cascTabPage->SuspendLayout();
				 this->tabLog->SuspendLayout();
				 this->statusStrip1->SuspendLayout();
				 this->menuStrip1->SuspendLayout();
				 this->SuspendLayout();
				 // 
				 // openFileDialog1
				 // 
				 this->openFileDialog1->FileName = L"openFileDialog1";
				 // 
				 // toolTip1
				 // 
				 this->toolTip1->AutomaticDelay = 300;
				 this->toolTip1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(38)), static_cast<System::Int32>(static_cast<System::Byte>(16)),
					 static_cast<System::Int32>(static_cast<System::Byte>(4)));
				 this->toolTip1->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(242)), static_cast<System::Int32>(static_cast<System::Byte>(94)),
					 static_cast<System::Int32>(static_cast<System::Byte>(131)));
				 this->toolTip1->UseAnimation = false;
				 this->toolTip1->UseFading = false;
				 // 
				 // label1
				 // 
				 this->label1->AutoSize = true;
				 this->label1->Location = System::Drawing::Point(20, 7);
				 this->label1->Name = L"label1";
				 this->label1->Size = System::Drawing::Size(46, 13);
				 this->label1->TabIndex = 3;
				 this->label1->Text = L"InputM2";
				 this->toolTip1->SetToolTip(this->label1, L"Required. Select an M2 for M2Mod to work with.");
				 // 
				 // buttonInputM2ExpBrowse
				 // 
				 this->buttonInputM2ExpBrowse->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
				 this->buttonInputM2ExpBrowse->Location = System::Drawing::Point(472, 3);
				 this->buttonInputM2ExpBrowse->Name = L"buttonInputM2ExpBrowse";
				 this->buttonInputM2ExpBrowse->Size = System::Drawing::Size(55, 20);
				 this->buttonInputM2ExpBrowse->TabIndex = 5;
				 this->buttonInputM2ExpBrowse->Text = L"...";
				 this->toolTip1->SetToolTip(this->buttonInputM2ExpBrowse, L"Browse...");
				 this->buttonInputM2ExpBrowse->UseVisualStyleBackColor = true;
				 this->buttonInputM2ExpBrowse->Click += gcnew System::EventHandler(this, &Form1::buttonInputM2Browse_Click);
				 // 
				 // buttonOutputM2IBrowse
				 // 
				 this->buttonOutputM2IBrowse->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
				 this->buttonOutputM2IBrowse->Location = System::Drawing::Point(473, 3);
				 this->buttonOutputM2IBrowse->Name = L"buttonOutputM2IBrowse";
				 this->buttonOutputM2IBrowse->Size = System::Drawing::Size(55, 20);
				 this->buttonOutputM2IBrowse->TabIndex = 8;
				 this->buttonOutputM2IBrowse->Text = L"...";
				 this->toolTip1->SetToolTip(this->buttonOutputM2IBrowse, L"Browse...");
				 this->buttonOutputM2IBrowse->UseVisualStyleBackColor = true;
				 this->buttonOutputM2IBrowse->Click += gcnew System::EventHandler(this, &Form1::buttonOutputM2IBrowse_Click);
				 // 
				 // label7
				 // 
				 this->label7->AutoSize = true;
				 this->label7->Location = System::Drawing::Point(10, 6);
				 this->label7->Name = L"label7";
				 this->label7->Size = System::Drawing::Size(57, 13);
				 this->label7->TabIndex = 6;
				 this->label7->Text = L"OutputM2I";
				 this->toolTip1->SetToolTip(this->label7, L"Optional. If set, M2Mod will convert InputM2 to an M2I and save it here.");
				 // 
				 // label3
				 // 
				 this->label3->AutoSize = true;
				 this->label3->Location = System::Drawing::Point(17, 8);
				 this->label3->Name = L"label3";
				 this->label3->Size = System::Drawing::Size(49, 13);
				 this->label3->TabIndex = 9;
				 this->label3->Text = L"InputM2I";
				 this->toolTip1->SetToolTip(this->label3, L"Optional. If set, M2Mod will merge InputM2 with InputM2I to create a modified M2 "
					 L"which will be saved to OutputM2.");
				 // 
				 // buttonInputM2IBrowse
				 // 
				 this->buttonInputM2IBrowse->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
				 this->buttonInputM2IBrowse->Location = System::Drawing::Point(472, 5);
				 this->buttonInputM2IBrowse->Name = L"buttonInputM2IBrowse";
				 this->buttonInputM2IBrowse->Size = System::Drawing::Size(55, 20);
				 this->buttonInputM2IBrowse->TabIndex = 11;
				 this->buttonInputM2IBrowse->Text = L"...";
				 this->toolTip1->SetToolTip(this->buttonInputM2IBrowse, L"Browse...");
				 this->buttonInputM2IBrowse->UseVisualStyleBackColor = true;
				 this->buttonInputM2IBrowse->Click += gcnew System::EventHandler(this, &Form1::buttonInputM2IBrowse_Click);
				 // 
				 // exportButtonGo
				 // 
				 this->exportButtonGo->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				 this->exportButtonGo->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->exportButtonGo->Location = System::Drawing::Point(214, 208);
				 this->exportButtonGo->Name = L"exportButtonGo";
				 this->exportButtonGo->Size = System::Drawing::Size(142, 38);
				 this->exportButtonGo->TabIndex = 26;
				 this->exportButtonGo->Text = L"Go!";
				 this->toolTip1->SetToolTip(this->exportButtonGo, L"Click this to perform operations.");
				 this->exportButtonGo->UseVisualStyleBackColor = true;
				 this->exportButtonGo->Click += gcnew System::EventHandler(this, &Form1::buttonGo_Click);
				 // 
				 // label5
				 // 
				 this->label5->AutoSize = true;
				 this->label5->Location = System::Drawing::Point(20, 7);
				 this->label5->Name = L"label5";
				 this->label5->Size = System::Drawing::Size(46, 13);
				 this->label5->TabIndex = 3;
				 this->label5->Text = L"InputM2";
				 this->toolTip1->SetToolTip(this->label5, L"Required. Select an M2 for M2Mod to work with.");
				 // 
				 // buttonInputM2ImpBrowse
				 // 
				 this->buttonInputM2ImpBrowse->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
				 this->buttonInputM2ImpBrowse->Location = System::Drawing::Point(472, 3);
				 this->buttonInputM2ImpBrowse->Name = L"buttonInputM2ImpBrowse";
				 this->buttonInputM2ImpBrowse->Size = System::Drawing::Size(55, 20);
				 this->buttonInputM2ImpBrowse->TabIndex = 5;
				 this->buttonInputM2ImpBrowse->Text = L"...";
				 this->toolTip1->SetToolTip(this->buttonInputM2ImpBrowse, L"Browse...");
				 this->buttonInputM2ImpBrowse->UseVisualStyleBackColor = true;
				 this->buttonInputM2ImpBrowse->Click += gcnew System::EventHandler(this, &Form1::buttonInputM2Browse_Click);
				 // 
				 // importButtonGo
				 // 
				 this->importButtonGo->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				 this->importButtonGo->Enabled = false;
				 this->importButtonGo->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->importButtonGo->Location = System::Drawing::Point(207, 10);
				 this->importButtonGo->Name = L"importButtonGo";
				 this->importButtonGo->Size = System::Drawing::Size(142, 38);
				 this->importButtonGo->TabIndex = 27;
				 this->importButtonGo->Text = L"Go!";
				 this->toolTip1->SetToolTip(this->importButtonGo, L"Produce M2");
				 this->importButtonGo->UseVisualStyleBackColor = true;
				 this->importButtonGo->Click += gcnew System::EventHandler(this, &Form1::importButtonGo_Click);
				 // 
				 // importButtonPreload
				 // 
				 this->importButtonPreload->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				 this->importButtonPreload->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold,
					 System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				 this->importButtonPreload->Location = System::Drawing::Point(54, 10);
				 this->importButtonPreload->Name = L"importButtonPreload";
				 this->importButtonPreload->Size = System::Drawing::Size(142, 38);
				 this->importButtonPreload->TabIndex = 28;
				 this->importButtonPreload->Text = L"Preload";
				 this->toolTip1->SetToolTip(this->importButtonPreload, L"Load source M2 and import M2I");
				 this->importButtonPreload->UseVisualStyleBackColor = true;
				 this->importButtonPreload->Click += gcnew System::EventHandler(this, &Form1::importButtonPreload_Click);
				 // 
				 // importCancelButton
				 // 
				 this->importCancelButton->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				 this->importCancelButton->Enabled = false;
				 this->importCancelButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
					 static_cast<System::Byte>(0)));
				 this->importCancelButton->Location = System::Drawing::Point(359, 10);
				 this->importCancelButton->Name = L"importCancelButton";
				 this->importCancelButton->Size = System::Drawing::Size(142, 38);
				 this->importCancelButton->TabIndex = 29;
				 this->importCancelButton->Text = L"Cancel";
				 this->toolTip1->SetToolTip(this->importCancelButton, L"Cancel preload");
				 this->importCancelButton->UseVisualStyleBackColor = true;
				 this->importCancelButton->Click += gcnew System::EventHandler(this, &Form1::importCancelButton_Click);
				 // 
				 // manageMeshesButton
				 // 
				 this->manageMeshesButton->Location = System::Drawing::Point(9, 3);
				 this->manageMeshesButton->Name = L"manageMeshesButton";
				 this->manageMeshesButton->Size = System::Drawing::Size(107, 30);
				 this->manageMeshesButton->TabIndex = 0;
				 this->manageMeshesButton->Text = L"Manage Meshes";
				 this->toolTip1->SetToolTip(this->manageMeshesButton, L"Open Mesh Management UI");
				 this->manageMeshesButton->UseVisualStyleBackColor = true;
				 this->manageMeshesButton->Click += gcnew System::EventHandler(this, &Form1::manageMeshesButton_Click);
				 // 
				 // textBoxInputM2Imp
				 // 
				 this->textBoxInputM2Imp->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->textBoxInputM2Imp->Location = System::Drawing::Point(88, 3);
				 this->textBoxInputM2Imp->Name = L"textBoxInputM2Imp";
				 this->textBoxInputM2Imp->Size = System::Drawing::Size(378, 20);
				 this->textBoxInputM2Imp->TabIndex = 4;
				 this->toolTip1->SetToolTip(this->textBoxInputM2Imp, L"Path to source M2");
				 // 
				 // textBoxInputM2I
				 // 
				 this->textBoxInputM2I->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->textBoxInputM2I->Location = System::Drawing::Point(88, 5);
				 this->textBoxInputM2I->Name = L"textBoxInputM2I";
				 this->textBoxInputM2I->Size = System::Drawing::Size(378, 20);
				 this->textBoxInputM2I->TabIndex = 10;
				 this->toolTip1->SetToolTip(this->textBoxInputM2I, L"Path to M2I");
				 // 
				 // textBoxReplaceM2
				 // 
				 this->textBoxReplaceM2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->textBoxReplaceM2->Location = System::Drawing::Point(8, 3);
				 this->textBoxReplaceM2->Name = L"textBoxReplaceM2";
				 this->textBoxReplaceM2->Size = System::Drawing::Size(378, 20);
				 this->textBoxReplaceM2->TabIndex = 13;
				 this->toolTip1->SetToolTip(this->textBoxReplaceM2, L"Destination path for M2");
				 // 
				 // buttonReplaceM2Browse
				 // 
				 this->buttonReplaceM2Browse->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
				 this->buttonReplaceM2Browse->Location = System::Drawing::Point(392, 2);
				 this->buttonReplaceM2Browse->Name = L"buttonReplaceM2Browse";
				 this->buttonReplaceM2Browse->Size = System::Drawing::Size(55, 20);
				 this->buttonReplaceM2Browse->TabIndex = 14;
				 this->buttonReplaceM2Browse->Text = L"...";
				 this->toolTip1->SetToolTip(this->buttonReplaceM2Browse, L"Browse...");
				 this->buttonReplaceM2Browse->UseVisualStyleBackColor = true;
				 this->buttonReplaceM2Browse->Click += gcnew System::EventHandler(this, &Form1::buttonReplaceM2Browse_Click);
				 // 
				 // checkBoxReplaceM2
				 // 
				 this->checkBoxReplaceM2->AutoSize = true;
				 this->checkBoxReplaceM2->Enabled = false;
				 this->checkBoxReplaceM2->Location = System::Drawing::Point(7, 73);
				 this->checkBoxReplaceM2->Name = L"checkBoxReplaceM2";
				 this->checkBoxReplaceM2->Size = System::Drawing::Size(81, 17);
				 this->checkBoxReplaceM2->TabIndex = 15;
				 this->checkBoxReplaceM2->Text = L"ReplaceM2";
				 this->toolTip1->SetToolTip(this->checkBoxReplaceM2, L"If you are swapping models, select target M2 that you would like to swap from.");
				 this->checkBoxReplaceM2->UseVisualStyleBackColor = true;
				 this->checkBoxReplaceM2->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBoxReplaceM2_CheckedChanged);
				 // 
				 // tabControl
				 // 
				 this->tabControl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
					 | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->tabControl->Controls->Add(this->tabExport);
				 this->tabControl->Controls->Add(this->tabImport);
				 this->tabControl->Controls->Add(this->cascTabPage);
				 this->tabControl->Controls->Add(this->tabLog);
				 this->tabControl->Location = System::Drawing::Point(1, 24);
				 this->tabControl->Name = L"tabControl";
				 this->tabControl->Padding = System::Drawing::Point(10, 3);
				 this->tabControl->SelectedIndex = 0;
				 this->tabControl->Size = System::Drawing::Size(570, 277);
				 this->tabControl->TabIndex = 25;
				 // 
				 // tabExport
				 // 
				 this->tabExport->Controls->Add(this->panelImputM2Exp);
				 this->tabExport->Controls->Add(this->panelOutputM2I);
				 this->tabExport->Controls->Add(this->exportButtonGo);
				 this->tabExport->Location = System::Drawing::Point(4, 22);
				 this->tabExport->Name = L"tabExport";
				 this->tabExport->Padding = System::Windows::Forms::Padding(3);
				 this->tabExport->Size = System::Drawing::Size(562, 251);
				 this->tabExport->TabIndex = 0;
				 this->tabExport->Text = L"Export";
				 this->tabExport->UseVisualStyleBackColor = true;
				 // 
				 // panelImputM2Exp
				 // 
				 this->panelImputM2Exp->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->panelImputM2Exp->Controls->Add(this->textBoxInputM2Exp);
				 this->panelImputM2Exp->Controls->Add(this->label1);
				 this->panelImputM2Exp->Controls->Add(this->buttonInputM2ExpBrowse);
				 this->panelImputM2Exp->Location = System::Drawing::Point(6, 9);
				 this->panelImputM2Exp->Name = L"panelImputM2Exp";
				 this->panelImputM2Exp->Size = System::Drawing::Size(531, 24);
				 this->panelImputM2Exp->TabIndex = 19;
				 // 
				 // textBoxInputM2Exp
				 // 
				 this->textBoxInputM2Exp->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->textBoxInputM2Exp->Location = System::Drawing::Point(88, 3);
				 this->textBoxInputM2Exp->Name = L"textBoxInputM2Exp";
				 this->textBoxInputM2Exp->Size = System::Drawing::Size(378, 20);
				 this->textBoxInputM2Exp->TabIndex = 4;
				 // 
				 // panelOutputM2I
				 // 
				 this->panelOutputM2I->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->panelOutputM2I->Controls->Add(this->buttonOutputM2IBrowse);
				 this->panelOutputM2I->Controls->Add(this->textBoxOutputM2I);
				 this->panelOutputM2I->Controls->Add(this->label7);
				 this->panelOutputM2I->Location = System::Drawing::Point(5, 38);
				 this->panelOutputM2I->Name = L"panelOutputM2I";
				 this->panelOutputM2I->Size = System::Drawing::Size(532, 26);
				 this->panelOutputM2I->TabIndex = 20;
				 // 
				 // textBoxOutputM2I
				 // 
				 this->textBoxOutputM2I->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->textBoxOutputM2I->Location = System::Drawing::Point(89, 3);
				 this->textBoxOutputM2I->Name = L"textBoxOutputM2I";
				 this->textBoxOutputM2I->Size = System::Drawing::Size(378, 20);
				 this->textBoxOutputM2I->TabIndex = 7;
				 // 
				 // tabImport
				 // 
				 this->tabImport->Controls->Add(this->checkBoxReplaceM2);
				 this->tabImport->Controls->Add(this->panelReplaceM2);
				 this->tabImport->Controls->Add(this->panel1);
				 this->tabImport->Controls->Add(this->extraworkPanel);
				 this->tabImport->Controls->Add(this->panelInputM2Import);
				 this->tabImport->Controls->Add(this->panelInputM2I);
				 this->tabImport->Location = System::Drawing::Point(4, 22);
				 this->tabImport->Name = L"tabImport";
				 this->tabImport->Padding = System::Windows::Forms::Padding(3);
				 this->tabImport->Size = System::Drawing::Size(562, 251);
				 this->tabImport->TabIndex = 1;
				 this->tabImport->Text = L"Import";
				 this->tabImport->UseVisualStyleBackColor = true;
				 // 
				 // panelReplaceM2
				 // 
				 this->panelReplaceM2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->panelReplaceM2->Controls->Add(this->textBoxReplaceM2);
				 this->panelReplaceM2->Controls->Add(this->buttonReplaceM2Browse);
				 this->panelReplaceM2->Enabled = false;
				 this->panelReplaceM2->Location = System::Drawing::Point(86, 70);
				 this->panelReplaceM2->Name = L"panelReplaceM2";
				 this->panelReplaceM2->Size = System::Drawing::Size(451, 25);
				 this->panelReplaceM2->TabIndex = 25;
				 // 
				 // panel1
				 // 
				 this->panel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->panel1->Controls->Add(this->importButtonGo);
				 this->panel1->Controls->Add(this->importButtonPreload);
				 this->panel1->Controls->Add(this->importCancelButton);
				 this->panel1->Location = System::Drawing::Point(7, 198);
				 this->panel1->Name = L"panel1";
				 this->panel1->Size = System::Drawing::Size(527, 62);
				 this->panel1->TabIndex = 31;
				 // 
				 // extraworkPanel
				 // 
				 this->extraworkPanel->Controls->Add(this->manageMeshesButton);
				 this->extraworkPanel->Enabled = false;
				 this->extraworkPanel->Location = System::Drawing::Point(7, 101);
				 this->extraworkPanel->Name = L"extraworkPanel";
				 this->extraworkPanel->Size = System::Drawing::Size(530, 91);
				 this->extraworkPanel->TabIndex = 30;
				 // 
				 // panelInputM2Import
				 // 
				 this->panelInputM2Import->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->panelInputM2Import->Controls->Add(this->textBoxInputM2Imp);
				 this->panelInputM2Import->Controls->Add(this->label5);
				 this->panelInputM2Import->Controls->Add(this->buttonInputM2ImpBrowse);
				 this->panelInputM2Import->Location = System::Drawing::Point(6, 9);
				 this->panelInputM2Import->Name = L"panelInputM2Import";
				 this->panelInputM2Import->Size = System::Drawing::Size(531, 24);
				 this->panelInputM2Import->TabIndex = 26;
				 // 
				 // panelInputM2I
				 // 
				 this->panelInputM2I->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->panelInputM2I->Controls->Add(this->label3);
				 this->panelInputM2I->Controls->Add(this->buttonInputM2IBrowse);
				 this->panelInputM2I->Controls->Add(this->textBoxInputM2I);
				 this->panelInputM2I->Location = System::Drawing::Point(6, 36);
				 this->panelInputM2I->Name = L"panelInputM2I";
				 this->panelInputM2I->Size = System::Drawing::Size(531, 30);
				 this->panelInputM2I->TabIndex = 23;
				 // 
				 // cascTabPage
				 // 
				 this->cascTabPage->Controls->Add(this->refreshButton);
				 this->cascTabPage->Controls->Add(this->fileTestButton);
				 this->cascTabPage->Controls->Add(this->testOutputTextBox);
				 this->cascTabPage->Controls->Add(this->testInputTextBox);
				 this->cascTabPage->Controls->Add(this->fileTestLabel);
				 this->cascTabPage->Controls->Add(this->loadListfileButton);
				 this->cascTabPage->Controls->Add(this->cascInfoTextBox);
				 this->cascTabPage->Controls->Add(this->cascInfoLabel);
				 this->cascTabPage->Location = System::Drawing::Point(4, 22);
				 this->cascTabPage->Name = L"cascTabPage";
				 this->cascTabPage->Padding = System::Windows::Forms::Padding(3);
				 this->cascTabPage->Size = System::Drawing::Size(562, 251);
				 this->cascTabPage->TabIndex = 3;
				 this->cascTabPage->Text = L"CASC";
				 this->cascTabPage->UseVisualStyleBackColor = true;
				 // 
				 // refreshButton
				 // 
				 this->refreshButton->Location = System::Drawing::Point(3, 2);
				 this->refreshButton->Name = L"refreshButton";
				 this->refreshButton->Size = System::Drawing::Size(75, 23);
				 this->refreshButton->TabIndex = 9;
				 this->refreshButton->Text = L"Refresh";
				 this->refreshButton->UseVisualStyleBackColor = true;
				 this->refreshButton->Click += gcnew System::EventHandler(this, &Form1::refreshButton_Click);
				 // 
				 // fileTestButton
				 // 
				 this->fileTestButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
				 this->fileTestButton->Location = System::Drawing::Point(30, 193);
				 this->fileTestButton->Name = L"fileTestButton";
				 this->fileTestButton->Size = System::Drawing::Size(75, 23);
				 this->fileTestButton->TabIndex = 8;
				 this->fileTestButton->Text = L"Test";
				 this->fileTestButton->UseVisualStyleBackColor = true;
				 this->fileTestButton->Click += gcnew System::EventHandler(this, &Form1::fileTestButton_Click);
				 // 
				 // testOutputTextBox
				 // 
				 this->testOutputTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->testOutputTextBox->Location = System::Drawing::Point(128, 195);
				 this->testOutputTextBox->Name = L"testOutputTextBox";
				 this->testOutputTextBox->Size = System::Drawing::Size(431, 20);
				 this->testOutputTextBox->TabIndex = 7;
				 // 
				 // testInputTextBox
				 // 
				 this->testInputTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->testInputTextBox->Location = System::Drawing::Point(128, 169);
				 this->testInputTextBox->Name = L"testInputTextBox";
				 this->testInputTextBox->Size = System::Drawing::Size(431, 20);
				 this->testInputTextBox->TabIndex = 6;
				 this->testInputTextBox->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::testInputTextBox_KeyDown);
				 // 
				 // fileTestLabel
				 // 
				 this->fileTestLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
				 this->fileTestLabel->AutoSize = true;
				 this->fileTestLabel->Location = System::Drawing::Point(6, 172);
				 this->fileTestLabel->Name = L"fileTestLabel";
				 this->fileTestLabel->Size = System::Drawing::Size(121, 13);
				 this->fileTestLabel->TabIndex = 5;
				 this->fileTestLabel->Text = L"Input path or FileDataId:";
				 // 
				 // loadListfileButton
				 // 
				 this->loadListfileButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
				 this->loadListfileButton->Location = System::Drawing::Point(451, 3);
				 this->loadListfileButton->Name = L"loadListfileButton";
				 this->loadListfileButton->Size = System::Drawing::Size(108, 23);
				 this->loadListfileButton->TabIndex = 3;
				 this->loadListfileButton->Text = L"Load Listfile";
				 this->loadListfileButton->UseVisualStyleBackColor = true;
				 this->loadListfileButton->Click += gcnew System::EventHandler(this, &Form1::loadCacheButton_Click);
				 // 
				 // cascInfoTextBox
				 // 
				 this->cascInfoTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
					 | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->cascInfoTextBox->Location = System::Drawing::Point(3, 29);
				 this->cascInfoTextBox->Multiline = true;
				 this->cascInfoTextBox->Name = L"cascInfoTextBox";
				 this->cascInfoTextBox->ReadOnly = true;
				 this->cascInfoTextBox->Size = System::Drawing::Size(556, 134);
				 this->cascInfoTextBox->TabIndex = 1;
				 // 
				 // cascInfoLabel
				 // 
				 this->cascInfoLabel->AutoSize = true;
				 this->cascInfoLabel->Location = System::Drawing::Point(84, 12);
				 this->cascInfoLabel->Name = L"cascInfoLabel";
				 this->cascInfoLabel->Size = System::Drawing::Size(59, 13);
				 this->cascInfoLabel->TabIndex = 0;
				 this->cascInfoLabel->Text = L"CASC Info:";
				 // 
				 // tabLog
				 // 
				 this->tabLog->Controls->Add(this->clearButton);
				 this->tabLog->Controls->Add(this->logTextBox);
				 this->tabLog->Location = System::Drawing::Point(4, 22);
				 this->tabLog->Name = L"tabLog";
				 this->tabLog->Padding = System::Windows::Forms::Padding(3);
				 this->tabLog->Size = System::Drawing::Size(562, 251);
				 this->tabLog->TabIndex = 2;
				 this->tabLog->Text = L"Log";
				 this->tabLog->UseVisualStyleBackColor = true;
				 // 
				 // clearButton
				 // 
				 this->clearButton->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
				 this->clearButton->Location = System::Drawing::Point(241, 222);
				 this->clearButton->Name = L"clearButton";
				 this->clearButton->Size = System::Drawing::Size(75, 23);
				 this->clearButton->TabIndex = 1;
				 this->clearButton->Text = L"Clear";
				 this->clearButton->UseVisualStyleBackColor = true;
				 this->clearButton->Click += gcnew System::EventHandler(this, &Form1::clearButton_Click);
				 // 
				 // logTextBox
				 // 
				 this->logTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
					 | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->logTextBox->Location = System::Drawing::Point(3, 3);
				 this->logTextBox->Multiline = true;
				 this->logTextBox->Name = L"logTextBox";
				 this->logTextBox->ReadOnly = true;
				 this->logTextBox->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
				 this->logTextBox->Size = System::Drawing::Size(556, 216);
				 this->logTextBox->TabIndex = 0;
				 // 
				 // statusStrip1
				 // 
				 this->statusStrip1->BackColor = System::Drawing::Color::Transparent;
				 this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->toolStripStatusLabel1 });
				 this->statusStrip1->Location = System::Drawing::Point(0, 298);
				 this->statusStrip1->Name = L"statusStrip1";
				 this->statusStrip1->Size = System::Drawing::Size(569, 22);
				 this->statusStrip1->TabIndex = 31;
				 this->statusStrip1->Text = L"statusStrip1";
				 // 
				 // toolStripStatusLabel1
				 // 
				 this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
				 this->toolStripStatusLabel1->Size = System::Drawing::Size(0, 17);
				 // 
				 // menuStrip1
				 // 
				 this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
					 this->fileToolStripMenuItem,
						 this->toolsToolStripMenuItem
				 });
				 this->menuStrip1->Location = System::Drawing::Point(0, 0);
				 this->menuStrip1->Name = L"menuStrip1";
				 this->menuStrip1->Size = System::Drawing::Size(569, 24);
				 this->menuStrip1->TabIndex = 32;
				 this->menuStrip1->Text = L"menuStrip1";
				 // 
				 // fileToolStripMenuItem
				 // 
				 this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
					 this->settingsToolStripMenuItem,
						 this->exitToolStripMenuItem
				 });
				 this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
				 this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
				 this->fileToolStripMenuItem->Text = L"File";
				 // 
				 // settingsToolStripMenuItem
				 // 
				 this->settingsToolStripMenuItem->Name = L"settingsToolStripMenuItem";
				 this->settingsToolStripMenuItem->Size = System::Drawing::Size(116, 22);
				 this->settingsToolStripMenuItem->Text = L"Settings";
				 this->settingsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::settingsToolStripMenuItem_Click);
				 // 
				 // exitToolStripMenuItem
				 // 
				 this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
				 this->exitToolStripMenuItem->Size = System::Drawing::Size(116, 22);
				 this->exitToolStripMenuItem->Text = L"Exit";
				 this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
				 // 
				 // toolsToolStripMenuItem
				 // 
				 this->toolsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->compareBonesToolStripMenuItem });
				 this->toolsToolStripMenuItem->Name = L"toolsToolStripMenuItem";
				 this->toolsToolStripMenuItem->Size = System::Drawing::Size(47, 20);
				 this->toolsToolStripMenuItem->Text = L"Tools";
				 // 
				 // compareBonesToolStripMenuItem
				 // 
				 this->compareBonesToolStripMenuItem->Name = L"compareBonesToolStripMenuItem";
				 this->compareBonesToolStripMenuItem->Size = System::Drawing::Size(158, 22);
				 this->compareBonesToolStripMenuItem->Text = L"Compare bones";
				 this->compareBonesToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::compareBonesToolStripMenuItem_Click);
				 // 
				 // Form1
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->ClientSize = System::Drawing::Size(569, 320);
				 this->Controls->Add(this->statusStrip1);
				 this->Controls->Add(this->menuStrip1);
				 this->Controls->Add(this->tabControl);
				 this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
				 this->MainMenuStrip = this->menuStrip1;
				 this->MinimumSize = System::Drawing::Size(500, 320);
				 this->Name = L"Form1";
				 this->Text = L"M2Mod Redux";
				 this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
				 this->tabControl->ResumeLayout(false);
				 this->tabExport->ResumeLayout(false);
				 this->panelImputM2Exp->ResumeLayout(false);
				 this->panelImputM2Exp->PerformLayout();
				 this->panelOutputM2I->ResumeLayout(false);
				 this->panelOutputM2I->PerformLayout();
				 this->tabImport->ResumeLayout(false);
				 this->tabImport->PerformLayout();
				 this->panelReplaceM2->ResumeLayout(false);
				 this->panelReplaceM2->PerformLayout();
				 this->panel1->ResumeLayout(false);
				 this->extraworkPanel->ResumeLayout(false);
				 this->panelInputM2Import->ResumeLayout(false);
				 this->panelInputM2Import->PerformLayout();
				 this->panelInputM2I->ResumeLayout(false);
				 this->panelInputM2I->PerformLayout();
				 this->cascTabPage->ResumeLayout(false);
				 this->cascTabPage->PerformLayout();
				 this->tabLog->ResumeLayout(false);
				 this->tabLog->PerformLayout();
				 this->statusStrip1->ResumeLayout(false);
				 this->statusStrip1->PerformLayout();
				 this->menuStrip1->ResumeLayout(false);
				 this->menuStrip1->PerformLayout();
				 this->ResumeLayout(false);
				 this->PerformLayout();

			 }
#pragma endregion

	private:
		System::Void buttonInputM2Browse_Click(System::Object^  sender, System::EventArgs^  e)
		{
			openFileDialog1->Filter = M2Filter;
			try
			{
				openFileDialog1->FileName = textBoxInputM2Exp->Text;
				openFileDialog1->InitialDirectory = Path::GetDirectoryName(textBoxInputM2Exp->Text);
			}
			catch (...) {}

			if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				textBoxInputM2Exp->Text = openFileDialog1->FileName;
				textBoxInputM2Imp->Text = openFileDialog1->FileName;
				textBoxOutputM2I->Text = Path::ChangeExtension(openFileDialog1->FileName, "m2i");
				textBoxInputM2I->Text = Path::ChangeExtension(openFileDialog1->FileName, "m2i");
			}
		}

	private:
		System::Void buttonOutputM2IBrowse_Click(System::Object^  sender, System::EventArgs^  e)
		{
			saveFileDialog1->Filter = M2IFilter;
			try
			{
				saveFileDialog1->FileName = textBoxOutputM2I->Text;
				saveFileDialog1->InitialDirectory = System::IO::Path::GetDirectoryName(textBoxOutputM2I->Text);
			}
			catch (...) {}
			if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				textBoxOutputM2I->Text = saveFileDialog1->FileName;
			}
		}

	private:
		System::Void buttonInputM2IBrowse_Click(System::Object^  sender, System::EventArgs^  e)
		{
			openFileDialog1->Filter = M2IFilter;
			try
			{
				openFileDialog1->FileName = textBoxInputM2I->Text;
				openFileDialog1->InitialDirectory = System::IO::Path::GetDirectoryName(textBoxInputM2I->Text);
			}
			catch (...) {}
			if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				textBoxInputM2I->Text = openFileDialog1->FileName;
			}
		}

	private:
		System::Void SetStatus(String^ Status)
		{
			toolStripStatusLabel1->Text = Status;
			statusStrip1->Refresh();
		}

	private:
		System::Void buttonGo_Click(System::Object^  sender, System::EventArgs^  e)
		{
			exportButtonGo->Enabled = false;
			exportButtonGo->Refresh();
			SetStatus("Working...");

			// Check fields.
			if (textBoxInputM2Exp->Text->Length == 0)
			{
				SetStatus("Error: No input M2 file Specified.");
				exportButtonGo->Enabled = true;
				return;
			}

			if (textBoxOutputM2I->Text->Length == 0)
			{
				SetStatus("Error: No output M2I file Specified.");
				exportButtonGo->Enabled = true;
				return;
			}

			M2Lib::M2 M2(settings);
			M2.SetCasc(GetCasc());

			// import M2
			M2Lib::EError Error = M2.Load(StringConverter(textBoxInputM2Exp->Text).ToStringW());
			if (Error != M2Lib::EError_OK)
			{
				SetStatus(gcnew String(M2Lib::GetErrorText(Error)));
				exportButtonGo->Enabled = true;
				return;
			}

			// export M2I
			Error = M2.ExportM2Intermediate(StringConverter(textBoxOutputM2I->Text).ToStringW());
			if (Error != M2Lib::EError_OK)
			{
				SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
				exportButtonGo->Enabled = true;
				return;
			}

			SetStatus("Export done.");

			exportButtonGo->Enabled = true;
		}


	private: void PreloadTransition(bool On)
	{
		if (On)
		{
			panelInputM2Import->Enabled = false;
			panelInputM2I->Enabled = false;

			importButtonPreload->Enabled = false;
			importButtonGo->Enabled = true;
			extraworkPanel->Enabled = true;
			importCancelButton->Enabled = true;
			checkBoxReplaceM2->Enabled = true;
			panelReplaceM2->Enabled = checkBoxReplaceM2->Checked;
		}
		else
		{
			panelInputM2Import->Enabled = true;
			panelInputM2I->Enabled = true;

			importButtonPreload->Enabled = true;
			importButtonGo->Enabled = false;
			extraworkPanel->Enabled = false;
			importCancelButton->Enabled = false;

			checkBoxReplaceM2->Enabled = false;
			panelReplaceM2->Enabled = false;

			if (preloadM2)
			{
				delete preloadM2;
				preloadM2 = NULL;
			}

			MeshManagementForm = nullptr;
		}
	}

	private: M2Lib::M2* preloadM2 = NULL;
	private: M2Lib::GlobalSettings* settings = NULL;


	private: M2Lib::Casc* _casc = NULL;
	private: M2Lib::Casc* GetCasc()
	{
		if (!_casc)
		{
			_casc = new M2Lib::Casc();

			static bool pathNotified = false;
			if (settings->WowPath.empty() && !pathNotified)
			{
				if (!pathNotified)
				{
					pathNotified = true;

					auto result = MessageBox::Show(L"Wow path not set. Would you like to specify it?", L"Warning", MessageBoxButtons::YesNo, MessageBoxIcon::Warning);
					if (result == System::Windows::Forms::DialogResult::Yes)
						settingsToolStripMenuItem_Click(this, EventArgs::Empty);
				}
			}

			_casc->SetStoragePath(settings->WowPath);
			_casc->SetReleaseOnDereference(settings->UnloadCascOnDereference);

			static bool listfileNotified = false;
			if (!listfileNotified && !File::Exists(gcnew String(M2Lib::Casc::DefaultListfilePath.c_str())) &&
				!File::Exists(gcnew String(M2Lib::Casc::DefaultBinListfilePath.c_str())))
			{
				listfileNotified = true;
				auto result = MessageBox::Show(L"Wow listfile not found. Would you like to specify it?", L"Warning", MessageBoxButtons::YesNo, MessageBoxIcon::Warning);
				if (result == System::Windows::Forms::DialogResult::Yes)
				{
					auto dialog = gcnew OpenFileDialog();
					dialog->InitialDirectory = gcnew String(M2Lib::FileSystemA::GetCurrentPath().c_str());
					dialog->FileName = L"listfile.txt";
					dialog->Filter = L"WoW Listfile|*.txt";
					auto result = dialog->ShowDialog();
					if (result == System::Windows::Forms::DialogResult::OK)
						_casc->GenerateListFileCache(StringConverter(dialog->FileName).ToStringA());;
				}
			}
		}

		return _casc;
	}

	private: delegate void LoggerDelegate(char const*);

	private: static void Log(char const* data)
	{
		if (Instance)
			Instance->_Log(data);
	}

	private: void _Log(char const* data)
	{
		if (logTextBox->Text->Length)
			logTextBox->Text += "\r\n" + gcnew String(data);
		else
			logTextBox->Text = gcnew String(data);
	}

	private: System::Void importButtonGo_Click(System::Object^  sender, System::EventArgs^  e) {
		importButtonPreload->Enabled = false;
		importButtonPreload->Refresh();
		SetStatus("Importing...");

		if (!preloadM2)
		{
			SetStatus("Error: Model not preloaded");
			PreloadTransition(false);
			return;
		}

		if (GetOrCreateMeshManagementForm()->DialogResult == System::Windows::Forms::DialogResult::OK ||
			GetOrCreateMeshManagementForm()->DialogResult == System::Windows::Forms::DialogResult::None)
		{
			auto MainSkin = preloadM2->Skins[0];

			auto transparentRenderFlag = -1;
			// first assing textures and gloss
			for (unsigned int i = 0; i < MeshManagementForm->ElementInfoList.Count; ++i)
			{
				auto Control = MeshManagementForm->ElementInfoList[i];

				if (Control->copyMaterialCheckBox->Checked && Control->srcMaterialComboBox->SelectedIndex > 0)
					const_cast<M2Lib::SubmeshExtraData*>(MainSkin->ExtraDataBySubmeshIndex[i])->MaterialOverride = Control->srcMaterialComboBox->SelectedIndex - 1;

				if (Control->customTextureCheckBox->Checked && Control->customTextureTextBox->Text->Length)
					const_cast<M2Lib::SubmeshExtraData*>(MainSkin->ExtraDataBySubmeshIndex[i])->CustomTextureName = StringConverter(Control->customTextureTextBox->Text).ToStringA();

				if (Control->makeGlossyCheckBox->Checked && Control->glossTextureTextBox->Text->Length)
					const_cast<M2Lib::SubmeshExtraData*>(MainSkin->ExtraDataBySubmeshIndex[i])->GlossTextureName = StringConverter(Control->glossTextureTextBox->Text).ToStringA();
			}
		}

		M2Lib::M2* ReplaceM2 = NULL;
		if (checkBoxReplaceM2->Checked)
		{
			ReplaceM2 = new M2Lib::M2();
			auto Error = ReplaceM2->Load(StringConverter(textBoxReplaceM2->Text).ToStringW());
			if (Error != M2Lib::EError_OK)
			{
				SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
				delete ReplaceM2;

				delete preloadM2;
				preloadM2 = NULL;
				PreloadTransition(false);
				return;
			}
		}

		auto outputDirectory = Path::Combine(Path::GetDirectoryName(textBoxInputM2Exp->Text), "Export");
		if (!Directory::Exists(outputDirectory))
			Directory::CreateDirectory(outputDirectory);
		
		auto ExportFileName = Path::Combine(outputDirectory, Path::GetFileName(checkBoxReplaceM2->Checked ? textBoxReplaceM2->Text : textBoxInputM2Exp->Text));

		// export M2
		auto Error = preloadM2->Save(StringConverter(ExportFileName).ToStringW(), ReplaceM2);
		if (Error != M2Lib::EError_OK)
		{
			SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
			delete preloadM2;
			delete ReplaceM2;
			preloadM2 = NULL;
			PreloadTransition(false);
			return;
		}

		delete preloadM2;
		delete ReplaceM2;
		preloadM2 = NULL;

		SetStatus("Import done.");
		PreloadTransition(false);
	}

	private: System::Void importButtonPreload_Click(System::Object^  sender, System::EventArgs^  e) {
		importButtonPreload->Enabled = false;
		importButtonPreload->Refresh();
		SetStatus("Preloading...");

		if (preloadM2)
			delete preloadM2;

		// Check fields.
		if (textBoxInputM2I->Text->Length == 0)
		{
			SetStatus("Error: No input M2I file Specified.");
			PreloadTransition(false);
			return;
		}

		// import M2
		if (textBoxInputM2Imp->Text->Length == 0)
		{
			SetStatus("Error: No input M2 file Specified.");
			PreloadTransition(false);
			return;
		}

		preloadM2 = new M2Lib::M2(settings);
		preloadM2->SetCasc(GetCasc());

		M2Lib::EError Error = preloadM2->Load(StringConverter(textBoxInputM2Imp->Text).ToStringW());
		if (Error != M2Lib::EError_OK)
		{
			SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
			delete preloadM2;
			preloadM2 = NULL;
			PreloadTransition(false);
			return;
		}

		// import M2I
		Error = preloadM2->ImportM2Intermediate(StringConverter(textBoxInputM2I->Text).ToStringW());
		if (Error != M2Lib::EError_OK)
		{
			SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
			delete preloadM2;
			preloadM2 = NULL;
			PreloadTransition(false);
			return;
		}

		SetStatus("Preload finished.");

		PreloadTransition(true);
	}

	private: System::Void importCancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
		PreloadTransition(false);
		if (preloadM2)
		{
			delete preloadM2;
			preloadM2 = NULL;
		}

		SetStatus("Cancelled preload.");
	}

	private: ElementManagementForm<MeshInfoControl>^ MeshManagementForm = nullptr;


	private: ElementManagementForm<MeshInfoControl>^ GetOrCreateMeshManagementForm()
	{
		if (MeshManagementForm)
			return MeshManagementForm;

		auto MeshInfos = preloadM2->Skins[0]->GetMeshInfo();

		MeshManagementForm = gcnew ElementManagementForm<MeshInfoControl>();
		MeshManagementForm->Text = "Mesh Management";

		for (unsigned int i = 0; i < MeshInfos.size(); ++i)
		{
			auto Control = gcnew MeshInfoControl();
			Control->Initialize(preloadM2, i, MeshInfos);
			MeshManagementForm->AddElementInfo(Control);
		}

		MeshManagementForm->SetupInfoPositions();

		return MeshManagementForm;
	}

	private: System::Void manageMeshesButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		auto result = GetOrCreateMeshManagementForm()->ShowDialog();
		if (result != Windows::Forms::DialogResult::OK)
			MeshManagementForm = nullptr;
	}
	private: System::Void checkBoxReplaceM2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		panelReplaceM2->Enabled = checkBoxReplaceM2->Checked;
	}
	private: System::Void buttonReplaceM2Browse_Click(System::Object^  sender, System::EventArgs^  e) {
		openFileDialog1->Filter = M2Filter;
		try
		{
			openFileDialog1->FileName = textBoxReplaceM2->Text;
			openFileDialog1->InitialDirectory = System::IO::Path::GetDirectoryName(textBoxReplaceM2->Text);
		}
		catch (...) {}

		if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			textBoxReplaceM2->Text = openFileDialog1->FileName;
		}
	}

	private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		Close();
	}

	private: System::Void settingsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		auto form = gcnew SettingsForm();
		form->Setup(settings);
		if (form->ShowDialog() == Windows::Forms::DialogResult::OK)
		{
			*settings = form->ProduceSettings();
			if (auto casc = GetCasc())
			{
				casc->SetReleaseOnDereference(settings->UnloadCascOnDereference);
				casc->SetStoragePath(settings->WowPath);
				AnalyzeCasc();
			}
		}
	}
	private: System::Void compareBonesToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		auto form = gcnew CompareBonesForm();
		form->ShowDialog();
	}
	private: System::Void clearButton_Click(System::Object^  sender, System::EventArgs^  e) {
		logTextBox->Text = "";
	}
	private: System::Void loadCacheButton_Click(System::Object^  sender, System::EventArgs^  e) {
		
		auto dialog = gcnew OpenFileDialog();
		dialog->InitialDirectory = gcnew String(M2Lib::FileSystemA::GetCurrentPath().c_str());
		dialog->FileName = L"listfile.txt";
		dialog->Filter = L"WoW Listfile|*.txt";
		auto result = dialog->ShowDialog();
		if (result != System::Windows::Forms::DialogResult::OK)
			return;

		if (auto casc = GetCasc())
			casc->GenerateListFileCache(StringConverter(dialog->FileName).ToStringA());

		AnalyzeCasc();
	}
	private: System::Void fileTestButton_Click(System::Object^  sender, System::EventArgs^  e) {
		TestFiles();
	}
	private: System::Void testInputTextBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
		
	}
	private: System::Void testInputTextBox_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		if (e->KeyCode != Keys::Enter)
			return;

		TestFiles();
	}
	private: System::Void refreshButton_Click(System::Object^  sender, System::EventArgs^  e) {
		AnalyzeCasc();
	}
};
}
