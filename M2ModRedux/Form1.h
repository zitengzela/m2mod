#pragma once

#define M2Filter L"M2 Files|*.m2|All Files|*.*"
#define M2IFilter L"M2I Files|*.m2i|All Files|*.*"

#include "ElementManagementForm.h"
#include "MeshInfoControl.h"
#include "RegistryStore.h"
#include "Version.h"
#include "Updater.h"

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

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			
			this->Text = String::Format(L"M2Mod Redux {0}.{1}.{2}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

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
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::ImportOutM2))
					this->textBoxOutputM2->Text = (String^)value;
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::ImportReplaceM2))
					this->textBoxReplaceM2->Text = (String^)value;

				if (auto value = RegistyStore::GetValue(RegistyStore::Value::MergeAttachments))
					this->checkBoxMergeAttachments->Checked = Boolean::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::MergeBones))
					this->checkBoxMergeBones->Checked = Boolean::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::MergeCameras))
					this->checkBoxMergeCameras->Checked = Boolean::Parse(value->ToString());
				if (auto value = RegistyStore::GetValue(RegistyStore::Value::FixSeams))
					this->checkBoxFixSeams->Checked = Boolean::Parse(value->ToString());
			}
			catch (...)
			{

			}

			auto updater = gcnew Updater();
			auto func = gcnew Func<int>(updater, &Updater::Update);
			Task::Run(func);
		}

	private: System::Void Form1_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {

		RegistyStore::SetValue(RegistyStore::Value::ExportM2, textBoxInputM2Exp->Text);

		RegistyStore::SetValue(RegistyStore::Value::ExportM2I, this->textBoxOutputM2I->Text);

		RegistyStore::SetValue(RegistyStore::Value::ImportInM2, this->textBoxInputM2Imp->Text);
		RegistyStore::SetValue(RegistyStore::Value::ImportM2I, this->textBoxInputM2I->Text);
		RegistyStore::SetValue(RegistyStore::Value::ImportOutM2, this->textBoxOutputM2->Text);
		RegistyStore::SetValue(RegistyStore::Value::ImportReplaceM2, this->textBoxReplaceM2->Text);

		RegistyStore::SetValue(RegistyStore::Value::MergeAttachments, this->checkBoxMergeAttachments->Checked);
		RegistyStore::SetValue(RegistyStore::Value::MergeBones, this->checkBoxMergeBones->Checked);
		RegistyStore::SetValue(RegistyStore::Value::MergeCameras, this->checkBoxMergeCameras->Checked);
		RegistyStore::SetValue(RegistyStore::Value::FixSeams, this->checkBoxFixSeams->Checked);

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
		}

	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::TabControl^  tabControl1;
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
	private: System::Windows::Forms::Panel^  panelOutputM2;
	private: System::Windows::Forms::TextBox^  textBoxOutputM2;
	private: System::Windows::Forms::Button^  buttonOutputM2Browse;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Panel^  panelImportCb;
	private: System::Windows::Forms::CheckBox^  checkBoxMergeCameras;
	private: System::Windows::Forms::CheckBox^  checkBoxMergeAttachments;
	private: System::Windows::Forms::CheckBox^  checkBoxMergeBones;
	private: System::Windows::Forms::Button^  exportButtonGo;

	private: System::Windows::Forms::CheckBox^  checkBoxFixSeams;

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
				 this->buttonOutputM2Browse = (gcnew System::Windows::Forms::Button());
				 this->label2 = (gcnew System::Windows::Forms::Label());
				 this->checkBoxMergeCameras = (gcnew System::Windows::Forms::CheckBox());
				 this->checkBoxMergeAttachments = (gcnew System::Windows::Forms::CheckBox());
				 this->checkBoxMergeBones = (gcnew System::Windows::Forms::CheckBox());
				 this->exportButtonGo = (gcnew System::Windows::Forms::Button());
				 this->label5 = (gcnew System::Windows::Forms::Label());
				 this->buttonInputM2ImpBrowse = (gcnew System::Windows::Forms::Button());
				 this->importButtonGo = (gcnew System::Windows::Forms::Button());
				 this->importButtonPreload = (gcnew System::Windows::Forms::Button());
				 this->importCancelButton = (gcnew System::Windows::Forms::Button());
				 this->manageMeshesButton = (gcnew System::Windows::Forms::Button());
				 this->textBoxInputM2Imp = (gcnew System::Windows::Forms::TextBox());
				 this->textBoxInputM2I = (gcnew System::Windows::Forms::TextBox());
				 this->textBoxOutputM2 = (gcnew System::Windows::Forms::TextBox());
				 this->textBoxReplaceM2 = (gcnew System::Windows::Forms::TextBox());
				 this->buttonReplaceM2Browse = (gcnew System::Windows::Forms::Button());
				 this->checkBoxReplaceM2 = (gcnew System::Windows::Forms::CheckBox());
				 this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
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
				 this->panelOutputM2 = (gcnew System::Windows::Forms::Panel());
				 this->panelImportCb = (gcnew System::Windows::Forms::Panel());
				 this->checkBoxFixSeams = (gcnew System::Windows::Forms::CheckBox());
				 this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
				 this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
				 this->tabControl1->SuspendLayout();
				 this->tabExport->SuspendLayout();
				 this->panelImputM2Exp->SuspendLayout();
				 this->panelOutputM2I->SuspendLayout();
				 this->tabImport->SuspendLayout();
				 this->panelReplaceM2->SuspendLayout();
				 this->panel1->SuspendLayout();
				 this->extraworkPanel->SuspendLayout();
				 this->panelInputM2Import->SuspendLayout();
				 this->panelInputM2I->SuspendLayout();
				 this->panelOutputM2->SuspendLayout();
				 this->panelImportCb->SuspendLayout();
				 this->statusStrip1->SuspendLayout();
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
				 // buttonOutputM2Browse
				 // 
				 this->buttonOutputM2Browse->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
				 this->buttonOutputM2Browse->Location = System::Drawing::Point(472, 2);
				 this->buttonOutputM2Browse->Name = L"buttonOutputM2Browse";
				 this->buttonOutputM2Browse->Size = System::Drawing::Size(55, 20);
				 this->buttonOutputM2Browse->TabIndex = 14;
				 this->buttonOutputM2Browse->Text = L"...";
				 this->toolTip1->SetToolTip(this->buttonOutputM2Browse, L"Browse...");
				 this->buttonOutputM2Browse->UseVisualStyleBackColor = true;
				 this->buttonOutputM2Browse->Click += gcnew System::EventHandler(this, &Form1::buttonOutputM2Browse_Click);
				 // 
				 // label2
				 // 
				 this->label2->AutoSize = true;
				 this->label2->Location = System::Drawing::Point(12, 5);
				 this->label2->Name = L"label2";
				 this->label2->Size = System::Drawing::Size(54, 13);
				 this->label2->TabIndex = 12;
				 this->label2->Text = L"OutputM2";
				 this->toolTip1->SetToolTip(this->label2, L"Optional. If set, this is where M2Mod will save the modified M2.");
				 // 
				 // checkBoxMergeCameras
				 // 
				 this->checkBoxMergeCameras->AutoSize = true;
				 this->checkBoxMergeCameras->Checked = true;
				 this->checkBoxMergeCameras->CheckState = System::Windows::Forms::CheckState::Checked;
				 this->checkBoxMergeCameras->Location = System::Drawing::Point(217, 0);
				 this->checkBoxMergeCameras->Name = L"checkBoxMergeCameras";
				 this->checkBoxMergeCameras->Size = System::Drawing::Size(97, 17);
				 this->checkBoxMergeCameras->TabIndex = 17;
				 this->checkBoxMergeCameras->Text = L"MergeCameras";
				 this->toolTip1->SetToolTip(this->checkBoxMergeCameras, L"Check to load cameras from M2I");
				 this->checkBoxMergeCameras->UseVisualStyleBackColor = true;
				 // 
				 // checkBoxMergeAttachments
				 // 
				 this->checkBoxMergeAttachments->AutoSize = true;
				 this->checkBoxMergeAttachments->Checked = true;
				 this->checkBoxMergeAttachments->CheckState = System::Windows::Forms::CheckState::Checked;
				 this->checkBoxMergeAttachments->Location = System::Drawing::Point(95, 0);
				 this->checkBoxMergeAttachments->Name = L"checkBoxMergeAttachments";
				 this->checkBoxMergeAttachments->Size = System::Drawing::Size(115, 17);
				 this->checkBoxMergeAttachments->TabIndex = 16;
				 this->checkBoxMergeAttachments->Text = L"MergeAttachments";
				 this->toolTip1->SetToolTip(this->checkBoxMergeAttachments, L"Check to load attachments from M2I");
				 this->checkBoxMergeAttachments->UseVisualStyleBackColor = true;
				 // 
				 // checkBoxMergeBones
				 // 
				 this->checkBoxMergeBones->AutoSize = true;
				 this->checkBoxMergeBones->Checked = true;
				 this->checkBoxMergeBones->CheckState = System::Windows::Forms::CheckState::Checked;
				 this->checkBoxMergeBones->Location = System::Drawing::Point(2, 0);
				 this->checkBoxMergeBones->Name = L"checkBoxMergeBones";
				 this->checkBoxMergeBones->Size = System::Drawing::Size(86, 17);
				 this->checkBoxMergeBones->TabIndex = 15;
				 this->checkBoxMergeBones->Text = L"MergeBones";
				 this->toolTip1->SetToolTip(this->checkBoxMergeBones, L"Check to load bones from M2I");
				 this->checkBoxMergeBones->UseVisualStyleBackColor = true;
				 // 
				 // exportButtonGo
				 // 
				 this->exportButtonGo->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
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
				 this->importButtonGo->Anchor = System::Windows::Forms::AnchorStyles::Top;
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
				 this->importCancelButton->Anchor = System::Windows::Forms::AnchorStyles::Right;
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
				 // textBoxOutputM2
				 // 
				 this->textBoxOutputM2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->textBoxOutputM2->Location = System::Drawing::Point(88, 2);
				 this->textBoxOutputM2->Name = L"textBoxOutputM2";
				 this->textBoxOutputM2->Size = System::Drawing::Size(378, 20);
				 this->textBoxOutputM2->TabIndex = 13;
				 this->toolTip1->SetToolTip(this->textBoxOutputM2, L"Destination path for M2");
				 // 
				 // textBoxReplaceM2
				 // 
				 this->textBoxReplaceM2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->textBoxReplaceM2->Location = System::Drawing::Point(8, 2);
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
				 this->checkBoxReplaceM2->Location = System::Drawing::Point(8, 129);
				 this->checkBoxReplaceM2->Name = L"checkBoxReplaceM2";
				 this->checkBoxReplaceM2->Size = System::Drawing::Size(81, 17);
				 this->checkBoxReplaceM2->TabIndex = 15;
				 this->checkBoxReplaceM2->Text = L"ReplaceM2";
				 this->toolTip1->SetToolTip(this->checkBoxReplaceM2, L"If you are swapping models, select target M2 that you would like to swap from.");
				 this->checkBoxReplaceM2->UseVisualStyleBackColor = true;
				 this->checkBoxReplaceM2->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBoxReplaceM2_CheckedChanged);
				 // 
				 // tabControl1
				 // 
				 this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->tabControl1->Controls->Add(this->tabExport);
				 this->tabControl1->Controls->Add(this->tabImport);
				 this->tabControl1->Location = System::Drawing::Point(1, 2);
				 this->tabControl1->Name = L"tabControl1";
				 this->tabControl1->Padding = System::Drawing::Point(10, 3);
				 this->tabControl1->SelectedIndex = 0;
				 this->tabControl1->Size = System::Drawing::Size(570, 309);
				 this->tabControl1->TabIndex = 25;
				 // 
				 // tabExport
				 // 
				 this->tabExport->Controls->Add(this->panelImputM2Exp);
				 this->tabExport->Controls->Add(this->panelOutputM2I);
				 this->tabExport->Controls->Add(this->exportButtonGo);
				 this->tabExport->Location = System::Drawing::Point(4, 22);
				 this->tabExport->Name = L"tabExport";
				 this->tabExport->Padding = System::Windows::Forms::Padding(3);
				 this->tabExport->Size = System::Drawing::Size(562, 283);
				 this->tabExport->TabIndex = 0;
				 this->tabExport->Text = L"Export";
				 this->tabExport->UseVisualStyleBackColor = true;
				 // 
				 // panelImputM2Exp
				 // 
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
				 this->tabImport->Controls->Add(this->panelOutputM2);
				 this->tabImport->Controls->Add(this->panelImportCb);
				 this->tabImport->Location = System::Drawing::Point(4, 22);
				 this->tabImport->Name = L"tabImport";
				 this->tabImport->Padding = System::Windows::Forms::Padding(3);
				 this->tabImport->Size = System::Drawing::Size(562, 283);
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
				 this->panelReplaceM2->Location = System::Drawing::Point(86, 124);
				 this->panelReplaceM2->Name = L"panelReplaceM2";
				 this->panelReplaceM2->Size = System::Drawing::Size(451, 25);
				 this->panelReplaceM2->TabIndex = 25;
				 // 
				 // panel1
				 // 
				 this->panel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
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
				 this->extraworkPanel->Location = System::Drawing::Point(6, 156);
				 this->extraworkPanel->Name = L"extraworkPanel";
				 this->extraworkPanel->Size = System::Drawing::Size(528, 36);
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
				 // panelOutputM2
				 // 
				 this->panelOutputM2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->panelOutputM2->Controls->Add(this->textBoxOutputM2);
				 this->panelOutputM2->Controls->Add(this->buttonOutputM2Browse);
				 this->panelOutputM2->Controls->Add(this->label2);
				 this->panelOutputM2->Enabled = false;
				 this->panelOutputM2->Location = System::Drawing::Point(6, 96);
				 this->panelOutputM2->Name = L"panelOutputM2";
				 this->panelOutputM2->Size = System::Drawing::Size(531, 25);
				 this->panelOutputM2->TabIndex = 24;
				 // 
				 // panelImportCb
				 // 
				 this->panelImportCb->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
					 | System::Windows::Forms::AnchorStyles::Right));
				 this->panelImportCb->Controls->Add(this->checkBoxFixSeams);
				 this->panelImportCb->Controls->Add(this->checkBoxMergeCameras);
				 this->panelImportCb->Controls->Add(this->checkBoxMergeAttachments);
				 this->panelImportCb->Controls->Add(this->checkBoxMergeBones);
				 this->panelImportCb->Location = System::Drawing::Point(6, 70);
				 this->panelImportCb->Name = L"panelImportCb";
				 this->panelImportCb->Size = System::Drawing::Size(531, 22);
				 this->panelImportCb->TabIndex = 25;
				 // 
				 // checkBoxFixSeams
				 // 
				 this->checkBoxFixSeams->AutoSize = true;
				 this->checkBoxFixSeams->Location = System::Drawing::Point(320, 0);
				 this->checkBoxFixSeams->Name = L"checkBoxFixSeams";
				 this->checkBoxFixSeams->Size = System::Drawing::Size(74, 17);
				 this->checkBoxFixSeams->TabIndex = 18;
				 this->checkBoxFixSeams->Text = L"Fix Seams";
				 this->checkBoxFixSeams->UseVisualStyleBackColor = true;
				 // 
				 // statusStrip1
				 // 
				 this->statusStrip1->BackColor = System::Drawing::Color::Transparent;
				 this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->toolStripStatusLabel1 });
				 this->statusStrip1->Location = System::Drawing::Point(0, 287);
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
				 // Form1
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->ClientSize = System::Drawing::Size(569, 309);
				 this->Controls->Add(this->statusStrip1);
				 this->Controls->Add(this->tabControl1);
				 this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
				 this->MinimumSize = System::Drawing::Size(500, 320);
				 this->Name = L"Form1";
				 this->Text = L"M2Mod Redux";
				 this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
				 this->tabControl1->ResumeLayout(false);
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
				 this->panelOutputM2->ResumeLayout(false);
				 this->panelOutputM2->PerformLayout();
				 this->panelImportCb->ResumeLayout(false);
				 this->panelImportCb->PerformLayout();
				 this->statusStrip1->ResumeLayout(false);
				 this->statusStrip1->PerformLayout();
				 this->ResumeLayout(false);
				 this->PerformLayout();

			 }
