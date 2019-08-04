#pragma once

namespace M2ModRedux {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for ErrorForm
	/// </summary>
	public ref class ErrorForm : public System::Windows::Forms::Form
	{
	private:
		ErrorForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			auto owner = this->Owner;
			auto parent = this->Parent;
			auto parentForm = this->ParentForm;
		}

		static Drawing::Size^ GetTextDimensions(Control^ control, Drawing::Font^ font, String^ stringData)
		{
			Graphics^ g = control->CreateGraphics();
			SizeF sizeF = g->MeasureString(stringData, font, control->Width);
			delete g;

			return gcnew Drawing::Size((int)Math::Ceiling(sizeF.Width), (int)Math::Ceiling(sizeF.Height));
		}

		void UpdateTextbox(String^ text)
		{
			auto dimensions = GetTextDimensions(textBox1, textBox1->Font, text);

			auto maxHeight = this->ClientSize.Height - 20 - 35;

			textBox1->Size = Drawing::Size(textBox1->Size.Width, Math::Min(dimensions->Height, maxHeight));

			textBox1->Location = Drawing::Point(textBox1->Location.X, 20 + maxHeight / 2 - textBox1->Size.Height / 2);

			textBox1->ScrollBars = dimensions->Height >
				textBox1->Height ?
				ScrollBars::Vertical : ScrollBars::None;
		}

	public:
		static Windows::Forms::DialogResult Show(String^ text, String^ title, MessageBoxIcon icon)
		{
			auto form = gcnew ErrorForm();
			if (String::IsNullOrEmpty(title))
			{
				switch (icon)
				{
					case MessageBoxIcon::Warning:
						title = "Warning";
						break;
					case MessageBoxIcon::Error:
						title = "Error";
						break;
					case MessageBoxIcon::Question:
						title = "Question";
						break;
					case MessageBoxIcon::Asterisk:
						title = "Information";
						break;
					default:
						title = "Information";
						break;
				}
			}
			form->Text = title;
			form->MessageIcon = icon;

			form->UpdateTextbox(text);

			form->textBox1->AppendText(text);
			form->textBox1->SelectionStart = form->textBox1->TextLength;
			form->textBox1->ScrollToCaret();

			form->StartPosition = FormStartPosition::CenterScreen;

			return form->ShowDialog();
		}

		static Windows::Forms::DialogResult Show(String^ text)
		{
			return Show(text, nullptr, MessageBoxIcon::None);
		}

		static  Windows::Forms::DialogResult Show(String^ text, String^ title)
		{
			return Show(text, title, MessageBoxIcon::None);
		}

		static Windows::Forms::DialogResult Show(String^ text, MessageBoxIcon icon)
		{
			return Show(text, nullptr, icon);
		}

		static Windows::Forms::DialogResult ShowError(String^ text)
		{
			return Show(text, nullptr, MessageBoxIcon::Error);
		}

		static  Windows::Forms::DialogResult ShowError(String^ text, String^ title)
		{
			return Show(text, title, MessageBoxIcon::Error);
		}

		static Windows::Forms::DialogResult ShowWarning(String^ text)
		{
			return Show(text, nullptr, MessageBoxIcon::Warning);
		}

		static  Windows::Forms::DialogResult ShowWarning(String^ text, String^ title)
		{
			return Show(text, title, MessageBoxIcon::Warning);
		}

	private: System::Windows::Forms::PictureBox^ pictureBox1;

	private: System::Windows::Forms::CheckBox^ checkBox1;
private: System::Windows::Forms::TextBox^ textBox1;


	private:
		property MessageBoxIcon MessageIcon
		{
			MessageBoxIcon get()
			{
				return messageIcon;
			}
			void set(MessageBoxIcon value)
			{
				messageIcon = value;

				Drawing::Icon^ icon = nullptr;
				switch (messageIcon)
				{
					case MessageBoxIcon::Warning:
						icon = SystemIcons::Warning;
						break;
					case MessageBoxIcon::Error:
						icon = SystemIcons::Error;
						break;
					case MessageBoxIcon::Question:
						icon = SystemIcons::Question;
						break;
					case MessageBoxIcon::Asterisk:
						icon = SystemIcons::Asterisk;
						break;
					default:
						icon = SystemIcons::Asterisk;
						break;
				}

				if (icon != nullptr)
				{
					pictureBox1->Image = icon->ToBitmap();
					this->Icon = icon;
				}
			}
		}

	protected:
		MessageBoxIcon messageIcon;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ErrorForm()
		{
			if (components)
			{
				delete components;
			}
		}


	private: System::Windows::Forms::Button^ button1;
	private: System::ComponentModel::IContainer^ components;

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
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// checkBox1
			// 
			this->checkBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(12, 110);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(108, 17);
			this->checkBox1->TabIndex = 1;
			this->checkBox1->Text = L"Don\'t show again";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button1->Location = System::Drawing::Point(280, 107);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 0;
			this->button1->Text = L"OK";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &ErrorForm::Button1_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Anchor = System::Windows::Forms::AnchorStyles::Left;
			this->pictureBox1->Location = System::Drawing::Point(11, 44);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(32, 32);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->BackColor = System::Drawing::SystemColors::Control;
			this->textBox1->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->textBox1->Location = System::Drawing::Point(52, 29);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(311, 73);
			this->textBox1->TabIndex = 0;
			// 
			// ErrorForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(367, 139);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->pictureBox1);
			this->MinimumSize = System::Drawing::Size(383, 178);
			this->Name = L"ErrorForm";
			this->Text = L"ErrorForm";
			this->Load += gcnew System::EventHandler(this, &ErrorForm::ErrorForm_Load);
			this->SizeChanged += gcnew System::EventHandler(this, &ErrorForm::ErrorForm_SizeChanged);
			this->Resize += gcnew System::EventHandler(this, &ErrorForm::ErrorForm_Resize);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Button1_Click(System::Object^ sender, System::EventArgs^ e) {
		this->DialogResult = checkBox1->Checked ? Windows::Forms::DialogResult::Ignore : Windows::Forms::DialogResult::OK;
	}
	private: System::Void ErrorForm_Load(System::Object^ sender, System::EventArgs^ e) {
		this->ActiveControl = button1;
	}
	private: System::Void ErrorForm_SizeChanged(System::Object^ sender, System::EventArgs^ e) {
		UpdateTextbox(textBox1->Text);
	}
	private: System::Void ErrorForm_Resize(System::Object^ sender, System::EventArgs^ e) {
		UpdateTextbox(textBox1->Text);
	}
};
}
