#pragma once

#include "BaseTypes.h"
#include "M2Types.h"
#include <string>

namespace M2Lib
{
#pragma pack(push, 1)
	struct M2LIB_API_CLASS GlobalSettings
	{
		char OutputDirectory[1024];
		char WorkingDirectory[1024];
		Expansion ForceLoadExpansion = Expansion::None;
		bool MergeBones = true;
		bool MergeAttachments = true;
		bool MergeCameras = true;
		bool FixSeams = false;
		bool FixEdgeNormals = true;
		bool IgnoreOriginalMeshIndexes = false;
		bool FixAnimationsTest = false;

		void setOutputDirectory(const char* directory);
		void setWorkingDirectory(const char* directory);

		GlobalSettings()
		{
			memset(OutputDirectory, 0, sizeof(OutputDirectory));
			memset(WorkingDirectory, 0, sizeof(WorkingDirectory));
		}

		void operator=(GlobalSettings const& other);
	};

	ASSERT_SIZE(GlobalSettings, 2048 + 4 + 7);

#pragma pack(pop)

}
