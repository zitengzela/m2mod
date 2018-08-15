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

		bool LoadListFileCache();

		std::string StoragePath;
		std::string BinListFile;
		std::string ListFile;

		void* hStorage = NULL;

		bool cacheLoaded = false;
		std::map<UInt32, std::string> filesByFileDataId;

	public:
		Casc(std::string const& StoragePath);
		bool Load();
		void Unload();
		bool IsLoaded() const { return hStorage != NULL; }
		bool GenerateListFileCache();
		UInt32 GetCacheSize() const { return filesByFileDataId.size(); }

		std::string GetFileById(UInt32 FileDataId);
	};
}
