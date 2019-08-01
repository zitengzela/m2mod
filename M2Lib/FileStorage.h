#pragma once

#include "BaseTypes.h"
#include <string>
#include <map>

namespace M2Lib
{
	class FileStorage
	{
	private:
		void ClearCache() { filesByFileDataId.clear(); fileDataIdsByHash.clear(); }

		std::map<UInt32, std::string> filesByFileDataId;
		std::map<UInt64, UInt32> fileDataIdsByHash;

		FileStorage();

	public:
		struct FileInfo
		{
			UInt32 FileDataId = -1;
			std::string Path;
		};

		static FileStorage* GetInstance()
		{
			static FileStorage* casc = new FileStorage();
			if (!casc->Loaded())
				casc->LoadCSV(DefaultListfilePath);

			return casc;
		}

		bool LoadCSV(std::string const& Path);

		static const std::string DefaultListfilePath;
	
		bool Loaded() const { return GetStorageSize() > 0; }
		UInt32 GetStorageSize() const { return filesByFileDataId.size(); }

		FileInfo FindByPartialFileName(std::string const& Name);
		std::string GetFileByFileDataId(UInt32 FileDataId);
		UInt32 GetFileDataIdByFile(std::string const& File);
		static UInt64 CalculateHash(std::string const& File);
	};
}
