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

		std::string StoragePath;
		std::string BinListFile;
		std::string ListFile;

		void* hStorage = NULL;

		bool cacheLoaded = false;
		std::map<UInt32, std::string> filesByFileDataId;
		std::map<UInt64, UInt32> fileDataIdsByHash;

	public:
		Casc();

		void SetStoragePath(std::string const& StoragePath);

		void Unload();

		bool InitializeStorage();
		bool StorageInitialized() const { return hStorage != NULL; }
		
		bool LoadListFileCache();
		bool GenerateListFileCache();
		bool CacheLoaded() const { return cacheLoaded; }
		UInt32 GetCacheSize() const { return filesByFileDataId.size(); }

		std::string GetFileByFileDataId(UInt32 FileDataId);
		UInt32 GetFileDataIdByFile(std::string const& File);
	};
}
