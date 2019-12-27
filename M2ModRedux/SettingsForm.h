#pragma once

#include "Settings.h"
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

		void Setup(M2Lib::Settings* settings)
		{
			M2Lib::Expansion ExpansionIndex = settings->ForceLoadExpansion;
			if (ExpansionIndex != M2Lib::Expansion::None)
				forceExpansionComboBox->SelectedIndex = (int32_t)ExpansionIndex + 1;
			else
				forceExpansionComboBox->SelectedIndex = 0;

			workingDirectoryTextBox->Text = gcnew String(settings->WorkingDirectory);
			outputDirectoryTextBox->Text = gcnew String(settings->OutputDirectory);

			checkBoxMergeBones->Checked = settings->MergeBones;
			checkBoxMergeAttachments->Checked = settings->MergeAttachments;
			checkBoxMergeCameras->Checked = settings->MergeCameras;
			checkBoxFixSeams->Checked = settings->FixSeams;
			checkBoxFixEdgeNormals->Checked = settings->FixEdgeNormals;
			checkBoxIgnoreOriginalMeshIndexes->Checked = settings->IgnoreOriginalMeshIndexes;
			testFixAnimationsCheckBox->Checked = settings->FixAnimationsTest;
		}

		M2Lib::Settings ProduceSettings()
		{
			M2Lib::Settings settings;

			settings.setWorkingDirectory(StringConverter(workingDirectoryTextBox->Text->Trim()).ToStringW());
			settings.setOutputDirectory(StringConverter(outputDirectoryTextBox->Text->Trim()).ToStringW());

			settings.MergeBones = checkBoxMergeBones->Checked;
			settings.MergeAttachments = checkBoxMergeAttachments->Checked;
			settings.MergeCameras = checkBoxMergeCameras->Checked;
			settings.FixSeams = checkBoxFixSeams->Checked;
			settings.FixEdgeNormals = checkBoxFixEdgeNormals->Checked;
			settings.IgnoreOriginalMeshIndexes = checkBoxIgnoreOriginalMeshIndexes->Checked;
			settings.FixAnimationsTest = testFixAnimationsCheckBox->Checked;

			if (forceExpansionComboBox->SelectedIndex > 0)
				settings.ForceLoadExpansion = (M2Lib::Expansion)(forceExpansionComboBox->SelectedIndex - 1);
			else
				settings.ForceLoadExpansion = (M2Lib::Expansion) - 1;

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






	private: System::Windows::Forms::CheckBox^  testFixAnimationsCheckBox;
	private: System::Windows::Forms::CheckBox^  checkBoxIgnoreOriginalMeshIndexes;

	private: System::Windows::Forms::CheckBox^  checkBoxFixEdgeNormals;
	private: System::Windows::Forms::GroupBox^  workingDirectoryGroupBox;

	private: System::Windows::Forms::Button^  workingDirectoryBrowseButton;




	private: System::Windows::Forms::TextBox^  workingDirectoryTextBox;
private: System::Windows::Forms::FolderBrowserDialog^  directoryBrowserDialog;
private: System::Windows::Forms::GroupBox^  outputDirectoryGroupBox;
private: System::Windows::Forms::Button^  outputDirectoryBrowseButton;
private: System::Windows::Forms::TextBox^  outputDirectoryTextBox;










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
			this->checkBoxFixEdgeNormals = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxIgnoreOriginalMeshIndexes = (gcnew System::Windows::Forms::CheckBox());
			this->testFixAnimationsCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxFixSeams = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxMergeCameras = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxMergeAttachments = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxMergeBones = (gcnew System::Windows::Forms::CheckBox());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->cancelButton = (gcnew System::Windows::Forms::Button());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->workingDirectoryGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->workingDirectoryBrowseButton = (gcnew System::Windows::Forms::Button());
			this->workingDirectoryTextBox = (gcnew System::Windows::Forms::TextBox());
			this->outputDirectoryGroupBox = (gcnew System::Windows::Forms::GroupBox());
			this->outputDirectoryBrowseButton = (gcnew System::Windows::Forms::Button());
			this->outputDirectoryTextBox = (gcnew System::Windows::Forms::TextBox());
			this->directoryBrowserDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->groupBox1->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->workingDirectoryGroupBox->SuspendLayout();
			this->outputDirectoryGroupBox->SuspendLayout();
			this->SuspendLayout();
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->forceExpansionComboBox);
			this->groupBox1->Location = System::Drawing::Point(12, 135);
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
			this->groupBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox3->Controls->Add(this->checkBoxFixEdgeNormals);
			this->groupBox3->Controls->Add(this->checkBoxIgnoreOriginalMeshIndexes);
			this->groupBox3->Controls->Add(this->testFixAnimationsCheckBox);
			this->groupBox3->Controls->Add(this->checkBoxFixSeams);
			this->groupBox3->Controls->Add(this->checkBoxMergeCameras);
			this->groupBox3->Controls->Add(this->checkBoxMergeAttachments);
			this->groupBox3->Controls->Add(this->checkBoxMergeBones);
			this->groupBox3->Location = System::Drawing::Point(12, 190);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(346, 116);
			this->groupBox3->TabIndex = 2;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Import Settings";
			this->checkBoxFixEdgeNormals->AutoSize = true;
			this->checkBoxFixEdgeNormals->Location = System::Drawing::Point(131, 42);
			this->checkBoxFixEdgeNormals->Name = L"checkBoxFixEdgeNormals";
			this->checkBoxFixEdgeNormals->Size = System::Drawing::Size(108, 17);
			this->checkBoxFixEdgeNormals->TabIndex = 22;
			this->checkBoxFixEdgeNormals->Text = L"Fix Edge Normals";
			this->toolTip1->SetToolTip(this->checkBoxFixEdgeNormals, L"Normals will be averaged on similar vertices between meshes");
			this->checkBoxFixEdgeNormals->UseVisualStyleBackColor = true;
			this->checkBoxIgnoreOriginalMeshIndexes->AutoSize = true;
			this->checkBoxIgnoreOriginalMeshIndexes->Location = System::Drawing::Point(131, 65);
			this->checkBoxIgnoreOriginalMeshIndexes->Name = L"checkBoxIgnoreOriginalMeshIndexes";
			this->checkBoxIgnoreOriginalMeshIndexes->Size = System::Drawing::Size(159, 17);
			this->checkBoxIgnoreOriginalMeshIndexes->TabIndex = 21;
			this->checkBoxIgnoreOriginalMeshIndexes->Text = L"Ignore original mesh indexes";
			this->toolTip1->SetToolTip(this->checkBoxIgnoreOriginalMeshIndexes, L"Materials for meshes will be selected comparing centers of mass between original "
				L"and modified meshes, not by index tunneled from original M2\r\n");
			this->checkBoxIgnoreOriginalMeshIndexes->UseVisualStyleBackColor = true;
			this->testFixAnimationsCheckBox->AutoSize = true;
			this->testFixAnimationsCheckBox->Location = System::Drawing::Point(131, 88);
			this->testFixAnimationsCheckBox->Name = L"testFixAnimationsCheckBox";
			this->testFixAnimationsCheckBox->Size = System::Drawing::Size(94, 17);
			this->testFixAnimationsCheckBox->TabIndex = 20;
			this->testFixAnimationsCheckBox->Text = L"Test Fix Anims";
			this->toolTip1->SetToolTip(this->testFixAnimationsCheckBox, L"Internal testing. Check only if you know what you do");
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
			this->saveButton->Location = System::Drawing::Point(188, 319);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(81, 29);
			this->saveButton->TabIndex = 3;
			this->saveButton->Text = L"Save";
			this->saveButton->UseVisualStyleBackColor = true;
			this->saveButton->Click += gcnew System::EventHandler(this, &SettingsForm::saveButton_Click);
			this->cancelButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->cancelButton->Location = System::Drawing::Point(275, 319);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(81, 29);
			this->cancelButton->TabIndex = 4;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &SettingsForm::cancelButton_Click);
			this->workingDirectoryGroupBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->workingDirectoryGroupBox->Controls->Add(this->workingDirectoryBrowseButton);
			this->workingDirectoryGroupBox->Controls->Add(this->workingDirectoryTextBox);
			this->workingDirectoryGroupBox->Location = System::Drawing::Point(12, 11);
			this->workingDirectoryGroupBox->Name = L"workingDirectoryGroupBox";
			this->workingDirectoryGroupBox->Size = System::Drawing::Size(346, 56);
			this->workingDirectoryGroupBox->TabIndex = 23;
			this->workingDirectoryGroupBox->TabStop = false;
			this->workingDirectoryGroupBox->Text = L"Working directory";
			this->toolTip1->SetToolTip(this->workingDirectoryGroupBox, L"Directory containing extracted Wow files");
			this->workingDirectoryBrowseButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->workingDirectoryBrowseButton->Location = System::Drawing::Point(304, 20);
			this->workingDirectoryBrowseButton->Name = L"workingDirectoryBrowseButton";
			this->workingDirectoryBrowseButton->Size = System::Drawing::Size(37, 22);
			this->workingDirectoryBrowseButton->TabIndex = 6;
			this->workingDirectoryBrowseButton->Text = L"...";
			this->workingDirectoryBrowseButton->UseVisualStyleBackColor = true;
			this->workingDirectoryBrowseButton->Click += gcnew System::EventHandler(this, &SettingsForm::workingDirectoryBrowseButton_Click);
			this->workingDirectoryTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->workingDirectoryTextBox->Location = System::Drawing::Point(9, 21);
			this->workingDirectoryTextBox->Name = L"workingDirectoryTextBox";
			this->workingDirectoryTextBox->Size = System::Drawing::Size(289, 20);
			this->workingDirectoryTextBox->TabIndex = 5;
			this->outputDirectoryGroupBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->outputDirectoryGroupBox->Controls->Add(this->outputDirectoryBrowseButton);
			this->outputDirectoryGroupBox->Controls->Add(this->outputDirectoryTextBox);
			this->outputDirectoryGroupBox->Location = System::Drawing::Point(12, 73);
			this->outputDirectoryGroupBox->Name = L"outputDirectoryGroupBox";
			this->outputDirectoryGroupBox->Size = System::Drawing::Size(346, 56);
			this->outputDirectoryGroupBox->TabIndex = 24;
			this->outputDirectoryGroupBox->TabStop = false;
			this->outputDirectoryGroupBox->Text = L"Output directory";
			this->toolTip1->SetToolTip(this->outputDirectoryGroupBox, L"M2Mod output files will be placed in this directory");
			this->outputDirectoryBrowseButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->outputDirectoryBrowseButton->Location = System::Drawing::Point(304, 20);
			this->outputDirectoryBrowseButton->Name = L"outputDirectoryBrowseButton";
			this->outputDirectoryBrowseButton->Size = System::Drawing::Size(37, 22);
			this->outputDirectoryBrowseButton->TabIndex = 6;
			this->outputDirectoryBrowseButton->Text = L"...";
			this->outputDirectoryBrowseButton->UseVisualStyleBackColor = true;
			this->outputDirectoryBrowseButton->Click += gcnew System::EventHandler(this, &SettingsForm::outputDirectoryBrowseButton_Click);
			this->outputDirectoryTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->outputDirectoryTextBox->Location = System::Drawing::Point(9, 21);
			this->outputDirectoryTextBox->Name = L"outputDirectoryTextBox";
			this->outputDirectoryTextBox->Size = System::Drawing::Size(289, 20);
			this->outputDirectoryTextBox->TabIndex = 5;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(368, 353);
			this->Controls->Add(this->outputDirectoryGroupBox);
			this->Controls->Add(this->workingDirectoryGroupBox);
			this->Controls->Add(this->cancelButton);
			this->Controls->Add(this->saveButton);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox1);
			this->Name = L"SettingsForm";
			this->Text = L"Settings";
			this->groupBox1->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->workingDirectoryGroupBox->ResumeLayout(false);
			this->workingDirectoryGroupBox->PerformLayout();
			this->outputDirectoryGroupBox->ResumeLayout(false);
			this->outputDirectoryGroupBox->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void saveButton_Click(System::Object^  sender, System::EventArgs^  e) {
		if (workingDirectoryTextBox->Text->Length > 0)
		{
			if (!Directory::Exists(workingDirectoryTextBox->Text))
			{
				MessageBox::Show("Working directory does not exist", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}
		}

		if (outputDirectoryTextBox->Text->Length > 0)
		{
			if (!Directory::Exists(outputDirectoryTextBox->Text))
			{
				MessageBox::Show("Output directory does not exist", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}
		}

		DialogResult = Windows::Forms::DialogResult::OK;
	}
	private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
		DialogResult = Windows::Forms::DialogResult::Cancel;
	}
	private: System::Void workingDirectoryBrowseButton_Click(System::Object^  sender, System::EventArgs^  e) {
		auto path = workingDirectoryTextBox->Text;
		if (path->Length > 0 && Directory::Exists(path))
			directoryBrowserDialog->SelectedPath = path;

		directoryBrowserDialog->Description = "Select working directory:";

		auto result = directoryBrowserDialog->ShowDialog();
		if (result == System::Windows::Forms::DialogResult::OK)
			workingDirectoryTextBox->Text = directoryBrowserDialog->SelectedPath;
	}
	private: System::Void outputDirectoryBrowseButton_Click(System::Object^  sender, System::EventArgs^  e) {
		auto path = outputDirectoryTextBox->Text;
		if (path->Length > 0 && Directory::Exists(path))
			directoryBrowserDialog->SelectedPath = path;

		directoryBrowserDialog->Description = "Select output directory:";

		auto result = directoryBrowserDialog->ShowDialog();
		if (result == System::Windows::Forms::DialogResult::OK)
			outputDirectoryTextBox->Text = directoryBrowserDialog->SelectedPath;
	}
};
}
