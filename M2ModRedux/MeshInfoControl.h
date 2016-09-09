#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace M2ModRedux {

	/// <summary>
	/// Summary for MeshInfoControl
	/// </summary>
	public ref class MeshInfoControl : public System::Windows::Forms::UserControl
	{
	public:
		MeshInfoControl(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MeshInfoControl()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::Label^  infoLabel;
	protected:

	protected:

	public: System::Windows::Forms::CheckBox^  makeGlossyCheckBox;


	public: System::Windows::Forms::CheckBox^  customTextureCheckBox;
	public: System::Windows::Forms::TextBox^  customTextureTextBox;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	public: System::Windows::Forms::TextBox^  glossTextureTextBox;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	public:
	private:
	public:
	private: System::ComponentModel::IContainer^  components;
	protected:


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
			this->infoLabel = (gcnew System::Windows::Forms::Label());
			this->makeGlossyCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->glossTextureTextBox = (gcnew System::Windows::Forms::TextBox());
			this->customTextureCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->customTextureTextBox = (gcnew System::Windows::Forms::TextBox());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// infoLabel
			// 
			this->infoLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->infoLabel->Location = System::Drawing::Point(6, 12);
			this->infoLabel->Name = L"infoLabel";
			this->infoLabel->Size = System::Drawing::Size(241, 93);
			this->infoLabel->TabIndex = 0;
			this->infoLabel->Text = L"____";
			// 
			// makeGlossyCheckBox
			// 
			this->makeGlossyCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->makeGlossyCheckBox->AutoSize = true;
			this->makeGlossyCheckBox->Location = System::Drawing::Point(253, 62);
			this->makeGlossyCheckBox->Name = L"makeGlossyCheckBox";
			this->makeGlossyCheckBox->Size = System::Drawing::Size(87, 17);
			this->makeGlossyCheckBox->TabIndex = 1;
			this->makeGlossyCheckBox->Text = L"Make Glossy";
			this->makeGlossyCheckBox->UseVisualStyleBackColor = true;
			this->makeGlossyCheckBox->CheckedChanged += gcnew System::EventHandler(this, &MeshInfoControl::makeGlossyCheckBox_CheckedChanged);
			// 
			// glossTextureTextBox
			// 
			this->glossTextureTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->glossTextureTextBox->Enabled = false;
			this->glossTextureTextBox->Location = System::Drawing::Point(253, 85);
			this->glossTextureTextBox->Name = L"glossTextureTextBox";
			this->glossTextureTextBox->Size = System::Drawing::Size(100, 20);
			this->glossTextureTextBox->TabIndex = 2;
			this->toolTip1->SetToolTip(this->glossTextureTextBox, L"Path to gloss texture");
			// 
			// customTextureCheckBox
			// 
			this->customTextureCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->customTextureCheckBox->AutoSize = true;
			this->customTextureCheckBox->Location = System::Drawing::Point(253, 13);
			this->customTextureCheckBox->Name = L"customTextureCheckBox";
			this->customTextureCheckBox->Size = System::Drawing::Size(100, 17);
			this->customTextureCheckBox->TabIndex = 3;
			this->customTextureCheckBox->Text = L"Custom Texture";
			this->customTextureCheckBox->UseVisualStyleBackColor = true;
			this->customTextureCheckBox->CheckedChanged += gcnew System::EventHandler(this, &MeshInfoControl::customTextureCheckBox_CheckedChanged);
			// 
			// customTextureTextBox
			// 
			this->customTextureTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->customTextureTextBox->Enabled = false;
			this->customTextureTextBox->Location = System::Drawing::Point(253, 36);
			this->customTextureTextBox->Name = L"customTextureTextBox";
			this->customTextureTextBox->Size = System::Drawing::Size(100, 20);
			this->customTextureTextBox->TabIndex = 4;
			this->toolTip1->SetToolTip(this->customTextureTextBox, L"Path to custom texture");
			// 
			// toolTip1
			// 
			this->toolTip1->AutomaticDelay = 300;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->customTextureCheckBox);
			this->groupBox1->Controls->Add(this->infoLabel);
			this->groupBox1->Controls->Add(this->customTextureTextBox);
			this->groupBox1->Controls->Add(this->makeGlossyCheckBox);
			this->groupBox1->Controls->Add(this->glossTextureTextBox);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(359, 119);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"groupBox1";
			// 
			// MeshInfoControl
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->Controls->Add(this->groupBox1);
			this->Name = L"MeshInfoControl";
			this->Size = System::Drawing::Size(359, 119);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void makeGlossyCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		glossTextureTextBox->Enabled = makeGlossyCheckBox->Checked;
	}

	private: System::Void customTextureCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		customTextureTextBox->Enabled = customTextureCheckBox->Checked;
	}

	public: System::Void Initialize(int index, M2Lib::M2Skin::MeshInfo& info)
	{
		groupBox1->Text = gcnew String("#" + (index + 1) + ": ");
		String^ text = gcnew String("ID = " + info.ID);
		if (info.Description.size())
			text += " \"" + gcnew String(info.Description.c_str()) + "\"\n";
		else
			text += "\n";

		for (auto& material : info.Materials)
			text += "Material: op_count = " + material->op_count + "\n";

		for (int i = 0; i < info.Textures.size(); ++i)
		{
			auto& texture = info.Textures[i];
			text += "Texture #" + (i + 1) + " Type: " + gcnew String(M2Lib::M2Element::CElement_Texture::GetTypeString(info.Textures[0].pTexture->Type).c_str());
			if (texture.pTexture->Type == M2Lib::M2Element::CElement_Texture::ETextureType::Final_Hardcoded)
				text += " Name: " + gcnew String(texture.Name.c_str()) + "\n";
			else
				text += "\n";
		}

		this->infoLabel->Text = text;
	}

	public: System::Boolean Fits(String^ SearchString)
	{
		return infoLabel->Text->ToLower()->Contains(SearchString->ToLower());
	}

	};
}
