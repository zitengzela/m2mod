#include "Casc.h"
#include "FileSystem.h"
#include "Logger.h"
#include <CascLib.h>
#include <common/Common.h>
#include <CascCommon.h>
#include <fstream>
#include <algorithm>

UInt32 M2Lib::Casc::Magic = 'BLSF';

bool M2Lib::Casc::_ExtractFile(void* hFile, std::string const & OutPath)
{
	FILE* fp = NULL;
	DWORD dwBytesRead = 1;
	DWORD dwFilePos = CascSetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	BYTE Buffer[0x1000];

	// Create/open the dump file
	fopen_s(&fp, OutPath.c_str(), "wt");
	if (!fp)
		return false;

	// Read data as long as we can, write as long as we can
	while (dwBytesRead != 0)
	{
		// Read from the source file
		if (!CascReadFile(hFile, Buffer, sizeof(Buffer), &dwBytesRead))
		{
			fclose(fp);
			return false;
		}

		// Write to the destination file
		if (fwrite(Buffer, 1, dwBytesRead, fp) != dwBytesRead)
		{
			fclose(fp);
			return false;
		}
	}

	// Restore the file pointer
	CascSetFilePointer(hFile, dwFilePos, NULL, FILE_BEGIN);

	// Close the dump file
	fclose(fp);

	return true;
}

const std::string M2Lib::Casc::DefaultBinListfilePath = FileSystemA::GetCurrentPath() + "\\" + "listfile.lsb";
const std::string M2Lib::Casc::DefaultListfilePath = FileSystemA::GetCurrentPath() + "\\" + "listfile.txt";

M2Lib::Casc::Casc()
{
}

void M2Lib::Casc::SetStoragePath(std::string const& StoragePath)
{
	if (!_strcmpi(StoragePath.c_str(), this->StoragePath.c_str()))
		return;

	this->StoragePath = StoragePath;
	
	ReleaseStorage();
	ClearCache();
	sLogger.LogInfo("CASC storage path set to: %s", StoragePath.c_str());
}

void M2Lib::Casc::SetReleaseOnDereference(bool On)
{
	releaseOnDereference = On;
}

bool M2Lib::Casc::InitializeStorage()
{
	if (StorageInitialized())
		return true;

	sLogger.LogInfo("Initializing CASC storage at '%s'", StoragePath.c_str());
	// Open the storage directory
	if (!CascOpenStorage(StoragePath.c_str(), 0, &hStorage))
	{
		//sLogger.LogError("Failed to load CASC storage: error %u", GetLastError());
		sLogger.LogInfo("Failed to load CASC storage: error %u", GetLastError());
		return false;
	}

	sLogger.LogInfo("Finished CASC initialization");
	return true;
}

void M2Lib::Casc::ReleaseStorage()
{
	if (hStorage != NULL)
		CascCloseStorage(hStorage);
	hStorage = NULL;
}

