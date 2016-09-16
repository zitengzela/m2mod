#pragma once

#include "UpdateForm.h"

namespace M2ModRedux
{

	using namespace System::Windows::Forms;
	using namespace System::Text::RegularExpressions;

	ref class Updater
	{
	public: int Update()
	{
		auto client = gcnew System::Net::WebClient();
		try
		{
			auto file = client->DownloadString(gcnew System::Uri("https://bitbucket.org/suncurio/m2mod/raw/master/M2Lib/Version.h"));
			if (file == String::Empty)
				return 0;

			auto matches = Regex::Matches(file, "#define\\s+([\\w_]+)\\s+(\\d+)", RegexOptions::Multiline);
			if (matches->Count == 0)
				return 0;

			int versionMajor = -1, versionMinor = -1, versionPatch = -1;
			for (int i = 0; i < matches->Count; ++i)
			{
				auto match = matches[i];
				if (match->Groups[1]->Value == "VERSION_MAJOR")
					versionMajor = Int32::Parse(match->Groups[2]->Value);
				else if (match->Groups[1]->Value == "VERSION_MINOR")
					versionMinor = Int32::Parse(match->Groups[2]->Value);
				else if (match->Groups[1]->Value == "VERSION_PATCH")
					versionPatch = Int32::Parse(match->Groups[2]->Value);
			}

			if (versionMajor == -1 || versionMinor == -1 || versionPatch == -1)
				return 0;

			if (versionMajor != VERSION_MAJOR || versionMinor != VERSION_MINOR || versionPatch != VERSION_PATCH)
			{
				auto versionString = String::Format("{0}.{1}.{2}", versionMajor, versionMinor, versionPatch);
				if (versionString != RegistyStore::GetValue(RegistyStore::Value::ActualVersion)->ToString()
					|| !Boolean::Parse(RegistyStore::GetValue(RegistyStore::Value::NotShowAutoUpdate)->ToString()))
				{
					auto form = gcnew UpdateForm();
					form->SetVersionData(versionString);
					form->ShowDialog();
					RegistyStore::SetValue(RegistyStore::Value::NotShowAutoUpdate, form->checkBox1->Checked);
				}

				RegistyStore::SetValue(RegistyStore::Value::ActualVersion, versionString);
			}
		}
		catch (...)
		{

		}


		
		return 0;
	}
	};
}
