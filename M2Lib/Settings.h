#pragma once

#include "M2Types.h"
#include <string>

namespace M2Lib
{
	class ImportSettings
	{
	public:
		bool MergeBones = true;
		bool MergeAttachments = true;
		bool MergeCameras = true;
		bool FixSeams = false;

		bool FixAnimationsTest = false;
	};

	class ExportSettings
	{
	public:
		Expansion ForceExpansion = Expansion::None;
	};

	class GlobalSettings
	{
	public:
		ExportSettings ExportSettings;
		ImportSettings ImportSettings;

		std::string WowPath;
	};
}
