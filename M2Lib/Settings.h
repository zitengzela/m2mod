#pragma once

#include "M2Types.h"
#include <string>

namespace M2Lib
{
	class GlobalSettings
	{
	public:
		bool MergeBones = true;
		bool MergeAttachments = true;
		bool MergeCameras = true;
		bool FixSeams = false;
		bool FixEdgeNormals = false;
		bool IgnoreOriginalMeshIndexes = false;
		Expansion ForceLoadExpansion = Expansion::None;

		bool FixAnimationsTest = false;
		bool DisplayErrors = true;

		std::string WorkingDirectory;
		std::string OutputDirectory;
	};
}