#pragma endregion

	private:
		System::Void buttonInputM2Browse_Click(System::Object^  sender, System::EventArgs^  e)
		{
			openFileDialog1->Filter = M2Filter;
			openFileDialog1->FileName = textBoxInputM2Exp->Text;
			openFileDialog1->FileName = textBoxInputM2Imp->Text;

			if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				textBoxInputM2Exp->Text = openFileDialog1->FileName;
				textBoxInputM2Imp->Text = openFileDialog1->FileName;
				textBoxOutputM2I->Text = openFileDialog1->FileName + "i";
				textBoxInputM2I->Text = openFileDialog1->FileName + "i";
				textBoxOutputM2->Text = textBoxInputM2Exp->Text->Substring(0, textBoxInputM2Exp->Text->Length - 3) + "2.m2";
			}
		}

	private:
		System::Void buttonOutputM2IBrowse_Click(System::Object^  sender, System::EventArgs^  e)
		{
			saveFileDialog1->Filter = M2IFilter;
			saveFileDialog1->FileName = textBoxOutputM2I->Text;
			if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				textBoxOutputM2I->Text = saveFileDialog1->FileName;
			}
		}

	private:
		System::Void buttonInputM2IBrowse_Click(System::Object^  sender, System::EventArgs^  e)
		{
			openFileDialog1->Filter = M2IFilter;
			openFileDialog1->FileName = textBoxInputM2I->Text;
			if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				textBoxInputM2I->Text = openFileDialog1->FileName;
			}
		}

	private:
		System::Void buttonOutputM2Browse_Click(System::Object^  sender, System::EventArgs^  e)
		{
			saveFileDialog1->Filter = M2Filter;
			saveFileDialog1->FileName = textBoxOutputM2->Text;
			if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				textBoxOutputM2->Text = saveFileDialog1->FileName;
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

			M2Lib::M2* M2 = new M2Lib::M2();

			// import M2
			System::IntPtr StringPointer = Marshal::StringToHGlobalUni(textBoxInputM2Exp->Text);
			M2Lib::EError Error = M2->Load((Char16*)StringPointer.ToPointer());
			Marshal::FreeHGlobal(StringPointer);

			if (Error != 0)
			{
				SetStatus(gcnew String(M2Lib::GetErrorText(Error)));
				exportButtonGo->Enabled = true;
				delete M2;
				return;
			}

			// export M2I
			StringPointer = Marshal::StringToHGlobalUni(textBoxOutputM2I->Text);
			Error = M2->ExportM2Intermediate((Char16*)StringPointer.ToPointer());
			Marshal::FreeHGlobal(StringPointer);

			if (Error != 0)
			{
				SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
				exportButtonGo->Enabled = true;
				delete M2;
				return;
			}

			SetStatus("Export done.");

			delete M2;

			exportButtonGo->Enabled = true;
		}


	private: void PreloadTransition(bool On)
	{
		if (On)
		{
			panelInputM2Import->Enabled = false;
			panelInputM2I->Enabled = false;
			panelImportCb->Enabled = false;
			panelOutputM2->Enabled = true;

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
			panelImportCb->Enabled = true;
			panelOutputM2->Enabled = false;

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
				{
					auto StringPointer = Marshal::StringToHGlobalAnsi(Control->customTextureTextBox->Text);
					const_cast<M2Lib::SubmeshExtraData*>(MainSkin->ExtraDataBySubmeshIndex[i])->CustomTextureName = (char const*)StringPointer.ToPointer();
					Marshal::FreeHGlobal(StringPointer);
				}

				if (Control->makeGlossyCheckBox->Checked && Control->glossTextureTextBox->Text->Length)
				{
					auto StringPointer = Marshal::StringToHGlobalAnsi(Control->glossTextureTextBox->Text);
					const_cast<M2Lib::SubmeshExtraData*>(MainSkin->ExtraDataBySubmeshIndex[i])->GlossTextureName = (char const*)StringPointer.ToPointer();
					Marshal::FreeHGlobal(StringPointer);
				}
			}
		}

		if (checkBoxReplaceM2->Checked)
		{
			M2Lib::M2 ReplaceM2;
			auto StringPointer = Marshal::StringToHGlobalUni(textBoxReplaceM2->Text);
			auto Error = ReplaceM2.Load((Char16*)StringPointer.ToPointer());
			Marshal::FreeHGlobal(StringPointer);
			if (Error)
			{
				SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
				delete preloadM2;
				preloadM2 = NULL;
				PreloadTransition(false);
				return;
			}

			preloadM2->CopySFIDChunk(&ReplaceM2);
		}

		// export M2
		auto StringPointer = Marshal::StringToHGlobalUni(textBoxOutputM2->Text);
		auto Error = preloadM2->Save((Char16*)StringPointer.ToPointer());
		Marshal::FreeHGlobal(StringPointer);

		if (Error != 0)
		{
			SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
			delete preloadM2;
			preloadM2 = NULL;
			PreloadTransition(false);
			return;
		}

		delete preloadM2;
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

		if (textBoxOutputM2->Text->Length == 0)
		{
			SetStatus("Error: No output M2 file Specified.");
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

		preloadM2 = new M2Lib::M2();

		System::IntPtr StringPointer = Marshal::StringToHGlobalUni(textBoxInputM2Imp->Text);
		M2Lib::EError Error = preloadM2->Load((Char16*)StringPointer.ToPointer());
		Marshal::FreeHGlobal(StringPointer);

		if (Error != 0)
		{
			SetStatus(gcnew System::String(M2Lib::GetErrorText(Error)));
			delete preloadM2;
			preloadM2 = NULL;
			PreloadTransition(false);
			return;
		}

		// import M2I
		StringPointer = Marshal::StringToHGlobalUni(textBoxInputM2I->Text);
		Error = preloadM2->ImportM2Intermediate((Char16*)StringPointer.ToPointer(), !checkBoxMergeBones->Checked, !checkBoxMergeAttachments->Checked, !checkBoxMergeCameras->Checked, checkBoxFixSeams->Checked);
		Marshal::FreeHGlobal(StringPointer);

		if (Error != 0)
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
		openFileDialog1->FileName = textBoxReplaceM2->Text;
		if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			textBoxReplaceM2->Text = openFileDialog1->FileName;
		}
	}
};
}
