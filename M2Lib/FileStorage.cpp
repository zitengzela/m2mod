#include "FileStorage.h"
#include "Logger.h"
#include "StringHelpers.h"
#include "StringHash.h"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <locale>

const std::wstring M2Lib::FileStorage::DefaultMappingsPath = std::filesystem::current_path() / L"mappings";

M2Lib::FileInfo::FileInfo(uint32_t FileDataId, const wchar_t* Path)
{
	this->FileDataId = FileDataId;
	this->Path = NormalizePath<wchar_t>(Path);
}

void M2Lib::FileStorage::ClearStorage()
{
	loadFailed = false;
	for (auto& info : fileInfosByFileDataId)
		delete info.second;
	fileInfosByFileDataId.clear();
	fileInfosByNameHash.clear();
}

uint32_t M2Lib::FileInfo_GetFileDataId(M2LIB_HANDLE handle)
{
	return static_cast<FileInfo*>(handle)->FileDataId;
}

wchar_t const* M2Lib::FileInfo_GetPath(M2LIB_HANDLE handle)
{
	return static_cast<FileInfo*>(handle)->Path.c_str();
}

bool M2Lib::FileStorage::ParseCsv(std::wstring const& Path)
{
	std::wifstream in;
	in.open(Path, std::ios::in);
	if (in.fail())
		return false;

	uint32_t dwLocaleFlags = 0;
	std::wstring line;
	while (std::getline(in, line))
	{
		StringHelpers::trim(line, { ' ', '\r','\n' });

		if (line.empty())
			continue;

		auto colonPos = line.find(L';');
		if (colonPos == std::string::npos)
			continue;

		line[colonPos] = L'\0';
		wchar_t const* strId = &line[0];
		wchar_t const* fileName = &line[colonPos] + 1;

		uint32_t FileDataId = std::stoul(strId);

		auto itr1 = fileInfosByFileDataId.find(FileDataId);
		if (itr1 != fileInfosByFileDataId.end())
		{
			sLogger.LogWarning(L"Duplicate file storage entry '%u':'%s' (already used: '%u':'%s'), skipping", FileDataId, fileName, itr1->second->FileDataId, itr1->second->Path.c_str());
			continue;
		}

		auto nameHash = CalcStringHash<wchar_t>(fileName);
		auto itr2 = fileInfosByNameHash.find(nameHash);
		if (itr2 != fileInfosByNameHash.end())
		{
			sLogger.LogWarning(L"Duplicate file storage entry '%u':'%s' (already used: '%u':'%s')", FileDataId, fileName, itr2->second->FileDataId, itr2->second->Path.c_str());
			continue;
		}

		auto info = new FileInfo(FileDataId, fileName);
		if (MaxFileDataId < FileDataId)
			MaxFileDataId = FileDataId;

		fileInfosByFileDataId[FileDataId] = info;
		fileInfosByNameHash[nameHash] = info;
	}
	in.close();

	return true;
}

bool M2Lib::FileStorage::LoadStorage()
{
	if (!fileInfosByFileDataId.empty())
		return true;

	if (!LoadMappings()) {
		loadFailed = true;

		return false;
	}

	sLogger.LogInfo(L"Loaded %u mapping entries", fileInfosByFileDataId.size());
	
	return true;
}

void M2Lib::FileStorage::ResetLoadFailed()
{
	loadFailed = false;
}

M2Lib::FileStorage::FileStorage(std::wstring const& mappingsDirectory)
{
	SetMappingsDirectory(mappingsDirectory);
}

void M2Lib::FileStorage::SetMappingsDirectory(std::wstring const& mappingsDirectory)
{
	this->mappingsDirectory = mappingsDirectory;
	ClearStorage();
}

void M2Lib::FileStorage::AddRecord(FileInfo const* record)
{
	auto nameHash = CalcStringHash(record->Path);

	fileInfosByFileDataId[record->FileDataId] = record;
	fileInfosByNameHash[nameHash] = record;
}

M2Lib::FileStorage::~FileStorage()
{
	ClearStorage();
}

