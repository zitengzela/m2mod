#pragma once

using namespace System;

ref class RegistyStore
{
	Microsoft::Win32::RegistryKey^ Root = nullptr;
public:

	enum class Value
	{
		ExportM2,
		ExportM2I,

		ImportInM2,
		ImportM2I,
		ImportOutM2,

		MergeBones,
		MergeAttachments,
		MergeCameras,
		FixSeams
	};

	RegistyStore()
	{
		Root = Microsoft::Win32::Registry::CurrentUser->CreateSubKey("M2Mod");
	}

	~RegistyStore()
	{
		Root->Close();
	}

	Object^ GetValue(Value Key) { return Root->GetValue(Key.ToString()); }
	System::Void SetValue(Value Key, Object^ Value) { Root->SetValue(Key.ToString(), Value); }
};
