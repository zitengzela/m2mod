#pragma once

namespace M2ModRedux
{
	using namespace System;

	ref class RegistyStore
	{
		static Microsoft::Win32::RegistryKey^ Root = nullptr;
	public:

		enum class Value
		{
			ExportM2,
			ExportM2I,

			ImportInM2,
			ImportM2I,
			ImportOutM2,
			ImportReplaceM2,

			MergeBones,
			MergeAttachments,
			MergeCameras,
			FixSeams,
			ForceExportExpansion,

			WowPath,

			NotShowAutoUpdate,
			ActualVersion,

			OldCompareM2,
			NewCompareM2
		};

		static RegistyStore()
		{
			Root = Microsoft::Win32::Registry::CurrentUser->CreateSubKey("M2Mod");
		}

		~RegistyStore()
		{
			Root->Close();
		}

		static Object^ GetValue(Value Key) { return Root->GetValue(Key.ToString()); }
		static System::Void SetValue(Value Key, Object^ Value) { Root->SetValue(Key.ToString(), Value); }
	};
}
