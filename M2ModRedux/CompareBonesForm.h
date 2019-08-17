#pragma once

#include "M2.h"
#include "BoneComparator.h"
#include "StringHelpers.h"

#ifndef _DEFINE_FILTERS
# define _DEFINE_FILTERS
# define M2Filter L"M2 Files|*.m2|All Files|*.*"
# define M2IFilter L"M2I Files|*.m2i|All Files|*.*"
#endif

#define TXTFilter L"Txt Files|*.txt|All Files|*.*"

namespace M2ModRedux {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace System::Runtime::InteropServices;

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

			if (auto Value = RegistyStore::GetValue(RegistyStore::Value::OldCompareM2))
				oldM2TextBox->Text = (String^)Value;
			if (auto Value = RegistyStore::GetValue(RegistyStore::Value::NewCompareM2))
				newM2TextBox->Text = (String^)Value;
			if (auto Value = RegistyStore::GetValue(RegistyStore::Value::CompareWeightThreshold))
				weightThresholdTextBox->Text = (String^)Value;
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
	private: System::Windows::Forms::Button^  saveButton;
	private: System::Windows::Forms::TextBox^  weightThresholdTextBox;

	private: System::Windows::Forms::Label^  weightThresholdLabel;



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
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->weightThresholdTextBox = (gcnew System::Windows::Forms::TextBox());
			this->weightThresholdLabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// oldM2TextBox
			// 
			this->oldM2TextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->oldM2TextBox->Location = System::Drawing::Point(61, 13);
			this->oldM2TextBox->Name = L"oldM2TextBox";
			this->oldM2TextBox->Size = System::Drawing::Size(390, 20);
			this->oldM2TextBox->TabIndex = 0;
			// 
			// newM2TextBox
			// 
			this->newM2TextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->newM2TextBox->Location = System::Drawing::Point(61, 39);
			this->newM2TextBox->Name = L"newM2TextBox";
			this->newM2TextBox->Size = System::Drawing::Size(390, 20);
			this->newM2TextBox->TabIndex = 1;
			// 
			// oldM2Label
			// 
			this->oldM2Label->AutoSize = true;
			this->oldM2Label->Location = System::Drawing::Point(14, 16);
			this->oldM2Label->Name = L"oldM2Label";
			this->oldM2Label->Size = System::Drawing::Size(41, 13);
			this->oldM2Label->TabIndex = 3;
			this->oldM2Label->Text = L"Old M2";
			// 
			// newM2Label
			// 
			this->newM2Label->AutoSize = true;
			this->newM2Label->Location = System::Drawing::Point(8, 42);
			this->newM2Label->Name = L"newM2Label";
			this->newM2Label->Size = System::Drawing::Size(47, 13);
			this->newM2Label->TabIndex = 4;
			this->newM2Label->Text = L"New M2";
			// 
			// oldM2BrowseButton
			// 
			this->oldM2BrowseButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->oldM2BrowseButton->Location = System::Drawing::Point(462, 12);
			this->oldM2BrowseButton->Name = L"oldM2BrowseButton";
			this->oldM2BrowseButton->Size = System::Drawing::Size(52, 20);
			this->oldM2BrowseButton->TabIndex = 6;
			this->oldM2BrowseButton->Text = L"...";
			this->oldM2BrowseButton->UseVisualStyleBackColor = true;
			this->oldM2BrowseButton->Click += gcnew System::EventHandler(this, &CompareBonesForm::oldM2BrowseButton_Click);
			// 
			// newM2BrowseButton
			// 
			this->newM2BrowseButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->newM2BrowseButton->Location = System::Drawing::Point(462, 39);
			this->newM2BrowseButton->Name = L"newM2BrowseButton";
			this->newM2BrowseButton->Size = System::Drawing::Size(52, 20);
			this->newM2BrowseButton->TabIndex = 7;
			this->newM2BrowseButton->Text = L"...";
			this->newM2BrowseButton->UseVisualStyleBackColor = true;
			this->newM2BrowseButton->Click += gcnew System::EventHandler(this, &CompareBonesForm::newM2BrowseButton_Click);
			// 
			// resultsTextBox
			// 
			this->resultsTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->resultsTextBox->Location = System::Drawing::Point(17, 91);
			this->resultsTextBox->Multiline = true;
			this->resultsTextBox->Name = L"resultsTextBox";
			this->resultsTextBox->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->resultsTextBox->Size = System::Drawing::Size(497, 271);
			this->resultsTextBox->TabIndex = 8;
			this->resultsTextBox->TextChanged += gcnew System::EventHandler(this, &CompareBonesForm::resultsTextBox_TextChanged);
			// 
			// compareButton
			// 
			this->compareButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->compareButton->Location = System::Drawing::Point(61, 65);
			this->compareButton->Name = L"compareButton";
			this->compareButton->Size = System::Drawing::Size(59, 20);
			this->compareButton->TabIndex = 9;
			this->compareButton->Text = L"Compare";
			this->compareButton->UseVisualStyleBackColor = true;
			this->compareButton->Click += gcnew System::EventHandler(this, &CompareBonesForm::compareButton_Click);
			// 
			// saveButton
			// 
			this->saveButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->saveButton->Enabled = false;
			this->saveButton->Location = System::Drawing::Point(126, 65);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(59, 20);
			this->saveButton->TabIndex = 10;
			this->saveButton->Text = L"Save";
			this->saveButton->UseVisualStyleBackColor = true;
			this->saveButton->Click += gcnew System::EventHandler(this, &CompareBonesForm::saveButton_Click);
			// 
			// weightThresholdTextBox
			// 
			this->weightThresholdTextBox->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->weightThresholdTextBox->Location = System::Drawing::Point(407, 66);
			this->weightThresholdTextBox->Name = L"weightThresholdTextBox";
			this->weightThresholdTextBox->Size = System::Drawing::Size(44, 20);
			this->weightThresholdTextBox->TabIndex = 11;
			// 
			// weightThresholdLabel
			// 
			this->weightThresholdLabel->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->weightThresholdLabel->AutoSize = true;
			this->weightThresholdLabel->Location = System::Drawing::Point(310, 69);
			this->weightThresholdLabel->Name = L"weightThresholdLabel";
			this->weightThresholdLabel->Size = System::Drawing::Size(91, 13);
			this->weightThresholdLabel->TabIndex = 12;
			this->weightThresholdLabel->Text = L"Weight Threshold";
			// 
			// CompareBonesForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(529, 374);
			this->Controls->Add(this->weightThresholdLabel);
			this->Controls->Add(this->weightThresholdTextBox);
			this->Controls->Add(this->saveButton);
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
		Compare();
	}

	private: void Compare()
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

		RegistyStore::SetValue(RegistyStore::Value::OldCompareM2, oldM2TextBox->Text);
		RegistyStore::SetValue(RegistyStore::Value::NewCompareM2, newM2TextBox->Text);
		RegistyStore::SetValue(RegistyStore::Value::CompareWeightThreshold, weightThresholdTextBox->Text);

		M2Lib::M2 oldM2, newM2;
		auto Error = oldM2.Load(StringConverter(oldM2TextBox->Text).ToStringW());
		if (Error != M2Lib::EError_OK)
		{
			MessageBox::Show(String::Format("Failed to load '{0}': {1}", oldM2TextBox->Text, gcnew String(M2Lib::GetErrorText(Error).c_str())), "Error", Windows::Forms::MessageBoxButtons::OK, Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		Error = newM2.Load(StringConverter(newM2TextBox->Text).ToStringW());
		if (Error != M2Lib::EError_OK)
		{
			MessageBox::Show(String::Format("Failed to load '{0}': {1}", newM2TextBox->Text, gcnew String(M2Lib::GetErrorText(Error).c_str())), "Error", Windows::Forms::MessageBoxButtons::OK, Windows::Forms::MessageBoxIcon::Error);
			return;
		}

		resultsTextBox->Text = "";
		auto result = M2Lib::BoneComparator::Diff(&oldM2, &newM2, true);
		if (result.empty())
		{
			MessageBox::Show("Empty result from bone comparator");
			return;
		}

		float weightTreshold;
		if (!float::TryParse(weightThresholdTextBox->Text, weightTreshold))
		{
			weightTreshold = 0.0;
			weightThresholdTextBox->Text = "0";
		}

		auto compareStatus = M2Lib::BoneComparator::GetDifferenceStatus(result, weightTreshold);

		if (compareStatus == M2Lib::BoneComparator::CompareStatus::Identical)
		{
			resultsTextBox->Text += "# Old and new model bones are identical\r\n";
			return;
		}
		else if (compareStatus == M2Lib::BoneComparator::CompareStatus::IdenticalWithinThreshold)
		{
			resultsTextBox->Text += "# Old and new model bones are identical within given threshold " + weightTreshold.ToString("0.00") + "\r\n";
			return;
		}

		resultsTextBox->Text += "# Old M2: " + oldM2TextBox->Text + "\r\n";
		resultsTextBox->Text += "# New M2: " + newM2TextBox->Text + "\r\n";
		resultsTextBox->Text += "# Weight threshold: " + weightThresholdTextBox->Text + "\r\n";
		resultsTextBox->Text += "# Use this file with Blender\r\n";
		resultsTextBox->Text += "# \r\n";
		resultsTextBox->Text += "# Output [old bone]: [new bone #1] (probability weight) [new bone #2] (probability weight) ...\r\n";
		resultsTextBox->Text += "# To use this file in Blender you MUST remove extra bone candidates to make sure only one bone present\r\n";
		resultsTextBox->Text += "# Bring contents to format: [old bone]: [new bone]\r\n";
		resultsTextBox->Text += "# E.g.:\r\n";
		resultsTextBox->Text += "# 13: 14\r\n";
		resultsTextBox->Text += "#\r\n";
		resultsTextBox->Text += "# If any <no candidate> lines present - remove, but most likely you won't be able to use this file for conversion\r\n";

		for (auto itr : result)
		{
			auto weighted = itr.second;

			if (weighted.size() == 1 && weighted.begin()->first == itr.first)
				continue;

			resultsTextBox->Text += itr.first.ToString() + ": ";

			for (auto itr = weighted.begin(); itr != weighted.end();)
			{
				if (weightTreshold > 0 && weightTreshold > itr->second)
					weighted.erase(itr++);
				else
					++itr;
			}

			if (weighted.empty())
				resultsTextBox->Text += "<no candidate>";
			else
			{
				for (auto candidate : weighted)
				{
					resultsTextBox->Text += candidate.first.ToString() + "(" + candidate.second.ToString("0.00") + ") ";
				}
			}
			resultsTextBox->Text += "\r\n";
		}
	}

	private: System::Void resultsTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		saveButton->Enabled = resultsTextBox->Text->Length > 0;
	}
	private: System::Void saveButton_Click(System::Object^  sender, System::EventArgs^  e) {
		auto dialog = gcnew SaveFileDialog();
		dialog->CheckPathExists = true;
		dialog->Filter = TXTFilter;

		if (oldM2TextBox->Text->Length > 0)
		{
			auto name = Path::GetFileNameWithoutExtension(oldM2TextBox->Text);
			if (name->Length > 0)
				dialog->FileName = String::Format("bone_migration_{0}.txt", name);
		}
		auto result = dialog->ShowDialog();
		if (result != Windows::Forms::DialogResult::OK)
			return;

		File::WriteAllText(dialog->FileName, resultsTextBox->Text);
	}
};
}
