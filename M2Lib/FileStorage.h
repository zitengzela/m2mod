#pragma once

#include "BaseTypes.h"
#include <string>
#include <map>

namespace M2Lib
{
	struct FileInfo
	{
		FileInfo(uint32_t FileDataId, const char* Path, bool IsCustom);

		uint32_t FileDataId = 0;
		std::string Path;
		bool IsCustom = false;
	};

	class FileStorage
	{
		void ClearStorage() { fileInfosByFileDataId.clear(); fileInfosByNameHash.clear(); }

		std::map<uint32_t, FileInfo const*> fileInfosByFileDataId;
		std::map<uint64_t, FileInfo const*> fileInfosByNameHash;

		FileStorage() = default;
		bool ParseCsv(std::string const& Path, bool IsCustom);

	public:
		static FileStorage* GetInstance()
		{
			static FileStorage* storage = new FileStorage();
			static bool loadFailed = false;
			if (!loadFailed && !storage->Loaded())
				loadFailed = !storage->LoadStorage(DefaultListfilePath);

			return storage;
		}

		~FileStorage();

		bool LoadListFileAddons(uint32_t& totalAddons);
		bool LoadStorage(std::string const& ListfilePath);

		static const std::string DefaultListfilePath;
		static const std::string ListfileAddonsPath;

		bool Loaded() const { return GetStorageSize() > 0; }
		uint32_t GetStorageSize() const { return fileInfosByFileDataId.size(); }

		FileInfo const* GetFileInfoByPartialPath(std::string const& Name) const;
		FileInfo const* GetFileInfoByFileDataId(uint32_t FileDataId) const;
		FileInfo const* GetFileInfoByPath(std::string const& Path) const;
		static uint64_t CalculateHash(std::string const& Path);
		static char const* PathInfo(uint32_t FileDataId);
	};

	M2LIB_API void __cdecl LoadFileStorage(const char* path);
	M2LIB_API FileInfo const* __cdecl GetFileInfoByFileDataId(uint32_t FileDataId);
	M2LIB_API FileInfo const* __cdecl GetFileInfoByPartialPath(char const* Path);

	M2LIB_API uint32_t __cdecl FileInfo_GetFileDataId(FileInfo* pointer);
	M2LIB_API char const* __cdecl FileInfo_GetPath(FileInfo* pointer);
	M2LIB_API bool __cdecl FileInfo_IsCustom(FileInfo* pointer);
}
