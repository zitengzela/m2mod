#include "Casc.h"
#include "FileSystem.h"
#include "Logger.h"
#include <CascLib.h>
#include <common/Common.h>
#include <fstream>

UInt32 M2Lib::Casc::Magic = 'BLSF';

M2Lib::Casc::Casc()
{
	BinListFile = FileSystemA::GetCurrentPath() + "\\" + "listfile.lsb";
	ListFile = FileSystemA::GetCurrentPath() + "\\" + "listfile.txt";
}

void M2Lib::Casc::SetStoragePath(std::string const& StoragePath)
{
	if (!_strcmpi(StoragePath.c_str(), this->StoragePath.c_str()))
		return;

	this->StoragePath = StoragePath;
	Unload();
}

bool M2Lib::Casc::InitializeStorage()
{
	sLogger.Log("Initializing CASC storage at '%s'", StoragePath.c_str());
	// Open the storage directory
	if (!CascOpenStorage(StoragePath.c_str(), 0, &hStorage))
	{
		sLogger.Log("Failed to load CASC storage: error %u", GetLastError());
		return false;
	}

	sLogger.Log("Finished CASC initialization");
	return true;
}

void M2Lib::Casc::Unload()
{
	if (hStorage != NULL)
		CascCloseStorage(hStorage);
	hStorage = NULL;

	ClearCache();
}

bool M2Lib::Casc::LoadListFileCache()
{
	ClearCache();

	if (!FileSystemA::IsFile(BinListFile))
	{
		sLogger.Log("Listfile cache at %s not found, generating new one", BinListFile.c_str());
		if (!GenerateListFileCache())
			sLogger.Log("Error: Failed to generate listfile cache");
		return false;
	}

	std::fstream FileStream;
	FileStream.open(BinListFile, std::ios::in | std::ios::binary);
	if (FileStream.fail())
	{
		sLogger.Log("Error: Failed to open listfile cache");
		return false;
	}

	UInt32 TestMagic;
	FileStream.read((char*)&TestMagic, sizeof(UInt32));
	if (TestMagic != REVERSE_CC(Magic))
	{
		sLogger.Log("Error: Usupported listfile cache magic");
		return false;
	}

	while (!FileStream.eof())
	{
		UInt32 FileDataId;
		UInt32 NameLen;
		std::string Name;

		FileStream.read((char*)&FileDataId, sizeof(UInt32));
		FileStream.read((char*)&NameLen, sizeof(UInt32));

		Name.resize(NameLen);
		FileStream.read((char*)Name.data(), NameLen);
		
		filesByFileDataId[FileDataId] = Name;
		fileDataIdsByHash[CalcFileNameHash(Name.c_str())] = FileDataId;
	}

	sLogger.Log("Loaded %u cached listfile entries", filesByFileDataId.size());

	cacheLoaded = true;
	return true;
}

bool M2Lib::Casc::GenerateListFileCache()
{
	ClearCache();

	sLogger.Log("Loading listfile at %s", ListFile.c_str());
	std::fstream in;
	in.open(ListFile, std::ios::in);
	if (in.fail())
	{
		sLogger.Log("Error: Failed to open listfile");
		return false;
	}

	if (!StorageInitialized())
	{
		if (!InitializeStorage())
		{
			sLogger.Log("Failed to generate listfile cache: can't be validated without CASC storage");
			return false;
		}
	}

	std::fstream out;
	out.open(BinListFile, std::ios::out | std::ios::binary | std::ios::trunc);
	if (out.fail())
	{
		sLogger.Log("Error: Failed to open listfile cache for writing at %s", BinListFile.c_str());
		return false;
	}

	UInt32 totalCount = 0;

	sLogger.Log("Parsing listfile...");

	DWORD dwLocaleFlags = 0;
	std::string s;
	while (std::getline(in, s))
	{
		++totalCount;
		UInt32 FileDataId = CascGetFileId(hStorage, s.c_str());
		if (!FileDataId || FileDataId == CASC_INVALID_ID)
			continue;

		filesByFileDataId[FileDataId] = s;
		fileDataIdsByHash[CalcFileNameHash(s.c_str())] = FileDataId;
	}
	sLogger.Log("Parsing finished. %u valid entries of %u", filesByFileDataId.size(), totalCount);

	cacheLoaded = true;

	sLogger.Log("Writing listfile cache file...");
	UInt32 _magic = REVERSE_CC(Magic);
	out.write((char const*)&_magic, sizeof(_magic));
	for (auto const& info : filesByFileDataId)
	{
		out.write((char const*)&info.first, sizeof(UInt32));
		UInt32 len = info.second.length();
		out.write((char const*)&len, sizeof(UInt32));
		out.write(info.second.c_str(), len);
	}

	out.close();

	sLogger.Log("Writing finished");
	return true;
}

std::string M2Lib::Casc::GetFileByFileDataId(UInt32 FileDataId)
{
	if (!cacheLoaded)
		if (!LoadListFileCache())
			return "";

	auto itr = filesByFileDataId.find(FileDataId);
	if (itr == filesByFileDataId.end())
		return "";

	return itr->second;
}

UInt32 M2Lib::Casc::GetFileDataIdByFile(std::string const & File)
{
	if (!cacheLoaded)
		LoadListFileCache();

	if (cacheLoaded)
	{
		auto itr = fileDataIdsByHash.find(CalcFileNameHash(File.c_str()));
		if (itr != fileDataIdsByHash.end())
			return itr->second;
	}

	if (!StorageInitialized())
		if (!InitializeStorage())
			return 0;

	UInt32 FileDataId = CascGetFileId(hStorage, File.c_str());
	if (!FileDataId || FileDataId == CASC_INVALID_ID)
		return 0;

	return FileDataId;
}

UInt64 M2Lib::Casc::CalculateHash(std::string const & FileName)
{
	return CalcFileNameHash(FileName.c_str());
}
