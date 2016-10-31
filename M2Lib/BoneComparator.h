#pragma once
#include "M2.h"
#include <set>
#include <sstream>
#include <algorithm>

namespace M2Lib
{
	bool floatEq(float a, float b);
	bool boneEq(M2Element::CElement_Bone& bone1, M2Element::CElement_Bone& bone2);

	class BoneStorage : public std::map<int, std::set<int>>
	{
	public:
		bool Equal(int newBone, int oldBone) const;
		void EraseData(BoneStorage& data);
		void EraseExcept(int boneId, int exceptBoneId);
		std::string CheckAmbiguity();
		std::string GetDiff() const;
	};

	class BoneComparator
	{
	private:
		M2* oldM2;
		M2* newM2;
		BoneStorage boneMap;

		bool prepared = false;
		void Prepare();

	public:
		BoneComparator(M2* oldM2, M2* newM2);
		std::string ShowDifference();
		EError Save(char const* fileName);
	};
}
