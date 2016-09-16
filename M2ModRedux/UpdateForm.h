#pragma once

#include "Version.h"
#include "RegistryStore.h"

namespace M2ModRedux {

	ref class Form1;

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for UpdateForm
	/// </summary>
	public ref class UpdateForm : public System::Windows::Forms::Form
	{
	public:
		UpdateForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));

			label1->Text = "A new version of M2Mod is available:";

			linkLabel1->Top = label1->Top;

			label1->Left = this->Width / 2 - (label1->Width + 2 + linkLabel1->Width) / 2;
			linkLabel1->Left = label1->Left + 3 + label1->Width;

			linkLabel1->Links->Add(0, linkLabel1->Text->Length, "https://bitbucket.org/suncurio/m2mod/downloads");
		}

		Void SetVersionData(String^ VersionString)
		{
			linkLabel1->Text = VersionString;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~UpdateForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::LinkLabel^  linkLabel1;
	protected:
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  okButton;
	public: System::Windows::Forms::CheckBox^  checkBox1;

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
			this->linkLabel1 = (gcnew System::Windows::Forms::LinkLabel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->okButton = (gcnew System::Windows::Forms::Button());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->SuspendLayout();
			// 
			// linkLabel1
			// 
			this->linkLabel1->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->linkLabel1->AutoSize = true;
			this->linkLabel1->Location = System::Drawing::Point(12, 77);
			this->linkLabel1->Name = L"linkLabel1";
			this->linkLabel1->Size = System::Drawing::Size(55, 13);
			this->linkLabel1->TabIndex = 0;
			this->linkLabel1->TabStop = true;
			this->linkLabel1->Text = L"linkLabel1";
			this->linkLabel1->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &UpdateForm::linkLabel1_LinkClicked);
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 32);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(35, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"label1";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// okButton
			// 
			this->okButton->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->okButton->Location = System::Drawing::Point(136, 138);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(75, 35);
			this->okButton->TabIndex = 2;
			this->okButton->Text = L"OK";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &UpdateForm::okButton_Click);
			// 
			// checkBox1
			// 
			this->checkBox1->Anchor = System::Windows::Forms::AnchorStyles::Top;
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(228, 156);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(108, 17);
			this->checkBox1->TabIndex = 3;
			this->checkBox1->Text = L"Don\'t show again";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// UpdateForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(348, 185);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->okButton);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->linkLabel1);
			this->Name = L"UpdateForm";
			this->Text = L"Update Available";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void linkLabel1_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
		auto target = (String^)e->Link->LinkData;
		System::Diagnostics::Process::Start(target);
	}
	private: System::Void UpdateForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	}
	private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e) {
		DialogResult = Windows::Forms::DialogResult::OK;
	}
};
}
