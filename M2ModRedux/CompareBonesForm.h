#pragma once

#include "M2.h"
#include "BoneComparator.h"

namespace M2ModRedux {

#define M2Filter L"M2 Files|*.m2|All Files|*.*"
#define M2BRFilter L"M2BR Files|*.m2br|All Files|*.*"

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Runtime::InteropServices;

	ref class CompareBonesForm;
	ref class Form1;

	/// <summary>
	/// Summary for CompareBonesForm
	/// </summary>
	public ref class CompareBonesForm : public System::Windows::Forms::Form
	{
	public:
		CompareBonesForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CompareBonesForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  oldM2TextBox;
	private: System::Windows::Forms::TextBox^  newM2TextBox;
	protected:

	protected:

	private: System::Windows::Forms::Label^  oldM2Label;
	private: System::Windows::Forms::Label^  newM2Label;
	private: System::Windows::Forms::Button^  oldM2BrowseButton;
	private: System::Windows::Forms::Button^  newM2BrowseButton;
	private: System::Windows::Forms::TextBox^  resultsTextBox;






	private: System::Windows::Forms::Button^  compareButton;
	private: System::Windows::Forms::CheckBox^  saveCheckBox;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->oldM2TextBox = (gcnew System::Windows::Forms::TextBox());
			this->newM2TextBox = (gcnew System::Windows::Forms::TextBox());
			this->oldM2Label = (gcnew System::Windows::Forms::Label());
			this->newM2Label = (gcnew System::Windows::Forms::Label());
			this->oldM2BrowseButton = (gcnew System::Windows::Forms::Button());
			this->newM2BrowseButton = (gcnew System::Windows::Forms::Button());
			this->resultsTextBox = (gcnew System::Windows::Forms::TextBox());
			this->compareButton = (gcnew System::Windows::Forms::Button());
			this->saveCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->SuspendLayout();
			this->oldM2TextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->oldM2TextBox->Location = System::Drawing::Point(61, 13);
			this->oldM2TextBox->Name = L"oldM2TextBox";
			this->oldM2TextBox->Size = System::Drawing::Size(390, 20);
			this->oldM2TextBox->TabIndex = 0;
			this->newM2TextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->newM2TextBox->Location = System::Drawing::Point(61, 39);
			this->newM2TextBox->Name = L"newM2TextBox";
			this->newM2TextBox->Size = System::Drawing::Size(390, 20);
			this->newM2TextBox->TabIndex = 1;
			this->oldM2Label->AutoSize = true;
			this->oldM2Label->Location = System::Drawing::Point(14, 16);
			this->oldM2Label->Name = L"oldM2Label";
			this->oldM2Label->Size = System::Drawing::Size(41, 13);
			this->oldM2Label->TabIndex = 3;
			this->oldM2Label->Text = L"Old M2";
			this->newM2Label->AutoSize = true;
			this->newM2Label->Location = System::Drawing::Point(8, 42);
			this->newM2Label->Name = L"newM2Label";
			this->newM2Label->Size = System::Drawing::Size(47, 13);
			this->newM2Label->TabIndex = 4;
			this->newM2Label->Text = L"New M2";
			this->oldM2BrowseButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->oldM2BrowseButton->Location = System::Drawing::Point(462, 12);
			this->oldM2BrowseButton->Name = L"oldM2BrowseButton";
			this->oldM2BrowseButton->Size = System::Drawing::Size(52, 20);
			this->oldM2BrowseButton->TabIndex = 6;
			this->oldM2BrowseButton->Text = L"...";
			this->oldM2BrowseButton->UseVisualStyleBackColor = true;
			this->oldM2BrowseButton->Click += gcnew System::EventHandler(this, &CompareBonesForm::oldM2BrowseButton_Click);
			this->newM2BrowseButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->newM2BrowseButton->Location = System::Drawing::Point(462, 39);
			this->newM2BrowseButton->Name = L"newM2BrowseButton";
			this->newM2BrowseButton->Size = System::Drawing::Size(52, 20);
			this->newM2BrowseButton->TabIndex = 7;
			this->newM2BrowseButton->Text = L"...";
			this->newM2BrowseButton->UseVisualStyleBackColor = true;
			this->newM2BrowseButton->Click += gcnew System::EventHandler(this, &CompareBonesForm::newM2BrowseButton_Click);
			this->resultsTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->resultsTextBox->Location = System::Drawing::Point(17, 103);
			this->resultsTextBox->Multiline = true;
			this->resultsTextBox->Name = L"resultsTextBox";
			this->resultsTextBox->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->resultsTextBox->Size = System::Drawing::Size(497, 103);
			this->resultsTextBox->TabIndex = 8;
			this->compareButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->compareButton->Location = System::Drawing::Point(252, 67);
			this->compareButton->Name = L"compareButton";
			this->compareButton->Size = System::Drawing::Size(59, 30);
			this->compareButton->TabIndex = 9;
			this->compareButton->Text = L"Compare";
			this->compareButton->UseVisualStyleBackColor = true;
			this->compareButton->Click += gcnew System::EventHandler(this, &CompareBonesForm::compareButton_Click);
			this->saveCheckBox->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->saveCheckBox->AutoSize = true;
			this->saveCheckBox->Location = System::Drawing::Point(195, 75);
			this->saveCheckBox->Name = L"saveCheckBox";
			this->saveCheckBox->Size = System::Drawing::Size(51, 17);
			this->saveCheckBox->TabIndex = 10;
			this->saveCheckBox->Text = L"Save";
			this->saveCheckBox->UseVisualStyleBackColor = true;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(529, 218);
			this->Controls->Add(this->saveCheckBox);
			this->Controls->Add(this->compareButton);
			this->Controls->Add(this->resultsTextBox);
			this->Controls->Add(this->newM2BrowseButton);
			this->Controls->Add(this->oldM2BrowseButton);
			this->Controls->Add(this->newM2Label);
			this->Controls->Add(this->oldM2Label);
			this->Controls->Add(this->newM2TextBox);
			this->Controls->Add(this->oldM2TextBox);
			this->Name = L"CompareBonesForm";
			this->Text = L"Compare Bones";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void oldM2BrowseButton_Click(System::Object^  sender, System::EventArgs^  e) {
		auto dialog = gcnew OpenFileDialog();
		dialog->Filter = M2Filter;
		auto result = dialog->ShowDialog();
		if (result != Windows::Forms::DialogResult::OK)
			return;

		oldM2TextBox->Text = dialog->FileName;
	}
	private: System::Void newM2BrowseButton_Click(System::Object^  sender, System::EventArgs^  e) {
		auto dialog = gcnew OpenFileDialog();
		dialog->Filter = M2Filter;
		auto result = dialog->ShowDialog();
		if (result != Windows::Forms::DialogResult::OK)
			return;

		newM2TextBox->Text = dialog->FileName;
	}
	private: System::Void compareButton_Click(System::Object^  sender, System::EventArgs^  e) {
		Compare(saveCheckBox->Checked);
	}

	private: void Compare(bool save)
	{
		if (!File::Exists(oldM2TextBox->Text))
		{
			MessageBox::Show(String::Format("No such file: '{0}'", oldM2TextBox->Text), "Error", Windows::Forms::MessageBoxButtons::OK, Windows::Forms::MessageBoxIcon::Error);
			return;
		}
		if (!File::Exists(newM2TextBox->Text))
		{
			MessageBox::Show(String::Format("No such file: '{0}'", newM2TextBox->Text), "Error", Windows::Forms::MessageBoxButtons::OK, Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		M2Lib::M2 oldM2, newM2;
		auto StringPointer = Marshal::StringToHGlobalUni(oldM2TextBox->Text);
		auto Error = oldM2.Load((Char16*)StringPointer.ToPointer());
		Marshal::FreeHGlobal(StringPointer);
		if (Error != M2Lib::EError_OK)
		{
			MessageBox::Show(String::Format("Failed to load '{0}': {1}", oldM2TextBox->Text, gcnew String(M2Lib::GetErrorText(Error))), "Error", Windows::Forms::MessageBoxButtons::OK, Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		StringPointer = Marshal::StringToHGlobalUni(newM2TextBox->Text);
		Error = newM2.Load((Char16*)StringPointer.ToPointer());
		Marshal::FreeHGlobal(StringPointer);

		if (Error != M2Lib::EError_OK)
		{
			MessageBox::Show(String::Format("Failed to load '{0}': {1}", newM2TextBox->Text, gcnew String(M2Lib::GetErrorText(Error))), "Error", Windows::Forms::MessageBoxButtons::OK, Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		M2Lib::BoneComparator comparator(&oldM2, &newM2);
		auto result = comparator.ShowDifference();

		resultsTextBox->Text = gcnew String(result.empty() ? "Bones are identical" : result.c_str());

		if (save)
		{
			auto dialog = gcnew SaveFileDialog();
			dialog->Filter = M2BRFilter;
			auto result = dialog->ShowDialog();
			if (result == Windows::Forms::DialogResult::OK)
			{
				auto filePath = dialog->FileName;
				StringPointer = Marshal::StringToHGlobalAnsi(filePath);
				Error = comparator.Save((char const*)StringPointer.ToPointer());
				Marshal::FreeHGlobal(StringPointer);

				if (Error != M2Lib::EError_OK)
					MessageBox::Show(String::Format("Failed to save bone remap data: {0}", gcnew String(M2Lib::GetErrorText(Error))), "Error", Windows::Forms::MessageBoxButtons::OK, Windows::Forms::MessageBoxIcon::Error);
			}
		}
	}
	};
}
