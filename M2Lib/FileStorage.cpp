#include "FileStorage.h"
#include "FileSystem.h"
#include "Logger.h"
#include <fstream>
#include <algorithm>
#include <sstream>

const std::string M2Lib::FileStorage::DefaultListfilePath = FileSystemA::GetCurrentPath() + "\\" + "listfile.csv";

M2Lib::FileStorage::FileStorage()
{
}

bool M2Lib::FileStorage::LoadCSV(std::string const& Path)
{
	sLogger.LogInfo("Loading listfile at %s", Path.c_str());
	std::fstream in;
	in.open(Path, std::ios::in);
	if (in.fail())
	{
		sLogger.LogError("Error: Failed to open listfile");
		return false;
	}

	sLogger.LogInfo("Parsing listfile...");

	UInt32 dwLocaleFlags = 0;
	std::string line;
	while (std::getline(in, line))
	{
		std::istringstream stream(line);

		std::string tmp, fileName;
		std::getline(stream, tmp, ';');
		UInt32 FileDataId = std::stoul(tmp);
		std::getline(stream, fileName, ';');

		filesByFileDataId[FileDataId] = fileName;
		fileDataIdsByHash[CalculateHash(fileName)] = FileDataId;
	}
	in.close();
	sLogger.LogInfo("Parsing finished. %u total entries", filesByFileDataId.size());

	return true;
}

M2Lib::FileStorage::FileInfo M2Lib::FileStorage::FindByPartialFileName(std::string const & Name)
{
	auto NameCopy = Name;
	NameCopy = FileSystemA::NormalizePath(NameCopy);
	std::transform(NameCopy.begin(), NameCopy.end(), NameCopy.begin(), ::tolower);

	for (auto itr : filesByFileDataId)
	{
		std::string str = itr.second;
		str = FileSystemA::NormalizePath(str);
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

		if (str.find(NameCopy) != std::string::npos)
			return { itr.first, itr.second };
	}

	return { 0, "" };
}

std::string M2Lib::FileStorage::GetFileByFileDataId(UInt32 FileDataId)
{
	auto itr = filesByFileDataId.find(FileDataId);
	if (itr == filesByFileDataId.end())
		return "";

	return itr->second;
}

UInt32 M2Lib::FileStorage::GetFileDataIdByFile(std::string const& File)
{
	UInt64 hash = CalculateHash(File);
	auto itr = fileDataIdsByHash.find(hash);
	if (itr != fileDataIdsByHash.end())
		return itr->second;

	return 0;
}

UInt64 M2Lib::FileStorage::CalculateHash(std::string const & FileName)
{
	auto name = FileName;
	std::transform(name.begin(), name.end(), name.begin(), [](auto c) {return std::tolower(c); });

	name = FileSystemA::NormalizePath(name);

	static std::hash<std::string> hasher;

	return hasher(name);
}
