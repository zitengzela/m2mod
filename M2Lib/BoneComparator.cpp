#include "BoneComparator.h"
#include "DataBinary.h"

bool M2Lib::floatEq(float a, float b)
{
	return std::fabsf(a - b) < 1e-5;
}

bool M2Lib::boneEq(M2Element::CElement_Bone& bone1, M2Element::CElement_Bone& bone2)
{
	return floatEq(bone1.Position[0], bone2.Position[0]) &&
		floatEq(bone1.Position[1], bone2.Position[1]) &&
		floatEq(bone1.Position[2], bone2.Position[2]) &&
		bone1.BoneLookupID == bone2.BoneLookupID &&
		bone1.Unknown[0] == bone2.Unknown[0] &&
		bone1.Unknown[1] == bone2.Unknown[1];
}

bool M2Lib::BoneStorage::Equal(int newBone, int oldBone) const
{
	auto itr = find(newBone);
	if (itr == end())
		return false;

	return itr->second.find(oldBone) != itr->second.end();
}

void M2Lib::BoneStorage::EraseData(BoneStorage& data)
{
	for (auto itr : data)
	{
		if (find(itr.first) == end())
			continue;

		for (auto id : itr.second)
			at(itr.first).erase(id);
	}
}

void M2Lib::BoneStorage::EraseExcept(int boneId, int exceptBoneId)
{
	auto itr = find(boneId);
	if (itr == end())
		return;

	auto set = itr->second;
	auto size = set.size();
	for (auto itr2 = set.begin(); itr2 != set.end();)
	{
		if (*itr2 == exceptBoneId)
			++itr2;
		else
		{
			set.erase(*itr2);
			itr2 = set.begin();
		}
	}
}

#define EOL "\r\n"
std::string M2Lib::BoneStorage::CheckAmbiguity()
{
	std::stringstream ss;
	for (auto& itr : *this)
	{
		if (itr.second.size() == 0)
			ss << "Bone #" << itr.first << " has no matches" << EOL;
		else if (itr.second.size() > 1)
		{
			ss << "Bone #" << itr.first << " has more than 1 match:";
			for (auto& oldBone : itr.second)
				ss << " " << oldBone;
			ss << EOL;
		}
	}

	for (auto& itr : *this)
	{
		for (auto& oldBone : itr.second)
		{
			for (auto& itr2 : *this)
			{
				if (itr.first == itr2.first)
					continue;

				if (itr2.second.find(oldBone) != itr2.second.end())
					ss << "Old bone #" << oldBone << " is equal both to new bone #" << itr.first << " and new bone #" << itr2.first << EOL;
			}
		}
	}

	return ss.str();
}

std::string M2Lib::BoneStorage::GetDiff() const
{
	std::stringstream ss;
	for (auto& itr : *this)
	{
		if (itr.second.size() == 0)
			ss << "New bone #" << itr.first << "\r\n";
		else if (itr.second.size() == 1)
		{
			if (itr.first != *itr.second.begin())
				ss << "New bone #" << itr.first << " is old bone #" << *itr.second.begin() << "\r\n";
		}
		else
		{
			ss << "Could not find match for new bone #" << itr.first << ", possible candidates: ";
			for (auto oldBone : itr.second)
				ss << " " << oldBone;
			ss << EOL;
		}
	}

	return ss.str();
}

M2Lib::BoneComparator::BoneComparator(M2* oldM2, M2* newM2)
{
	this->oldM2 = oldM2;
	this->newM2 = newM2;
}

void M2Lib::BoneComparator::Prepare()
{
	if (prepared)
		return;

	prepared = true;

	using namespace M2Element;
	auto bones71 = newM2->Elements[EElement_Bone].asVector<CElement_Bone>();
	auto bones = oldM2->Elements[EElement_Bone].asVector<CElement_Bone>();

	for (UInt32 i = 0; i < bones71.size(); ++i)
	{
		auto& boneNew = bones71[i];
		for (UInt32 j = 0; j < bones.size(); ++j)
		{
			auto& boneOld = bones[j];
			if (boneEq(boneNew, boneOld))
			{
				boneMap[i].insert(j);
			}
		}
	}


	BoneStorage toErase;
	for (auto& itr : boneMap)
	{
		auto boneNewId = itr.first;
		auto& newBone = bones71[boneNewId];
		if (newBone.ParentBone == -1)
			continue;

		for (auto oldBoneId : itr.second)
		{
			auto& oldBone = bones[oldBoneId];
			if (oldBone.ParentBone == -1)
				continue;

			if (!boneMap.Equal(newBone.ParentBone, oldBone.ParentBone))
				toErase[boneNewId].insert(oldBoneId);
		}
	}
	boneMap.EraseData(toErase);
	toErase.clear();

	auto lookupNew = newM2->Elements[EElement_KeyBoneLookup].asVector<CElement_BoneLookup>();
	auto lookupOld = oldM2->Elements[EElement_KeyBoneLookup].asVector<CElement_BoneLookup>();

	auto maxIndex = std::max(lookupNew.size(), lookupOld.size());

	for (UInt32 i = 0; i < maxIndex; ++i)
	{
		if (i >= lookupNew.size())
			continue;

		auto& newLookup = lookupNew[i];

		for (UInt32 j = 0; j < maxIndex; ++j)
		{
			if (j >= lookupOld.size())
				continue;

			auto& oldLookup = lookupOld[i];

			boneMap.EraseExcept(newLookup.BoneLookup, oldLookup.BoneLookup);
		}
	}
}

std::string M2Lib::BoneComparator::ShowDifference()
{
	if (!prepared)
		Prepare();

	//boneMap.CheckAmbiguity();
	return boneMap.GetDiff();
}

static const UInt32 Signature_M2BR = MakeFourCC('M', '2', 'B', 'R');

M2Lib::EError M2Lib::BoneComparator::Save(char const* fileName)
{
	if (!prepared)
		Prepare();

	auto result = boneMap.CheckAmbiguity();
	if (result.length() != 0)
		return EError_FailedToSaveM2BR_Ambiguous;;

	// open file stream
	std::fstream FileStream;
	FileStream.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToExportM2I_CouldNotOpenFile;

	// open binary stream
	DataBinary DataBinary(&FileStream, EEndianness_Little);

	DataBinary.WriteFourCC(Signature_M2BR);
	DataBinary.WriteUInt16(1);
	DataBinary.WriteUInt16(0);

	std::vector<UInt16> data;
	for (auto itr : boneMap)
	{
		if (*itr.second.begin() == itr.first)
			continue;
		data.push_back(*itr.second.begin());
		data.push_back(itr.first);
	}

	DataBinary.WriteUInt16(data.size() / 2);
	for (auto val : data)
		DataBinary.WriteUInt16(val);

	FileStream.close();

	return EError_OK;
}
