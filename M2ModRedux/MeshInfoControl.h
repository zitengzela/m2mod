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
	public: System::Windows::Forms::CheckBox^  makeGlossyCheckBox;
	public: System::Windows::Forms::CheckBox^  customTextureCheckBox;
	public: System::Windows::Forms::TextBox^  customTextureTextBox;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	public: System::Windows::Forms::TextBox^  glossTextureTextBox;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	public: System::Windows::Forms::CheckBox^  copyMaterialCheckBox;
	public: System::Windows::Forms::ComboBox^  srcMaterialComboBox;
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
			this->infoLabel = (gcnew System::Windows::Forms::Label());
			this->makeGlossyCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->glossTextureTextBox = (gcnew System::Windows::Forms::TextBox());
			this->customTextureCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->customTextureTextBox = (gcnew System::Windows::Forms::TextBox());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->copyMaterialCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->srcMaterialComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// infoLabel
			// 
			this->infoLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->infoLabel->Location = System::Drawing::Point(6, 12);
			this->infoLabel->Name = L"infoLabel";
			this->infoLabel->Size = System::Drawing::Size(312, 144);
			this->infoLabel->TabIndex = 0;
			this->infoLabel->Text = L"____";
			// 
			// makeGlossyCheckBox
			// 
			this->makeGlossyCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->makeGlossyCheckBox->AutoSize = true;
			this->makeGlossyCheckBox->Location = System::Drawing::Point(324, 107);
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
			this->glossTextureTextBox->Location = System::Drawing::Point(324, 128);
			this->glossTextureTextBox->Name = L"glossTextureTextBox";
			this->glossTextureTextBox->Size = System::Drawing::Size(100, 20);
			this->glossTextureTextBox->TabIndex = 2;
			this->toolTip1->SetToolTip(this->glossTextureTextBox, L"Path to gloss texture");
			// 
			// customTextureCheckBox
			// 
			this->customTextureCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->customTextureCheckBox->AutoSize = true;
			this->customTextureCheckBox->Location = System::Drawing::Point(324, 59);
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
			this->customTextureTextBox->Location = System::Drawing::Point(324, 79);
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
			this->groupBox1->Controls->Add(this->copyMaterialCheckBox);
			this->groupBox1->Controls->Add(this->srcMaterialComboBox);
			this->groupBox1->Controls->Add(this->customTextureCheckBox);
			this->groupBox1->Controls->Add(this->infoLabel);
			this->groupBox1->Controls->Add(this->customTextureTextBox);
			this->groupBox1->Controls->Add(this->makeGlossyCheckBox);
			this->groupBox1->Controls->Add(this->glossTextureTextBox);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->groupBox1->Location = System::Drawing::Point(0, 0);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(430, 159);
			this->groupBox1->TabIndex = 5;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"groupBox1";
			// 
			// copyMaterialCheckBox
			// 
			this->copyMaterialCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->copyMaterialCheckBox->AutoSize = true;
			this->copyMaterialCheckBox->Location = System::Drawing::Point(324, 12);
			this->copyMaterialCheckBox->Name = L"copyMaterialCheckBox";
			this->copyMaterialCheckBox->Size = System::Drawing::Size(90, 17);
			this->copyMaterialCheckBox->TabIndex = 6;
			this->copyMaterialCheckBox->Text = L"Copy Material";
			this->copyMaterialCheckBox->UseVisualStyleBackColor = true;
			this->copyMaterialCheckBox->CheckedChanged += gcnew System::EventHandler(this, &MeshInfoControl::copyMaterialCheckBox_CheckedChanged);
			// 
			// srcMaterialComboBox
			// 
			this->srcMaterialComboBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->srcMaterialComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->srcMaterialComboBox->Enabled = false;
			this->srcMaterialComboBox->FormattingEnabled = true;
			this->srcMaterialComboBox->Location = System::Drawing::Point(324, 32);
			this->srcMaterialComboBox->Name = L"srcMaterialComboBox";
			this->srcMaterialComboBox->Size = System::Drawing::Size(100, 21);
			this->srcMaterialComboBox->TabIndex = 5;
			this->toolTip1->SetToolTip(this->srcMaterialComboBox, L"Select source material");
			// 
			// MeshInfoControl
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->BackColor = System::Drawing::SystemColors::Control;
			this->Controls->Add(this->groupBox1);
			this->Name = L"MeshInfoControl";
			this->Size = System::Drawing::Size(430, 159);
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

	public: System::Void Initialize(int index, std::vector<M2Lib::M2Skin::MeshInfo> const& infos)
	{
		auto const& info = infos[index];

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
			text += "Texture #" + (i + 1) + " Type: " + gcnew String(M2Lib::M2Element::CElement_Texture::GetTypeString(texture.pTexture->Type).c_str());
			if (texture.pTexture->Type == M2Lib::M2Element::CElement_Texture::ETextureType::Final_Hardcoded)
				text += " Name: " + gcnew String(texture.Name.c_str()) + "\n";
			else
				text += "\n";
		}

		infoLabel->Text = text;

		System::Collections::Generic::List<String^> SrcMaterials;
		SrcMaterials.Add("");
		for (unsigned int i = 0; i < infos.size(); ++i)
		{
			auto const& info = infos[i];

			auto Element = gcnew String("# " + i + " ID = " + info.ID);
			if (!info.Description.empty())
				Element += " \"" + gcnew String(info.Description.c_str()) + "\"";

			SrcMaterials.Add(Element);
		}

		srcMaterialComboBox->Items->AddRange(SrcMaterials.ToArray());
	}

	public: System::Boolean Fits(String^ SearchString)
	{
		return infoLabel->Text->ToLower()->Contains(SearchString->ToLower());
	}

	private: System::Void copyMaterialCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		srcMaterialComboBox->Enabled = copyMaterialCheckBox->Checked;
		customTextureCheckBox->Enabled = !copyMaterialCheckBox->Checked;
		customTextureTextBox->Enabled = !copyMaterialCheckBox->Checked && customTextureCheckBox->Checked;
		makeGlossyCheckBox->Enabled = !copyMaterialCheckBox->Checked;
		glossTextureTextBox->Enabled = !copyMaterialCheckBox->Checked && makeGlossyCheckBox->Checked;
	}
};
}
