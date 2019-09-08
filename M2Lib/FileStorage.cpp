#include "FileStorage.h"
#include "FileSystem.h"
#include "Logger.h"
#include "StringHelpers.h"
#include "StringHash.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <locale>

const std::wstring M2Lib::FileStorage::DefaultMappingsPath = std::filesystem::current_path() / L"mappings";

M2Lib::FileInfo::FileInfo(uint32_t FileDataId, const char* Path)
{
	this->FileDataId = FileDataId;
	this->Path = Path;
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

char const* M2Lib::FileInfo_GetPath(M2LIB_HANDLE handle)
{
	return static_cast<FileInfo*>(handle)->Path.c_str();
}

bool M2Lib::FileStorage::ParseCsv(std::wstring const& Path)
{
	std::fstream in;
	in.open(Path, std::ios::in);
	if (in.fail())
		return false;

	uint32_t dwLocaleFlags = 0;
	std::string line;
	while (std::getline(in, line))
	{
		if (line.empty() || line.find(';') == std::string::npos)
			continue;

		std::istringstream stream(line);

		std::string tmp, fileName;
		std::getline(stream, tmp, ';');
		uint32_t FileDataId = std::stoul(tmp);
		std::getline(stream, fileName, ';');

		auto itr1 = fileInfosByFileDataId.find(FileDataId);
		if (itr1 != fileInfosByFileDataId.end())
		{
			sLogger.LogWarning("Duplicate storageRef entry '%u':'%s' (already used: '%u':'%s'), skipping", FileDataId, fileName.c_str(), itr1->second->FileDataId, itr1->second->Path.c_str());
			continue;
		}

		auto nameHash = CalcStringHash(fileName);
		auto itr2 = fileInfosByNameHash.find(nameHash);
		if (itr2 != fileInfosByNameHash.end())
		{
			sLogger.LogWarning("Duplicate storageRef entry '%u':'%s' (already used: '%u':'%s'), skipping (%llu vs %llu)", FileDataId, fileName.c_str(), itr2->second->FileDataId, itr2->second->Path.c_str(), nameHash, CalcStringHash(itr2->second->Path));
			continue;
		}

		auto info = new FileInfo(FileDataId, fileName.c_str());

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

	sLogger.LogInfo("Loaded %u mapping entries", fileInfosByFileDataId.size());
	
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
		sLogger.LogWarning("Mappings directory '%s' does not exist", StringHelpers::WStringToString(directory).c_str());
		return false;
	}

	sLogger.LogInfo("Loading mappings at '%s'", StringHelpers::WStringToString(directory).c_str());

	std::filesystem::directory_iterator itr;

	for (auto& p : std::filesystem::directory_iterator(directory))
	{
		if (p.path().extension().string() != ".csv")
			continue;

		sLogger.LogInfo("Loading mapping '%s'", p.path().filename().string().c_str());

		try
		{
			if (!ParseCsv(p.path().wstring()))
				sLogger.LogError("Failed to parse mapping file '%s'", p.path().filename().string().c_str());
		}
		catch (std::exception& e)
		{
			sLogger.LogError("Failed to parse mapping file '%s': %s", p.path().filename().string().c_str(), e.what());
		}
	}

	return true;
}

M2Lib::FileInfo const* M2Lib::FileStorage::GetFileInfoByPartialPath(std::string const & Name)
{
	LoadStorage();

	const auto normalizePath = [](std::string path) -> std::string
	{
		path = FileSystemA::NormalizePath(path);
		std::transform(path.begin(), path.end(), path.begin(), ::tolower);

		return path;
	};

	const auto NameCopy = normalizePath(Name);

	for (auto& itr : fileInfosByFileDataId)
	{
		if (normalizePath(itr.second->Path).find(NameCopy) != std::string::npos)
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

M2Lib::FileInfo const* M2Lib::FileStorage::GetFileInfoByPath(std::string const& Path)
{
	LoadStorage();

	uint64_t hash = CalcStringHash(Path);
	auto itr = fileInfosByNameHash.find(hash);
	if (itr != fileInfosByNameHash.end())
		return itr->second;

	return nullptr;
}

char const* M2Lib::FileStorage::PathInfo(uint32_t FileDataId)
{
	if (!FileDataId)
		return "<none>";

	auto info = GetFileInfoByFileDataId(FileDataId);
	if (!info)
		return "<not found in listfile>";

	return info->Path.c_str();
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

M2LIB_HANDLE M2Lib::FileStorage_GetFileInfoByPartialPath(M2LIB_HANDLE handle, char const* Path)
{
	return (M2LIB_HANDLE)static_cast<FileStorage*>(handle)->GetFileInfoByPartialPath(Path);
}
