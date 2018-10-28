#pragma once

#include "BaseTypes.h"
#include <string>
#include <map>

namespace M2Lib
{
	class Casc
	{
	private:
		static UInt32 Magic;

		void ClearCache() { filesByFileDataId.clear(); fileDataIdsByHash.clear(); cacheLoaded = false; }
		void ReleaseStorage();

		bool _ExtractFile(void* hFile, std::string const& OutPath);

		std::string StoragePath;
		bool releaseOnDereference = false;

		void* hStorage = NULL;
		SInt32 referenceCount = 0;

		bool cacheLoaded = false;
		std::map<UInt32, std::string> filesByFileDataId;
		std::map<UInt64, UInt32> fileDataIdsByHash;

	public:
		struct FileInfo
		{
			UInt32 FileDataId = -1;
			std::string Path;
		};

		static const std::string DefaultBinListfilePath;
		static const std::string DefaultListfilePath;

		Casc();

		void SetStoragePath(std::string const& StoragePath);
		void SetReleaseOnDereference(bool On);

		bool InitializeStorage();
		bool StorageInitialized() const { return hStorage != NULL; }
		
		bool LoadListFileCache(std::string const& ListfilePath);
		bool GenerateListFileCache(std::string const& ListfilePath);
		bool CacheLoaded() const { return cacheLoaded; }
		UInt32 GetCacheSize() const { return filesByFileDataId.size(); }

		bool ExtractFile(std::string const& File, std::string const& OutPath);
		bool ExtractFile(UInt32 FileDataId, std::string const& OutPath);

		FileInfo FindByPartialFileName(std::string const& Name);
		std::string GetFileByFileDataId(UInt32 FileDataId);
		UInt32 GetFileDataIdByFile(std::string const& File);
		static UInt64 CalculateHash(std::string const& File);

		void AddRefence();
		void RemoveReference();
	};
}
