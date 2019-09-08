#pragma once

#include "BaseTypes.h"
#include <string>
#include <map>
#include <unordered_map>

namespace M2Lib
{
	struct FileInfo
	{
		FileInfo(uint32_t FileDataId, const char* Path);

		uint32_t FileDataId = 0;
		std::string Path;
	};

	class FileStorage
	{
		bool loadFailed = false;
		void ClearStorage();
		bool LoadMappings();

		std::map<uint32_t, FileInfo const*> fileInfosByFileDataId;
		std::map<uint64_t, FileInfo const*> fileInfosByNameHash;
		std::wstring mappingsDirectory;

		bool ParseCsv(std::wstring const& Path);

		static const std::wstring DefaultMappingsPath;

	public:
		FileStorage(std::wstring const& mappingsDirectory);
		void SetMappingsDirectory(std::wstring const& mappingsDirectory);

		~FileStorage();

		bool LoadStorage();
		void ResetLoadFailed();

		bool Loaded() const { return GetStorageSize() > 0; }
		uint32_t GetStorageSize() const { return fileInfosByFileDataId.size(); }

		FileInfo const* GetFileInfoByPartialPath(std::string const& Name);
		FileInfo const* GetFileInfoByFileDataId(uint32_t FileDataId);
		FileInfo const* GetFileInfoByPath(std::string const& Path);
		char const* PathInfo(uint32_t FileDataId);
	};

	class StorageManager
	{
	private:
		std::unordered_map<uint64_t, FileStorage*> storages;

	public:

		static StorageManager* GetInstance()
		{
			static StorageManager instance;

			return &instance;
		}

		FileStorage* GetStorage(std::wstring const& mappingDirectory);
		~StorageManager();
	};

	M2LIB_API M2LIB_HANDLE __cdecl FileStorage_Get(const wchar_t* mappingsDirectory);
	M2LIB_API void __cdecl FileStorage_SetMappingsDirectory(M2LIB_HANDLE handle, const wchar_t* mappingsDirectory);
	M2LIB_API M2LIB_HANDLE __cdecl FileStorage_GetFileInfoByFileDataId(M2LIB_HANDLE handle, uint32_t FileDataId);
	M2LIB_API M2LIB_HANDLE __cdecl FileStorage_GetFileInfoByPartialPath(M2LIB_HANDLE handle, char const* Path);

	M2LIB_API uint32_t __cdecl FileInfo_GetFileDataId(M2LIB_HANDLE handle);
	M2LIB_API char const* __cdecl FileInfo_GetPath(M2LIB_HANDLE handle);
}
