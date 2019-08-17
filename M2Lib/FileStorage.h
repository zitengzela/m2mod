#pragma once

#include "BaseTypes.h"
#include <string>
#include <map>

namespace M2Lib
{
	struct FileInfo
	{
		uint32_t FileDataId = 0;
		std::string Path;
		bool IsCustom = false;

		static const FileInfo Empty;

		bool operator==(const FileInfo& other) const
		{
			return FileDataId == other.FileDataId && Path == other.Path;
		}

		explicit operator bool() const
		{
			return !IsEmpty();
		}

		bool IsEmpty() const { return *this == Empty; }
	};

	class FileStorage
	{
		void ClearStorage() { fileInfosByFileDataId.clear(); fileInfosByNameHash.clear(); }

		std::map<uint32_t, FileInfo> fileInfosByFileDataId;
		std::map<uint64_t, FileInfo> fileInfosByNameHash;

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

		bool LoadListFileAddons(uint32_t& totalAddons);
		bool LoadStorage(std::string const& ListfilePath);

		static const std::string DefaultListfilePath;
		static const std::string ListfileAddonsPath;

		bool Loaded() const { return GetStorageSize() > 0; }
		uint32_t GetStorageSize() const { return fileInfosByFileDataId.size(); }

		FileInfo const& GetFileInfoByPartialPath(std::string const& Name);
		FileInfo const& GetFileInfoByFileDataId(uint32_t FileDataId);
		FileInfo const& GetFileInfoByPath(std::string const& Path);
		static uint64_t CalculateHash(std::string const& Path);
		static std::string PathInfo(uint32_t FileDataId);
	};
}
