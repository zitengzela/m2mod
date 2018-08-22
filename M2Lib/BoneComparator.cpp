#include "BoneComparator.h"
#include "Logger.h"
#include <map>
#include <set>
#include <sstream>

M2Lib::BoneComparator::BoneComparator(M2* oldM2, M2* newM2)
{
	this->oldM2 = oldM2;
	this->newM2 = newM2;
}

std::map<unsigned int, std::set<unsigned int>> M2Lib::BoneComparator::Diff()
{
	auto OldSkin = oldM2->Skins[0];
	auto NewSkin = newM2->Skins[0];

	using namespace M2SkinElement;
	using namespace M2Element;

	std::map<UInt32, CElement_SubMesh*> OldSubsetsById;
	std::map<UInt32, CElement_SubMesh*> NewSubsetsById;

	auto OldSubsetCount = OldSkin->Elements[M2SkinElement::EElement_SubMesh].Count;
	CElement_SubMesh* OldSubsets = OldSkin->Elements[M2SkinElement::EElement_SubMesh].as<CElement_SubMesh>();

	auto NewSubsetCount = NewSkin->Elements[M2SkinElement::EElement_SubMesh].Count;
	CElement_SubMesh* NewSubsets = NewSkin->Elements[M2SkinElement::EElement_SubMesh].as<CElement_SubMesh>();

	auto BoneElement = newM2->GetBones();

	std::map<unsigned int, std::set<unsigned int>> OldToNewBoneMap;

	UInt16* OldIndices = OldSkin->Elements[EElement_VertexLookup].as<UInt16>();
	UInt16* NewIndices = NewSkin->Elements[EElement_VertexLookup].as<UInt16>();

	CVertex* OldVertices = oldM2->Elements[EElement_Vertex].as<CVertex>();
	CVertex* NewVertices = newM2->Elements[EElement_Vertex].as<CVertex>();

	for (UInt32 m = 0; m < OldSubsetCount; ++m)
	{
		auto& OldSubSet = OldSubsets[m];

		for (UInt32 n = 0; n < NewSubsetCount; ++n)
		{
			auto& NewSubSet = NewSubsets[n];

			for (UInt32 k = OldSubSet.VertexStart; k < OldSubSet.VertexStart + OldSubSet.VertexCount; ++k)
			{
				CVertex& OldVertex = OldVertices[OldIndices[k]];

				for (UInt32 l = NewSubSet.VertexStart; l < NewSubSet.VertexStart + NewSubSet.VertexCount; ++l)
				{
					CVertex& NewVertex = NewVertices[NewIndices[l]];

					if (floatEq(NewVertex.Position.X, OldVertex.Position.X) &&
						floatEq(NewVertex.Position.Y, OldVertex.Position.Y) &&
						floatEq(NewVertex.Position.Z, OldVertex.Position.Z))
					{
						for (int i = 0; i < 4; ++i)
						{
							if (floatEq(OldVertex.BoneWeights[i], 0.0f))
								continue;

							if (OldToNewBoneMap.find(OldVertex.BoneIndices[i]) == OldToNewBoneMap.end())
								OldToNewBoneMap[OldVertex.BoneIndices[i]] = std::set<UInt32>();

							std::list<UInt32> candidates;
							for (int j = 0; j < 4; ++j)
							{
								if (floatEq(NewVertex.BoneWeights[j], 0.0f))
									continue;

								if (floatEq(NewVertex.BoneWeights[j], OldVertex.BoneWeights[i]))
									candidates.push_back(NewVertex.BoneIndices[j]);
							}

							if (candidates.size() == 1)
								OldToNewBoneMap[OldVertex.BoneIndices[i]].insert(*candidates.begin());
						}
					}
				}
			}
		}
	}

	return OldToNewBoneMap;
}
