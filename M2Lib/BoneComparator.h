#pragma once
#include "M2.h"
#include <map>
#include <set>

namespace M2Lib
{
	namespace BoneComparator
	{
		enum class CompareStatus
		{
			Identical = 0,
			IdenticalWithinThreshold = 1,
			Differ = 2,
		};

		class Candidates
		{
		public:
			void AddCandidate(uint32_t BoneId);

			std::map<uint32_t, float> GetWeightedCandidates();

			uint32_t Size() const { return BoneUsage.size(); }

		private:
			std::map<uint32_t, uint32_t> BoneUsage;
		};

		std::map<uint32_t, std::map<uint32_t, float>> Diff(M2* oldM2, M2* newM2, bool CompareTextures);

		CompareStatus GetDifferenceStatus(std::map<uint32_t, std::map<uint32_t, float>> const& WeightedResult, float weightThreshold);
	}
}
