#pragma once

#include "M2Types.h"

namespace M2Lib
{
	class ImportSettings
	{
	public:
		bool MergeBones = true;
		bool MergeAttachments = true;
		bool MergeCameras = true;
		bool FixSeams = false;
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
	};
}
