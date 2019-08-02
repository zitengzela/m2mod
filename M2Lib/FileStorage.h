#pragma once

#include "BaseTypes.h"
#include <string>
#include <map>

namespace M2Lib
{
	struct FileInfo
	{
		UInt32 FileDataId = 0;
		std::string Path;

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
	private:
		void ClearStorage() { fileInfosByFileDataId.clear(); fileInfosByNameHash.clear(); }

		std::map<UInt32, FileInfo> fileInfosByFileDataId;
		std::map<UInt64, FileInfo> fileInfosByNameHash;

		FileStorage();
		bool ParseCsv(std::string const& Path, bool CheckExisting = false);

	public:
		static FileStorage* GetInstance()
		{
			static FileStorage* storage = new FileStorage();
			if (!storage->Loaded())
				storage->LoadStorage(DefaultListfilePath);

			return storage;
		}

		bool LoadListFileAddons(UInt32& totalAddons);
		bool LoadStorage(std::string const& ListfilePath);

		static const std::string DefaultListfilePath;
		static const std::string ListfileAddonsPath;

		bool Loaded() const { return GetStorageSize() > 0; }
		UInt32 GetStorageSize() const { return fileInfosByFileDataId.size(); }

		FileInfo const& GetFileInfoByPartialPath(std::string const& Name);
		FileInfo const& GetFileInfoByFileDataId(UInt32 FileDataId);
		FileInfo const& GetFileInfoByPath(std::string const& Path);
		static UInt64 CalculateHash(std::string const& Path);
	};
}
