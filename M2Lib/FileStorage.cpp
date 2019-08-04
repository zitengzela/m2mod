#include "FileStorage.h"
#include "FileSystem.h"
#include "Logger.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <filesystem>

const M2Lib::FileInfo M2Lib::FileInfo::Empty = { 0, "" };

const std::string M2Lib::FileStorage::DefaultListfilePath = FileSystemA::GetCurrentPath() + "\\" + "listfile.csv";
const std::string M2Lib::FileStorage::ListfileAddonsPath = FileSystemA::GetCurrentPath() + "\\" + "listfile_addons";

M2Lib::FileStorage::FileStorage()
{
}

bool M2Lib::FileStorage::ParseCsv(std::string const& Path, bool CheckExisting)
{
	std::fstream in;
	in.open(Path, std::ios::in);
	if (in.fail())
		return false;

	UInt32 dwLocaleFlags = 0;
	std::string line;
	while (std::getline(in, line))
	{
		if (line.empty() || line.find(';') == std::string::npos)
			continue;

		std::istringstream stream(line);

		std::string tmp, fileName;
		std::getline(stream, tmp, ';');
		UInt32 FileDataId = std::stoul(tmp);
		std::getline(stream, fileName, ';');

		auto nameHash = CalculateHash(fileName);
		FileInfo info = { FileDataId, fileName };
		if (CheckExisting)
		{
			auto itr1 = fileInfosByFileDataId.find(FileDataId);
			if (itr1 != fileInfosByFileDataId.end())
			{
				sLogger.LogWarning("Duplicate storage entry for FileDataId '%u' '%s' (already used by path '%s'), skipping", FileDataId, fileName.c_str(), itr1->second.Path.c_str());
				continue;
			}
			auto itr2 = fileInfosByNameHash.find(nameHash);
			if (itr2 != fileInfosByNameHash.end())
			{
				sLogger.LogWarning("Duplicate storage entry for path '%s' (already uses FileDataId '%u'), skipping", itr2->second.Path.c_str(), FileDataId);
				continue;
			}
		}

		fileInfosByFileDataId[FileDataId] = info;
		fileInfosByNameHash[nameHash] = info;
	}
	in.close();

	return true;
}

bool M2Lib::FileStorage::LoadStorage(std::string const& ListfilePath)
{
	ClearStorage();
	sLogger.LogInfo("Loading listfile at '%s'", ListfilePath.c_str());

	if (!ParseCsv(ListfilePath, false))
	{
		sLogger.LogError("Failed to open listfile at path: '%s'", ListfilePath.c_str());
		return false;
	}

	UInt32 addonCount = 0;
	if (!LoadListFileAddons(addonCount))
	{
		sLogger.LogError("Failed to load listfile addons at '%s'", ListfileAddonsPath.c_str());
		return false;
	}

	sLogger.LogInfo("Loaded %u entries listfile entries (including %u custom ones)", fileInfosByFileDataId.size(), addonCount);
	
	return true;
}

bool M2Lib::FileStorage::LoadListFileAddons(UInt32& totalAddons)
{
	if (!std::filesystem::is_directory(ListfileAddonsPath)) {
		sLogger.LogError("Listfile addons directory '%s' does not exist", ListfileAddonsPath.c_str());
		return false;
	}
	std::filesystem::directory_iterator itr;

	UInt32 currentCount = fileInfosByFileDataId.size();
	bool ok = true;
	for (auto& p : std::filesystem::directory_iterator(ListfileAddonsPath))
	{
		if (p.path().extension().string() != ".csv")
			continue;

		sLogger.LogInfo("Loading listfile addon '%s'", p.path().filename().string().c_str());
		if (!ParseCsv(p.path().string(), true)) {
			sLogger.LogError("Failed to parse listfile addon file '%s'", p.path().filename().string().c_str());
			ok = false;
		}
	}

	totalAddons = fileInfosByFileDataId.size() - currentCount;

	return true;
}

M2Lib::FileInfo const& M2Lib::FileStorage::GetFileInfoByPartialPath(std::string const & Name)
{
	auto normalizePath = [](std::string path) -> std::string
	{
		path = FileSystemA::NormalizePath(path);
		std::transform(path.begin(), path.end(), path.begin(), ::tolower);

		return path;
	};

	auto NameCopy = normalizePath(Name);

	for (auto& itr : fileInfosByFileDataId)
	{
		if (normalizePath(itr.second.Path).find(NameCopy) != std::string::npos)
			return itr.second;
	}

	return FileInfo::Empty;
}

M2Lib::FileInfo const& M2Lib::FileStorage::GetFileInfoByFileDataId(UInt32 FileDataId)
{
	auto itr = fileInfosByFileDataId.find(FileDataId);
	if (itr == fileInfosByFileDataId.end())
		return FileInfo::Empty;

	return itr->second;
}

M2Lib::FileInfo const& M2Lib::FileStorage::GetFileInfoByPath(std::string const& Path)
{
	UInt64 hash = CalculateHash(Path);
	auto itr = fileInfosByNameHash.find(hash);
	if (itr != fileInfosByNameHash.end())
		return itr->second;

	return FileInfo::Empty;
}

UInt64 M2Lib::FileStorage::CalculateHash(std::string const & FileName)
{
	auto name = FileName;
	std::transform(name.begin(), name.end(), name.begin(), [](auto c) {return std::tolower(c); });

	name = FileSystemA::NormalizePath(name);

	static std::hash<std::string> hasher;

	return hasher(name);
}

std::string M2Lib::FileStorage::PathInfo(UInt32 FileDataId)
{
	if (!FileDataId)
		return "<none>";

	auto info = GetInstance()->GetFileInfoByFileDataId(FileDataId);
	if (info.IsEmpty())
		return "<not found in listfile>";

	return info.Path;
};