bool M2Lib::FileStorage::LoadMappings()
{
	if (loadFailed)
		return false;

	auto directory = mappingsDirectory.length() > 0 ? mappingsDirectory : DefaultMappingsPath;
	if (!std::filesystem::is_directory(directory)) {
		sLogger.LogWarning(L"Mappings directory '%s' does not exist", directory.c_str());
		return false;
	}

	sLogger.LogInfo(L"Loading mappings at '%s'", directory.c_str());

	std::filesystem::directory_iterator itr;

	const auto isSupportedExtension = [](std::wstring const& extension)
	{
		auto copy = extension;
		std::transform(copy.begin(), copy.end(), copy.begin(), ::towlower);
		return copy == L".csv" || copy == L".txt";
	};

	auto now = time(NULL);
	for (auto& p : std::filesystem::directory_iterator(directory))
	{
		if (!isSupportedExtension(p.path().extension()))
			continue;

		sLogger.LogInfo(L"Loading mapping '%s'", p.path().filename().wstring().c_str());

		try
		{
			if (!ParseCsv(p.path().wstring()))
				sLogger.LogError(L"Failed to parse mapping file '%s'", p.path().filename().wstring().c_str());
		}
		catch (std::exception& e)
		{
			sLogger.LogError(L"Failed to parse mapping file '%s': %s", p.path().filename().wstring().c_str(), StringHelpers::StringToWString(e.what()).c_str());
		}
	}

	return true;
}

M2Lib::FileInfo const* M2Lib::FileStorage::GetFileInfoByPartialPath(std::wstring const & Name)
{
	LoadStorage();

	const auto NameCopy = NormalizePath(Name);

	for (auto& itr : fileInfosByFileDataId)
	{
		if (NormalizePath(itr.second->Path).find(NameCopy) != std::string::npos)
			return itr.second;
	}

	return nullptr;
}

M2Lib::FileInfo const* M2Lib::FileStorage::GetFileInfoByFileDataId(uint32_t FileDataId)
{
	LoadStorage();

	auto itr = fileInfosByFileDataId.find(FileDataId);
	if (itr == fileInfosByFileDataId.end())
		return nullptr;

	return itr->second;
}

M2Lib::FileInfo const* M2Lib::FileStorage::GetFileInfoByPath(std::wstring const& Path)
{
	LoadStorage();

	uint64_t hash = CalcStringHash(Path);
	auto itr = fileInfosByNameHash.find(hash);
	if (itr != fileInfosByNameHash.end())
		return itr->second;

	return nullptr;
}

wchar_t const* M2Lib::FileStorage::PathInfo(uint32_t FileDataId)
{
	if (!FileDataId)
		return L"<none>";

	auto info = GetFileInfoByFileDataId(FileDataId);
	if (!info)
		return L"<not found in listfile>";

	return info->Path.c_str();
}

std::filesystem::path M2Lib::FileStorage::DetectWorkingDirectory(std::filesystem::path fullPath, std::filesystem::path relativePath)
{
	for (;;)
	{
		if (relativePath.empty())
			return fullPath;

		if (ToLower(relativePath.filename().wstring()) == ToLower(fullPath.filename().wstring()))
		{
			relativePath = relativePath.parent_path();
			fullPath = fullPath.parent_path();
		}
		else
			return "";
	}
}

M2Lib::FileStorage* M2Lib::StorageManager::GetStorage(std::wstring const& mappingDirectory)
{
	const auto hash = CalcStringHash(mappingDirectory);
	auto itr = storages.find(hash);
	if (itr != storages.end()) {

		itr->second->ResetLoadFailed();
		return itr->second;
	}

	auto storage = new FileStorage(mappingDirectory);
	storages[hash] = storage;

	return storage;
}

void M2Lib::StorageManager::Clear()
{
	for (auto storage : storages)
		delete storage.second;

	storages.clear();
}

M2Lib::StorageManager::~StorageManager()
{
	Clear();
}

M2LIB_HANDLE M2Lib::FileStorage_Get(const wchar_t* mappingsDirectory)
{
	return static_cast<M2LIB_HANDLE>(StorageManager::GetInstance()->GetStorage(mappingsDirectory));
}

void M2Lib::FileStorage_Clear()
{
	StorageManager::GetInstance()->Clear();
};

void M2Lib::FileStorage_SetMappingsDirectory(M2LIB_HANDLE handle, const wchar_t* mappingsDirectory)
{
	static_cast<FileStorage*>(handle)->SetMappingsDirectory(mappingsDirectory);
}

M2LIB_HANDLE M2Lib::FileStorage_GetFileInfoByFileDataId(M2LIB_HANDLE handle, uint32_t FileDataId)
{
	return (M2LIB_HANDLE)static_cast<FileStorage*>(handle)->GetFileInfoByFileDataId(FileDataId);
}

M2LIB_HANDLE M2Lib::FileStorage_GetFileInfoByPartialPath(M2LIB_HANDLE handle, wchar_t const* Path)
{
	return (M2LIB_HANDLE)static_cast<FileStorage*>(handle)->GetFileInfoByPartialPath(Path);
}
