#include "Settings.h"
#include <fstream>

void M2Lib::Settings::setOutputDirectory(const char* directory)
{
	strcpy_s(OutputDirectory, sizeof(OutputDirectory), directory);
}

void M2Lib::Settings::setWorkingDirectory(const char* directory)
{
	strcpy_s(WorkingDirectory, sizeof(WorkingDirectory), directory);
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
