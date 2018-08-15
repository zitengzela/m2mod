#include "Casc.h"
#include "FileSystem.h"
#include "Logger.h"
#include <CascLib.h>
#include <fstream>

UInt32 M2Lib::Casc::Magic = 'BLSF';

M2Lib::Casc::Casc(std::string const & StoragePath)
{
	this->StoragePath = StoragePath;

	BinListFile = FileSystemA::GetCurrentPath() + "\\" + "listfile.lsb";
	ListFile = FileSystemA::GetCurrentPath() + "\\" + "listfile.txt";
}

bool M2Lib::Casc::Load()
{
	sLogger.Log("Initializing CASC storage at '%s'", StoragePath.c_str());
	// Open the storage directory
	if (!CascOpenStorage(StoragePath.c_str(), 0, &hStorage))
	{
		sLogger.Log("Failed to load CASC storage: error %u", GetLastError());
		return false;
	}

	if (!FileSystemA::IsFile(BinListFile))
	{
		sLogger.Log("Listfile cache at %s not found, generating new one", BinListFile.c_str());
		if (!GenerateListFileCache())
			sLogger.Log("Error: Failed to generate listfile cache");
	}
	else if (!LoadListFileCache())
	{
		sLogger.Log("Failed to load listfile cache at %s, generating new one", BinListFile.c_str());
		if (!GenerateListFileCache())
			sLogger.Log("Error: Failed to generate listfile cache");
	}

	sLogger.Log("Finished CASC initialization");
	return true;
}

void M2Lib::Casc::Unload()
{
	if (hStorage != NULL)
		CascCloseStorage(hStorage);
	hStorage = NULL;

	filesByFileDataId.clear();
}

bool M2Lib::Casc::LoadListFileCache()
{
	filesByFileDataId.clear();

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
		FileStream.close();
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
	}

	FileStream.close();

	sLogger.Log("Loaded %u cached listfile entries", filesByFileDataId.size());

	return true;
}

bool M2Lib::Casc::GenerateListFileCache()
{
	filesByFileDataId.clear();

	sLogger.Log("Loading listfile at %s", ListFile.c_str());
	std::fstream in;
	in.open(ListFile, std::ios::in);
	if (in.fail())
	{
		sLogger.Log("Error: Failed to open listfile");
		return false;
	}

	std::fstream out;
	out.open(BinListFile, std::ios::out | std::ios::binary | std::ios::trunc);
	if (out.fail())
	{
		sLogger.Log("Error: Failed to open listfile cache for writing at %s", BinListFile.c_str());

		in.close();
		return false;
	}

	if (!IsLoaded())
		Load();

	UInt32 totalCount = 0;

	sLogger.Log("Parsing listfile...");

	DWORD dwLocaleFlags = 0;
	std::string s;
	while (std::getline(in, s))
	{
		++totalCount;
		UInt32 FileId = CascGetFileId(hStorage, s.c_str());
		if (!FileId || FileId == CASC_INVALID_ID)
			continue;

		filesByFileDataId[FileId] = s;
	}
	sLogger.Log("Parsing finished. %u valid entries of %u", filesByFileDataId.size(), totalCount);

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
	in.close();
	out.close();

	sLogger.Log("Writing finished");
	return true;
}
