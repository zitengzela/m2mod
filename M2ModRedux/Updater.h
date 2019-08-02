#pragma once

#include "UpdateForm.h"

namespace M2ModRedux
{

	using namespace System::Windows::Forms;
	using namespace System::Text::RegularExpressions;
	using namespace System::Net;

	ref class Updater
	{
		static String^ UpdateUrl = "https://bitbucket.org/suncurio/m2mod/raw/8.x/M2ModRedux/Version.h";

	public: String ^ GetRepoVersion()
	{
		ServicePointManager::SecurityProtocol =  SecurityProtocolType::Tls12;

		auto client = gcnew System::Net::WebClient();
		try
		{
			auto file = client->DownloadString(gcnew System::Uri(UpdateUrl));
			if (file == String::Empty)
				return nullptr;

			auto matches = Regex::Matches(file, "#define\\s+([\\w_]+)\\s+(\\d+)", RegexOptions::Multiline);
			if (matches->Count == 0)
				return nullptr;

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
				return nullptr;

			return String::Format("{0}.{1}.{2}", versionMajor, versionMinor, versionPatch);
		}
		catch (...)
		{
		}

		return nullptr;
	}

	public: int CheckUpdates()
	{
		auto currentVersion = String::Format("{0}.{1}.{2}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
		auto repoVersion = GetRepoVersion();
		if (String::IsNullOrEmpty(repoVersion))
		{
			MessageBox::Show("Failed to checkupdates", "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
			return 0;
		}
		else if (repoVersion == currentVersion)
		{
			MessageBox::Show("Up to date", "", MessageBoxButtons::OK, MessageBoxIcon::Information);
			return 0;
		}

		auto form = gcnew UpdateForm();
		form->SetVersionData(repoVersion);
		form->ShowDialog();
		return 0;
	}
	};
}
