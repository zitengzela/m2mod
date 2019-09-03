#include "Settings.h"
#include <fstream>

void M2Lib::GlobalSettings::setOutputDirectory(const char* directory)
{
	strcpy_s(OutputDirectory, sizeof(OutputDirectory), directory);
}

void M2Lib::GlobalSettings::setWorkingDirectory(const char* directory)
{
	strcpy_s(WorkingDirectory, sizeof(WorkingDirectory), directory);
}

void M2Lib::GlobalSettings::operator=(GlobalSettings const& other)
{
	setOutputDirectory(other.OutputDirectory);
	setWorkingDirectory(other.WorkingDirectory);
	ForceLoadExpansion = other.ForceLoadExpansion;
	MergeBones = other.MergeBones;
	MergeAttachments = other.MergeAttachments;
	MergeCameras = other.MergeCameras;
	FixSeams = other.FixSeams;
	FixEdgeNormals = other.FixEdgeNormals;
	IgnoreOriginalMeshIndexes = other.IgnoreOriginalMeshIndexes;
	FixAnimationsTest = other.FixAnimationsTest;
}
