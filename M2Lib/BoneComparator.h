#pragma once
#include "M2.h"
#include <map>
#include <set>

namespace M2Lib
{
	class BoneComparator
	{
	private:
		M2* oldM2;
		M2* newM2;

	public:
		BoneComparator(M2* oldM2, M2* newM2);
		std::map<unsigned int, std::set<unsigned int>> Diff();
	};
}
