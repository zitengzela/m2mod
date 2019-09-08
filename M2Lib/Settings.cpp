#include "Settings.h"
#include <fstream>

void M2Lib::Settings::setOutputDirectory(const wchar_t* directory)
{
	wcscpy_s(OutputDirectory, sizeof(OutputDirectory) / sizeof(wchar_t), directory);
}

void M2Lib::Settings::setWorkingDirectory(const wchar_t* directory)
{
	wcscpy_s(WorkingDirectory, sizeof(WorkingDirectory) / sizeof(wchar_t), directory);
}

void M2Lib::Settings::setMappingsDirectory(const wchar_t* directory)
{
	wcscpy_s(MappingsDirectory, sizeof(MappingsDirectory) / sizeof(wchar_t), directory);
}

void M2Lib::Settings::operator=(Settings const& other)
{
	setOutputDirectory(other.OutputDirectory);
	setWorkingDirectory(other.WorkingDirectory);
	setMappingsDirectory(other.MappingsDirectory);
	ForceLoadExpansion = other.ForceLoadExpansion;
	MergeBones = other.MergeBones;
	MergeAttachments = other.MergeAttachments;
	MergeCameras = other.MergeCameras;
	FixSeams = other.FixSeams;
	FixEdgeNormals = other.FixEdgeNormals;
	IgnoreOriginalMeshIndexes = other.IgnoreOriginalMeshIndexes;
	FixAnimationsTest = other.FixAnimationsTest;
}
