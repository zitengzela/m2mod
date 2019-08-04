#pragma once

namespace M2ModRedux {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for ElementManagementForm
	/// </summary>
	template <class ControlType>
	public ref class ElementManagementForm : public System::Windows::Forms::Form
	{
	public:
		ElementManagementForm(void)
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
		~ElementManagementForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	protected:
	private: System::Windows::Forms::Button^  cancelButton;
	private: System::Windows::Forms::Button^  saveButton;
	private: System::Windows::Forms::Label^  filterLabel;
	private: System::Windows::Forms::TextBox^  filterTextBox;
	private: System::Windows::Forms::Label^ label1;


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
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->cancelButton = (gcnew System::Windows::Forms::Button());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->filterLabel = (gcnew System::Windows::Forms::Label());
			this->filterTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->AutoScroll = true;
			this->splitContainer1->Panel1->Controls->Add(this->label1);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->cancelButton);
			this->splitContainer1->Panel2->Controls->Add(this->saveButton);
			this->splitContainer1->Panel2->Controls->Add(this->filterLabel);
			this->splitContainer1->Panel2->Controls->Add(this->filterTextBox);
			this->splitContainer1->Size = System::Drawing::Size(464, 289);
			this->splitContainer1->SplitterDistance = 350;
			this->splitContainer1->TabIndex = 0;
			// 
			// cancelButton
			// 
			this->cancelButton->Location = System::Drawing::Point(7, 91);
			this->cancelButton->Name = L"cancelButton";
			this->cancelButton->Size = System::Drawing::Size(91, 30);
			this->cancelButton->TabIndex = 3;
			this->cancelButton->Text = L"Cancel";
			this->cancelButton->UseVisualStyleBackColor = true;
			this->cancelButton->Click += gcnew System::EventHandler(this, &ElementManagementForm::cancelButton_Click);
			// 
			// saveButton
			// 
			this->saveButton->Location = System::Drawing::Point(7, 55);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(91, 30);
			this->saveButton->TabIndex = 2;
			this->saveButton->Text = L"Save";
			this->saveButton->UseVisualStyleBackColor = true;
			this->saveButton->Click += gcnew System::EventHandler(this, &ElementManagementForm::saveButton_Click);
			// 
			// filterLabel
			// 
			this->filterLabel->AutoSize = true;
			this->filterLabel->Location = System::Drawing::Point(4, 9);
			this->filterLabel->Name = L"filterLabel";
			this->filterLabel->Size = System::Drawing::Size(32, 13);
			this->filterLabel->TabIndex = 1;
			this->filterLabel->Text = L"Filter:";
			// 
			// filterTextBox
			// 
			this->filterTextBox->Location = System::Drawing::Point(5, 28);
			this->filterTextBox->Name = L"filterTextBox";
			this->filterTextBox->Size = System::Drawing::Size(100, 20);
			this->filterTextBox->TabIndex = 0;
			this->filterTextBox->TextChanged += gcnew System::EventHandler(this, &ElementManagementForm::filterTextBox_TextChanged);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->ForeColor = System::Drawing::Color::Red;
			this->label1->Location = System::Drawing::Point(101, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(146, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Do not use this - use Blender ";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// ElementManagementForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(464, 289);
			this->Controls->Add(this->splitContainer1);
			this->Name = L"ElementManagementForm";
			this->Text = L"ElementManagementForm";
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel1->PerformLayout();
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->Panel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

	public: System::Collections::Generic::List<ControlType^> ElementInfoList;

	public: System::Void AddElementInfo(ControlType^ Control)
	{
		Control->Left = 5;

		//Control->Anchor = (AnchorStyles)((int)AnchorStyles::Left || (int)AnchorStyles::Right);
		//Control->Anchor = AnchorStyles::Right + AnchorStyles::Left;
		Control->Width = splitContainer1->Panel1->Width - 25;
		Control->Anchor |= AnchorStyles::Right;

		ElementInfoList.Add(Control);
		splitContainer1->Panel1->Controls->Add(Control);
	}

	public: System::Void SetupInfoPositions()
	{
		ControlType^ Prev = nullptr;

		int TopOffset = 5 + label1->Location.Y + label1->Size.Height;
		for (int i = 0; i < ElementInfoList.Count; ++i)
		{
			auto Control = ElementInfoList[i];
			if (filterTextBox->Text->Length && !Control->Fits(filterTextBox->Text))
			{
				Control->Hide();
				continue;
			}

			Control->Show();

			if (!Prev)
				Control->Top = TopOffset;
			else
				Control->Top = Prev->Top + Prev->Height + 5;

			Prev = Control;
		}
	}

	private: System::Void filterTextBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		SetupInfoPositions();
	}
	private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
		DialogResult = Windows::Forms::DialogResult::Cancel;
	}
private: System::Void saveButton_Click(System::Object^  sender, System::EventArgs^  e) {
	this->DialogResult = Windows::Forms::DialogResult::OK;
}
};
}