bool M2Lib::Casc::LoadListFileCache(std::string const& ListfilePath)
{
	if (CacheLoaded())
		return true;

	if (!FileSystemA::IsFile(DefaultBinListfilePath))
	{
		sLogger.LogInfo("Listfile cache at %s not found, generating new one", DefaultBinListfilePath.c_str());
		if (!GenerateListFileCache(ListfilePath))
			sLogger.LogError("Error: Failed to generate listfile cache");
		return false;
	}

	std::fstream FileStream;
	FileStream.open(DefaultBinListfilePath, std::ios::in | std::ios::binary);
	if (FileStream.fail())
	{
		sLogger.LogError("Error: Failed to open listfile cache");
		return false;
	}

	UInt32 TestMagic;
	FileStream.read((char*)&TestMagic, sizeof(UInt32));
	if (TestMagic != REVERSE_CC(Magic))
	{
		sLogger.LogError("Error: Usupported listfile cache magic");
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

	sLogger.LogInfo("Loaded %u cached listfile entries", filesByFileDataId.size());

	cacheLoaded = true;
	return true;
}

bool M2Lib::Casc::GenerateListFileCache(std::string const& ListfilePath)
{
	ClearCache();

	auto Path = ListfilePath;

	if (Path.empty())
		Path = DefaultListfilePath;

	sLogger.LogInfo("Loading listfile at %s", Path.c_str());
	std::fstream in;
	in.open(Path, std::ios::in);
	if (in.fail())
	{
		sLogger.LogError("Error: Failed to open listfile");
		return false;
	}

	if (!InitializeStorage())
	{
		sLogger.LogError("Failed to generate listfile cache: can't be validated without CASC storage");
		return false;
	}

	std::fstream out;
	out.open(DefaultBinListfilePath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (out.fail())
	{
		sLogger.LogError("Error: Failed to open listfile cache for writing at %s", DefaultBinListfilePath.c_str());
		return false;
	}

	UInt32 totalCount = 0;

	sLogger.LogInfo("Parsing listfile...");

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
	sLogger.LogInfo("Parsing finished. %u valid entries of %u", filesByFileDataId.size(), totalCount);

	cacheLoaded = true;

	sLogger.LogInfo("Writing listfile cache file...");
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

	sLogger.LogInfo("Writing finished");
	return true;
}

bool M2Lib::Casc::ExtractFile(std::string const & File, std::string const& OutPath)
{
	if (!InitializeStorage())
		return false;

	HANDLE hFile;
	if (!CascOpenFile(hStorage, File.c_str(), 0, CASC_OPEN_BY_NAME | CASC_STRICT_DATA_CHECK, &hFile))
		return false;

	if (!_ExtractFile(hFile, OutPath))
	{
		CascCloseFile(hFile);
		return false;
	}

	CascCloseFile(hFile);
	return true;
}

bool M2Lib::Casc::ExtractFile(UInt32 FileDataId, std::string const& OutPath)
{
	if (!InitializeStorage())
		return false;

	std::string File = GetFileByFileDataId(FileDataId);
	if (File.empty())
		return false;

	HANDLE hFile;
	if (!CascOpenFile(hStorage, File.c_str(), 0, CASC_OPEN_BY_NAME | CASC_STRICT_DATA_CHECK, &hFile))
		return false;

	if (!_ExtractFile(hFile, OutPath))
	{
		CascCloseFile(hFile);
		return false;
	}

	CascCloseFile(hFile);
	return true;
}

std::string M2Lib::Casc::FindByPartialFileName(std::string const & Name)
{
	if (!LoadListFileCache(""))
		return "";

	auto NameCopy = Name;
	std::transform(NameCopy.begin(), NameCopy.end(), NameCopy.begin(), ::tolower);

	for (auto itr : filesByFileDataId)
	{
		std::string str = itr.second;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

		if (str.find(NameCopy) != std::string::npos)
			return str;
	}

	return "";
}

std::string M2Lib::Casc::GetFileByFileDataId(UInt32 FileDataId)
{
	if (!LoadListFileCache(""))
		return "";

	auto itr = filesByFileDataId.find(FileDataId);
	if (itr == filesByFileDataId.end())
		return "";

	return itr->second;
}

UInt32 M2Lib::Casc::GetFileDataIdByFile(std::string const & File)
{
	if (!cacheLoaded)
		LoadListFileCache("");

	UInt64 hash = CalcFileNameHash(File.c_str());
	if (cacheLoaded)
	{
		auto itr = fileDataIdsByHash.find(hash);
		if (itr != fileDataIdsByHash.end())
			return itr->second;
	}

	if (!InitializeStorage())
		return 0;

	UInt32 FileDataId = CascGetFileId(hStorage, File.c_str());
	if (!FileDataId || FileDataId == CASC_INVALID_ID)
	{
		fileDataIdsByHash[hash] = 0;
		return 0;
	}

	fileDataIdsByHash[hash] = FileDataId;
	return FileDataId;
}

UInt64 M2Lib::Casc::CalculateHash(std::string const & FileName)
{
	return CalcFileNameHash(FileName.c_str());
}

void M2Lib::Casc::AddRefence()
{
	++referenceCount;
}

void M2Lib::Casc::RemoveReference()
{
	--referenceCount;
	if (referenceCount <= 0)
	{
		referenceCount = 0;
		ReleaseStorage();
	}
}
