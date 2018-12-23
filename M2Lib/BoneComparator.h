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
			void AddCandidate(unsigned int BoneId);

			std::map<unsigned int, float> GetWeightedCandidates();

			unsigned int Size() const { return BoneUsage.size(); }

		private:
			std::map<unsigned int, unsigned int> BoneUsage;
		};

		std::map<unsigned int, std::map<unsigned int, float>> Diff(M2* oldM2, M2* newM2);

		CompareStatus GetDifferenceStatus(std::map<unsigned int, std::map<unsigned int, float>> const& WeightedResult, float weightThreshold);
	}
}
