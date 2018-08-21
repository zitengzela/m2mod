#pragma once

#include "Settings.h"
#include "FileSystem.h"
#include "StringHelpers.h"

namespace M2ModRedux {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;
	using namespace System::Runtime::InteropServices;
	using namespace System::IO;

	ref class SettingsForm;
	ref class Form1;

	/// <summary>
	/// Summary for SettingsForm
	/// </summary>
	public ref class SettingsForm : public System::Windows::Forms::Form
	{
	public:
		SettingsForm(void)
		{
			InitializeComponent();

			List<String^> items;
			items.Add("");
			items.Add("Classic");
			items.Add("BurningCrusade");
			items.Add("WrathOfTheLichKing");
			items.Add("Cataclysm");
			items.Add("MistsOfPandaria");
			items.Add("WarlordsOfDraenor");
			items.Add("Legion");
			forceExpansionComboBox->Items->AddRange(items.ToArray());

			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
		}

		void Setup(M2Lib::GlobalSettings* settings)
		{
			M2Lib::Expansion ExpansionIndex = settings->ExportSettings.ForceExpansion;
			if (ExpansionIndex != M2Lib::Expansion::None)
				forceExpansionComboBox->SelectedIndex = (SInt32)ExpansionIndex + 1;
			else
				forceExpansionComboBox->SelectedIndex = 0;

			wowPathTextBox->Text = gcnew String(settings->WowPath.c_str());

			checkBoxMergeBones->Checked = settings->ImportSettings.MergeBones;
			checkBoxMergeAttachments->Checked = settings->ImportSettings.MergeAttachments;
			checkBoxMergeCameras->Checked = settings->ImportSettings.MergeCameras;
			checkBoxFixSeams->Checked = settings->ImportSettings.FixSeams;
			testFixAnimationsCheckBox->Checked = settings->ImportSettings.FixAnimationsTest;
		}

		M2Lib::GlobalSettings ProduceSettings()
		{
			M2Lib::GlobalSettings settings;

			settings.WowPath = StringConverter(wowPathTextBox->Text->Trim()).ToStringA();

			settings.ImportSettings.MergeBones = checkBoxMergeBones->Checked;
			settings.ImportSettings.MergeAttachments = checkBoxMergeAttachments->Checked;
			settings.ImportSettings.MergeCameras = checkBoxMergeCameras->Checked;
			settings.ImportSettings.FixSeams = checkBoxFixSeams->Checked;
			settings.ImportSettings.FixAnimationsTest = testFixAnimationsCheckBox->Checked;

			if (forceExpansionComboBox->SelectedIndex > 0)
				settings.ExportSettings.ForceExpansion = (M2Lib::Expansion)(forceExpansionComboBox->SelectedIndex - 1);
			else
				settings.ExportSettings.ForceExpansion = (M2Lib::Expansion) - 1;

			return settings;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SettingsForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::GroupBox^  groupBox1;
	protected:

	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Button^  saveButton;
	private: System::Windows::Forms::Button^  cancelButton;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ComboBox^  forceExpansionComboBox;
	private: System::Windows::Forms::CheckBox^  checkBoxMergeBones;
	private: System::Windows::Forms::CheckBox^  checkBoxMergeAttachments;
	private: System::Windows::Forms::CheckBox^  checkBoxMergeCameras;
	private: System::Windows::Forms::CheckBox^  checkBoxFixSeams;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::TextBox^  wowPathTextBox;

	private: System::Windows::Forms::Button^  wowBrowseButton;

	private: System::Windows::Forms::OpenFileDialog^  wowBrowseDialog;
	private: System::Windows::Forms::GroupBox^  wowPathGroupBox;
	private: System::Windows::Forms::CheckBox^  testFixAnimationsCheckBox;

	private: System::ComponentModel::IContainer^  components;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->forceExpansionComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->testFixAnimationsCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFixSeams = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxMergeCameras = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxMergeAttachments = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxMergeBones = (gcnew System::Windows::Forms::CheckBox());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->cancelButton = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->wowPathTextBox = (gcnew System::Windows::Forms::TextBox());
			this->wowBrowseButton = (gcnew System::Windows::Forms::Button());
			this->wowBrowseDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->wowPathGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->wowPathGroupBox->SuspendLayout();
			this->SuspendLayout();
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->forceExpansionComboBox);
			this->groupBox1->Location = System::Drawing::Point(12, 73);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(346, 53);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Export Settings";
			this->label1->Location = System::Drawing::Point(6, 20);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(153, 29);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Force Input M2 Expansion:";
			this->forceExpansionComboBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->forceExpansionComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->forceExpansionComboBox->FormattingEnabled = true;
			this->forceExpansionComboBox->Location = System::Drawing::Point(218, 18);
			this->forceExpansionComboBox->Name = L"forceExpansionComboBox";
			this->forceExpansionComboBox->Size = System::Drawing::Size(122, 21);
			this->forceExpansionComboBox->TabIndex = 0;
			this->toolTip1->SetToolTip(this->forceExpansionComboBox, L"All input M2\'s will be treated accordingly");
			this->groupBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox3->Controls->Add(this->testFixAnimationsCheckBox);
			this->groupBox3->Controls->Add(this->checkBoxFixSeams);
			this->groupBox3->Controls->Add(this->checkBoxMergeCameras);
			this->groupBox3->Controls->Add(this->checkBoxMergeAttachments);
			this->groupBox3->Controls->Add(this->checkBoxMergeBones);
			this->groupBox3->Location = System::Drawing::Point(12, 127);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(346, 94);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Import Settings";
			this->testFixAnimationsCheckBox->AutoSize = true;
			this->testFixAnimationsCheckBox->Location = System::Drawing::Point(131, 42);
			this->testFixAnimationsCheckBox->Name = L"testFixAnimationsCheckBox";
			this->testFixAnimationsCheckBox->Size = System::Drawing::Size(94, 17);
			this->testFixAnimationsCheckBox->TabIndex = 20;
			this->testFixAnimationsCheckBox->Text = L"Test Fix Anims";
			this->toolTip1->SetToolTip(this->testFixAnimationsCheckBox, L"Seams between meshes will be eleminated, also normals will be averaged on edges");
			this->testFixAnimationsCheckBox->UseVisualStyleBackColor = true;
			this->checkBoxFixSeams->AutoSize = true;
			this->checkBoxFixSeams->Location = System::Drawing::Point(131, 19);
			this->checkBoxFixSeams->Name = L"checkBoxFixSeams";
			this->checkBoxFixSeams->Size = System::Drawing::Size(74, 17);
			this->checkBoxFixSeams->TabIndex = 19;
			this->checkBoxFixSeams->Text = L"Fix Seams";
			this->toolTip1->SetToolTip(this->checkBoxFixSeams, L"Seams between meshes will be eleminated, also normals will be averaged on edges");
			this->checkBoxFixSeams->UseVisualStyleBackColor = true;
			this->checkBoxMergeCameras->AutoSize = true;
			this->checkBoxMergeCameras->Checked = true;
			this->checkBoxMergeCameras->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxMergeCameras->Location = System::Drawing::Point(9, 65);
			this->checkBoxMergeCameras->Name = L"checkBoxMergeCameras";
			this->checkBoxMergeCameras->Size = System::Drawing::Size(100, 17);
			this->checkBoxMergeCameras->TabIndex = 18;
			this->checkBoxMergeCameras->Text = L"Merge Cameras";
			this->toolTip1->SetToolTip(this->checkBoxMergeCameras, L"Merge cameras from M2I to result model");
			this->checkBoxMergeCameras->UseVisualStyleBackColor = true;
			this->checkBoxMergeAttachments->AutoSize = true;
			this->checkBoxMergeAttachments->Checked = true;
			this->checkBoxMergeAttachments->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxMergeAttachments->Location = System::Drawing::Point(9, 42);
			this->checkBoxMergeAttachments->Name = L"checkBoxMergeAttachments";
			this->checkBoxMergeAttachments->Size = System::Drawing::Size(118, 17);
			this->checkBoxMergeAttachments->TabIndex = 17;
			this->checkBoxMergeAttachments->Text = L"Merge Attachments";
			this->toolTip1->SetToolTip(this->checkBoxMergeAttachments, L"Merge attachments from M2I to result model");
			this->checkBoxMergeAttachments->UseVisualStyleBackColor = true;
			this->checkBoxMergeBones->AutoSize = true;
			this->checkBoxMergeBones->Checked = true;
			this->checkBoxMergeBones->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxMergeBones->Location = System::Drawing::Point(9, 19);
			this->checkBoxMergeBones->Name = L"checkBoxMergeBones";
			this->checkBoxMergeBones->Size = System::Drawing::Size(89, 17);
			this->checkBoxMergeBones->TabIndex = 16;
			this->checkBoxMergeBones->Text = L"Merge Bones";
			this->toolTip1->SetToolTip(this->checkBoxMergeBones, L"Merge bones from M2I to result model");
			this->checkBoxMergeBones->UseVisualStyleBackColor = true;
			this->saveButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->saveButton->Location = System::Drawing::Point(188, 228);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(81, 29);
			this->saveButton->TabIndex = 3;
			this->saveButton->Text = L"Save";
			this->saveButton->UseVisualStyleBackColor = true;
			this->saveButton->Click += gcnew System::EventHandler(this, &SettingsForm::saveButton_Click);
			this->cancelButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cancelButton->Location = System::Drawing::Point(275, 228);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(81, 29);
			this->cancelButton->TabIndex = 4;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &SettingsForm::cancelButton_Click);
			this->wowPathTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->wowPathTextBox->Location = System::Drawing::Point(9, 21);
			this->wowPathTextBox->Name = L"wowPathTextBox";
			this->wowPathTextBox->Size = System::Drawing::Size(289, 20);
			this->wowPathTextBox->TabIndex = 5;
			this->wowBrowseButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->wowBrowseButton->Location = System::Drawing::Point(304, 20);
			this->wowBrowseButton->Name = L"wowBrowseButton";
			this->wowBrowseButton->Size = System::Drawing::Size(37, 22);
			this->wowBrowseButton->TabIndex = 6;
			this->wowBrowseButton->Text = L"...";
			this->wowBrowseButton->UseVisualStyleBackColor = true;
			this->wowBrowseButton->Click += gcnew System::EventHandler(this, &SettingsForm::wowBrowseButton_Click);
			this->wowBrowseDialog->FileName = L"Wow.exe";
			this->wowBrowseDialog->Filter = L"WoW Executable|*.exe";
			this->wowPathGroupBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->wowPathGroupBox->Controls->Add(this->wowBrowseButton);
			this->wowPathGroupBox->Controls->Add(this->wowPathTextBox);
			this->wowPathGroupBox->Location = System::Drawing::Point(12, 12);
			this->wowPathGroupBox->Name = L"wowPathGroupBox";
			this->wowPathGroupBox->Size = System::Drawing::Size(346, 61);
			this->wowPathGroupBox->TabIndex = 2;
			this->wowPathGroupBox->TabStop = false;
			this->wowPathGroupBox->Text = L"WoW Path";
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(368, 262);
			this->Controls->Add(this->wowPathGroupBox);
			this->Controls->Add(this->cancelButton);
			this->Controls->Add(this->saveButton);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox1);
			this->Name = L"SettingsForm";
			this->Text = L"Settings";
			this->groupBox1->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->wowPathGroupBox->ResumeLayout(false);
			this->wowPathGroupBox->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void saveButton_Click(System::Object^  sender, System::EventArgs^  e) {
		std::string WowPath = StringConverter(wowPathTextBox->Text).ToStringA();
		if (WowPath.length() > 0)
		{
			if (!M2Lib::FileSystemA::IsFile(WowPath + "\\" + "Wow.exe"))
			{
				MessageBox::Show("Path must point to Wow.exe", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}
		}

		DialogResult = Windows::Forms::DialogResult::OK;
	}
	private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
		DialogResult = Windows::Forms::DialogResult::Cancel;
	}
	private: System::Void wowBrowseButton_Click(System::Object^  sender, System::EventArgs^  e) {
		auto path = wowPathTextBox->Text;
		if (path->Length > 0 && File::Exists(path))
			wowBrowseDialog->InitialDirectory = path;

		auto result = wowBrowseDialog->ShowDialog();
		if (result == System::Windows::Forms::DialogResult::OK)
		{
			wowPathTextBox->Text = Path::GetDirectoryName(wowBrowseDialog->FileName);
		}
	}
};
}
