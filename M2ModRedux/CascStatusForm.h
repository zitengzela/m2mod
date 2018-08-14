#pragma once

#include "Casc.h"

namespace M2ModRedux {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for CascStatusForm
	/// </summary>
	public ref class CascStatusForm : public System::Windows::Forms::Form
	{
	public:
		CascStatusForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		void SetParameters(M2Lib::Casc* casc, M2Lib::GlobalSettings* settings) { this->casc = casc; this->settings = settings; }

	protected:
		void Analyze()
		{
			std::string text = "Storage path: ";
			if (settings && settings->WowPath.length())
				text += settings->WowPath + "\r\n";
			else
				text += "<uninitialized>\r\n";

			text += "Casc status: ";
			if (!casc)
				text += "<uninitialied>\r\n";
			else if (!casc->IsLoaded())
				text += "not loaded\r\n";
			else
				text += "loaded\r\n";

			text += "Cache size: ";
			if (!casc)
				text += "<uninitialied>\r\n";
			else
				text += std::to_string(casc->GetCacheSize()) + "\r\n";

			loadCascButton->Enabled = casc && !casc->IsLoaded();
			regenerateCacheButton->Text = casc && casc->GetCacheSize() ? "Regenerate cache" : "Generate cache";

			infoTextBox->Text = gcnew String(text.c_str());
		}
	private: System::Windows::Forms::Button^  loadCascButton;
	private: System::Windows::Forms::Button^  regenerateCacheButton;
	protected:

	protected:

		M2Lib::Casc* casc;
		M2Lib::GlobalSettings* settings;
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~CascStatusForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  infoTextBox;
	protected:

	protected:

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
			this->infoTextBox = (gcnew System::Windows::Forms::TextBox());
			this->loadCascButton = (gcnew System::Windows::Forms::Button());
			this->regenerateCacheButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// infoTextBox
			// 
			this->infoTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->infoTextBox->Location = System::Drawing::Point(13, 13);
			this->infoTextBox->Multiline = true;
			this->infoTextBox->Name = L"infoTextBox";
			this->infoTextBox->ReadOnly = true;
			this->infoTextBox->Size = System::Drawing::Size(300, 195);
			this->infoTextBox->TabIndex = 0;
			// 
			// loadCascButton
			// 
			this->loadCascButton->Location = System::Drawing::Point(12, 214);
			this->loadCascButton->Name = L"loadCascButton";
			this->loadCascButton->Size = System::Drawing::Size(114, 23);
			this->loadCascButton->TabIndex = 1;
			this->loadCascButton->Text = L"Initialize CASC";
			this->loadCascButton->UseVisualStyleBackColor = true;
			this->loadCascButton->Click += gcnew System::EventHandler(this, &CascStatusForm::loadCascButton_Click);
			// 
			// regenerateCacheButton
			// 
			this->regenerateCacheButton->Location = System::Drawing::Point(142, 214);
			this->regenerateCacheButton->Name = L"regenerateCacheButton";
			this->regenerateCacheButton->Size = System::Drawing::Size(119, 23);
			this->regenerateCacheButton->TabIndex = 2;
			this->regenerateCacheButton->Text = L"button2";
			this->regenerateCacheButton->UseVisualStyleBackColor = true;
			this->regenerateCacheButton->Click += gcnew System::EventHandler(this, &CascStatusForm::regenerateCacheButton_Click);
			// 
			// CascStatusForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(325, 363);
			this->Controls->Add(this->regenerateCacheButton);
			this->Controls->Add(this->loadCascButton);
			this->Controls->Add(this->infoTextBox);
			this->Name = L"CascStatusForm";
			this->Text = L"Casc Info";
			this->Load += gcnew System::EventHandler(this, &CascStatusForm::CascStatusForm_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void CascStatusForm_Load(System::Object^  sender, System::EventArgs^  e) {
		Analyze();
	}
	private: System::Void loadCascButton_Click(System::Object^  sender, System::EventArgs^  e) {
		if (!casc || casc->IsLoaded())
			return;

		auto res = casc->Load();
		Analyze();
		if (!res)
			MessageBox::Show("Failed to initialize CASC storage", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	private: System::Void regenerateCacheButton_Click(System::Object^  sender, System::EventArgs^  e) {
		if (!casc)
			return;

		auto res = casc->GenerateListFileCache();
		Analyze();
		if (!res)
			MessageBox::Show("Failed to storage cache", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
};
}
