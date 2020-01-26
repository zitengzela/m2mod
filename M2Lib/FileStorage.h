#pragma once

#include "BaseTypes.h"
#include <string>
#include <map>
#include <unordered_map>

namespace std {
	namespace filesystem {
		class path;
	}
}

namespace M2Lib
{
	struct FileInfo
	{
		FileInfo(uint32_t FileDataId, const wchar_t* Path);

		uint32_t FileDataId = 0;
		std::wstring Path;
	};

	class FileStorage
	{
		bool loadFailed = false;
		uint32_t MaxFileDataId = 0;
		void ClearStorage();
		bool LoadMappings();

		std::map<uint32_t, FileInfo const*> fileInfosByFileDataId;
		std::map<uint64_t, FileInfo const*> fileInfosByNameHash;
		std::wstring mappingsDirectory;

		bool ParseCsv(std::wstring const& Path);

	public:
		FileStorage(std::wstring const& mappingsDirectory);
		void SetMappingsDirectory(std::wstring const& mappingsDirectory);
		void AddRecord(FileInfo const* record);
		static std::filesystem::path DetectWorkingDirectory(std::filesystem::path fullPath, std::filesystem::path relativePath);

		~FileStorage();

		bool LoadStorage();
		void ResetLoadFailed();

		bool Loaded() const { return GetStorageSize() > 0; }
		uint32_t GetStorageSize() const { return fileInfosByFileDataId.size(); }
		uint32_t GetMaxFileDataId() const { return MaxFileDataId; }

		FileInfo const* GetFileInfoByPartialPath(std::wstring const& Name);
		FileInfo const* GetFileInfoByFileDataId(uint32_t FileDataId);
		FileInfo const* GetFileInfoByPath(std::wstring const& Path);
		wchar_t const* PathInfo(uint32_t FileDataId);

		static const std::wstring DefaultMappingsPath;
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

		~StorageManager();

		FileStorage* GetStorage(std::wstring const& mappingDirectory);
		void Clear();
	};

	M2LIB_API M2LIB_HANDLE __cdecl FileStorage_Get(const wchar_t* mappingsDirectory);
	M2LIB_API void __cdecl FileStorage_Clear();
	M2LIB_API void __cdecl FileStorage_SetMappingsDirectory(M2LIB_HANDLE handle, const wchar_t* mappingsDirectory);
	M2LIB_API M2LIB_HANDLE __cdecl FileStorage_GetFileInfoByFileDataId(M2LIB_HANDLE handle, uint32_t FileDataId);
	M2LIB_API M2LIB_HANDLE __cdecl FileStorage_GetFileInfoByPartialPath(M2LIB_HANDLE handle, wchar_t const* Path);

	M2LIB_API uint32_t __cdecl FileInfo_GetFileDataId(M2LIB_HANDLE handle);
	M2LIB_API wchar_t const* __cdecl FileInfo_GetPath(M2LIB_HANDLE handle);
}
