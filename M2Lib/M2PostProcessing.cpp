#include "M2.h"
#include "Logger.h"
#include <set>
#include <iomanip>
#include <sstream>

using namespace M2Lib::M2Element;
using namespace M2Lib::M2Chunk;
using namespace M2Lib::M2SkinElement;

namespace M2Lib
{
	class EdgeLookup
	{
		M2* m2 = nullptr;

		std::map<CElement_SubMesh const*, std::set<uint16_t>> uniqueVerticesMap, verticesMap;

	public:
		void Initialize(M2* m2)
		{
			this->m2 = m2;
		}

		std::set<uint16_t> const& GetEdgeVertices(CElement_SubMesh const* submesh)
		{
			auto cached = uniqueVerticesMap.find(submesh);
			if (cached != uniqueVerticesMap.end())
				return cached->second;

			auto VertexCount = m2->Elements[EElement_Vertex].Count;

			uint32_t TriangleIndexStart = submesh->GetStartTrianlgeIndex();
			uint32_t TriangleIndexEnd = submesh->GetEndTriangleIndex();

			uint16_t* Triangles = m2->Skins[0]->Elements[EElement_TriangleIndex].as<uint16_t>();
			uint16_t* Indices = m2->Skins[0]->Elements[EElement_VertexLookup].as<uint16_t>();

			std::map<uint32_t, uint16_t> usageCounters;

			for (uint32_t k = TriangleIndexStart; k < TriangleIndexEnd; k += 3)
			{
				m2lib_assert(k + 2 < m2->Skins[0]->Elements[EElement_TriangleIndex].Count);

				uint16_t indexA = Indices[Triangles[k]];
				m2lib_assert(indexA < VertexCount);
				uint16_t indexB = Indices[Triangles[k + 1]];
				m2lib_assert(indexB < VertexCount);
				uint16_t indexC = Indices[Triangles[k + 2]];
				m2lib_assert(indexC < VertexCount);

				auto hashA = Geometry::Edge::GetHash(indexA, indexB);
				auto hashB = Geometry::Edge::GetHash(indexA, indexC);
				auto hashC = Geometry::Edge::GetHash(indexB, indexC);

				auto itr = usageCounters.find(hashA);
				if (itr == usageCounters.end())
					usageCounters[hashA] = 1;
				else
					++itr->second;

				itr = usageCounters.find(hashB);
				if (itr == usageCounters.end())
					usageCounters[hashB] = 1;
				else
					++itr->second;

				itr = usageCounters.find(hashC);
				if (itr == usageCounters.end())
					usageCounters[hashC] = 1;
				else
					++itr->second;
			}

			for (const auto itr : usageCounters)
			{
				if (itr.second > 1)
					continue;

				uniqueVerticesMap[submesh].insert(itr.first >> 16);
				uniqueVerticesMap[submesh].insert(itr.first & 0xFF);
			}

			return uniqueVerticesMap[submesh];
		}

		std::set<uint16_t> const& GetVertices(CElement_SubMesh const* submesh)
		{
			auto cached = verticesMap.find(submesh);
			if (cached != verticesMap.end())
				return cached->second;

			auto VertexCount = m2->Elements[EElement_Vertex].Count;

			uint32_t TriangleIndexStart = submesh->GetStartTrianlgeIndex();
			uint32_t TriangleIndexEnd = submesh->GetEndTriangleIndex();

			uint16_t* Triangles = m2->Skins[0]->Elements[EElement_TriangleIndex].as<uint16_t>();
			uint16_t* Indices = m2->Skins[0]->Elements[EElement_VertexLookup].as<uint16_t>();

			for (uint32_t k = TriangleIndexStart; k < TriangleIndexEnd; k += 3)
			{
				m2lib_assert(k + 2 < m2->Skins[0]->Elements[EElement_TriangleIndex].Count);

				uint16_t indexA = Indices[Triangles[k]];
				m2lib_assert(indexA < VertexCount);
				uint16_t indexB = Indices[Triangles[k + 1]];
				m2lib_assert(indexB < VertexCount);
				uint16_t indexC = Indices[Triangles[k + 2]];
				m2lib_assert(indexC < VertexCount);

				verticesMap[submesh].insert(indexA);
				verticesMap[submesh].insert(indexB);
				verticesMap[submesh].insert(indexC);
			}

			return verticesMap[submesh];
		}
	};
}

M2Lib::EdgeLookup triangleLookup;

void M2Lib::M2::FixNormals(float AngularTolerance)
{
	auto pSkin = Skins[0];
	
	auto allMeshes = pSkin->Elements[EElement_SubMesh].as<CElement_SubMesh>();
	uint32_t meshCount = pSkin->Elements[EElement_SubMesh].Count;
	std::list<CElement_SubMesh const*> body, armor, rest;

	std::list<uint32_t> bodyMeshIds, armorMeshIds, exclusiveMeshIds;
	uint32_t unkCount = 0;
	for (uint32_t i = 0; i < meshCount; ++i)
	{
		const auto submesh = &allMeshes[i];

		if (normalizationRules.IsMatch(submesh->ID))
		{
			body.push_back(submesh);
			exclusiveMeshIds.push_back(submesh->ID);
			continue;
		}

		switch (GetSubSetType(submesh->ID))
		{
			case Subset_Body:
				body.push_back(submesh);
				bodyMeshIds.push_back(submesh->ID);
				break;
			case Subset_Armor:
				armorMeshIds.push_back(submesh->ID);
				armor.push_back(submesh);
				break;
			case Subset_Unknown:
				++unkCount;
				rest.push_back(submesh);
				break;
			default:
				rest.push_back(submesh);
				break;
		}
	}

	std::wstringstream ss;
	ss << L"Exclusive normalization rules: ";
	for (auto id : normalizationRules.GetRules())
		ss << std::hex << std::setfill(L'0') << std::setw(8) << id << " ";
	sLogger.LogInfo(ss.str().c_str());

	sLogger.LogInfo(L"Body mesh count: %u, armor mesh count: %u, rest mesh count: %u", body.size(), armor.size(), rest.size());

	//sLogger.LogInfo(L"Body meshes:");
	//for (auto id : bodyMeshIds)
	//	sLogger.LogInfo(L"%u", id);

	triangleLookup.Initialize(this);

	FixNormals(body, body, -1.0f, false);
	FixNormals(body, armor, -1.0f, true);
}

void M2Lib::M2::FixNormals(std::list<CElement_SubMesh const*> const& sourceList, std::list<CElement_SubMesh const*> const& targetList, float AngularTolerance, bool preferSource)
{
	auto VertexList = Elements[EElement_Vertex].as<CVertex>();
	auto VertexCount = Elements[EElement_Vertex].Count;

	uint32_t similar = 0, compared = 0;
	// lookup hash map to make it slow af
	std::set<uint32_t> processedVertices;

	for (auto SubmeshI : sourceList)
	{
		//sLogger.LogError(L"Mesh %u", SubmeshI->ID);
		auto verticesI = triangleLookup.GetEdgeVertices(SubmeshI);

		for (auto iVertex : verticesI)
		{
			m2lib_assert(iVertex < VertexCount);
			auto vertexI = &VertexList[iVertex];
			if (processedVertices.find(iVertex) != processedVertices.end())
				continue;

			auto newNormal = vertexI->Normal;

			std::set<uint16_t> averaged;
			for (auto SubmeshJ : targetList)
			{
				auto verticesJ = triangleLookup.GetVertices(SubmeshJ);
				for (auto jVertex : verticesJ)
				{
					if (iVertex == jVertex)
						continue;

					if (processedVertices.find(jVertex) != processedVertices.end())
						continue;

					auto vertexJ = &VertexList[jVertex];

					static float const tolerance = 1e-4f;

					++compared;
					if (!CVertex::CompareSimilar(*vertexI, *vertexJ, false, false, false, -1.0f, AngularTolerance))
						continue;

					processedVertices.insert(jVertex);

					++similar;

					newNormal = newNormal + vertexJ->Normal;
					averaged.insert(jVertex);
				}
			}

			processedVertices.insert(iVertex);

			// if normal is zero, then possibly something is wrong
			// perhaps it's two surfaces on each other
			if (floatEq(newNormal.Length(), 0.0f))
				continue;

			if (!preferSource)
			{
				newNormal.Normalize();

				for (auto itr : averaged)
					VertexList[itr].Normal = newNormal;

				vertexI->Normal = newNormal;
			}
			else
			{
				for (auto itr : averaged)
					VertexList[itr].Normal = vertexI->Normal;
			}
		}
	}

	//sLogger.LogError(L"Similar vertices count: %u, compared: %u", similar, compared);
}

void M2Lib::M2::Scale(float Scale)
{
	// vertices
	{
		uint32_t VertexListLength = Elements[EElement_Vertex].Count;
		CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
		for (uint32_t i = 0; i < VertexListLength; i++)
		{
			CVertex& Vertex = VertexList[i];
			Vertex.Position = Vertex.Position * Scale;
		}
	}

	// bones
	{
		auto boneElement = GetBones();
		uint32_t BoneListLength = boneElement->Count;
		CElement_Bone* BoneList = boneElement->as<CElement_Bone>();
		for (uint32_t i = 0; i < BoneListLength; i++)
		{
			CElement_Bone& Bone = BoneList[i];
			Bone.Position = Bone.Position * Scale;
		}
	}

	// attachments
	{
		auto attachmentElement = GetAttachments();
		uint32_t AttachmentListLength = attachmentElement->Count;
		CElement_Attachment* AttachmentList = attachmentElement->as<CElement_Attachment>();
		for (uint32_t i = 0; i < AttachmentListLength; i++)
		{
			CElement_Attachment& Attachment = AttachmentList[i];
			Attachment.Position = Attachment.Position * Scale;
		}
	}

	// events
	{
		uint32_t EventListLength = Elements[EElement_Event].Count;
		CElement_Event* EventList = Elements[EElement_Event].as<CElement_Event>();
		for (uint32_t i = 0; i < EventListLength; i++)
		{
			CElement_Event& Event = EventList[i];
			Event.Position = Event.Position * Scale;
		}
	}

	// lights
	{
		uint32_t LightListLength = Elements[EElement_Light].Count;
		CElement_Light* LightList = Elements[EElement_Light].as<CElement_Light>();
		for (uint32_t i = 0; i < LightListLength; i++)
		{
			CElement_Light& Light = LightList[i];
			Light.Position = Light.Position * Scale;
		}
	}

	// cameras
	{
		uint32_t CameraListLength = Elements[EElement_Camera].Count;
		CElement_Camera* CameraList = Elements[EElement_Camera].as<CElement_Camera>();
		for (uint32_t i = 0; i < CameraListLength; i++)
		{
			CElement_Camera& Camera = CameraList[i];
			Camera.Position = Camera.Position * Scale;
			Camera.Target = Camera.Target * Scale;
		}
	}

	// ribbon emitters
	{
		uint32_t RibbonEmitterListLength = Elements[EElement_RibbonEmitter].Count;
		CElement_RibbonEmitter* RibbonEmitterList = Elements[EElement_RibbonEmitter].as<CElement_RibbonEmitter>();
		for (uint32_t i = 0; i < RibbonEmitterListLength; i++)
		{
			CElement_RibbonEmitter& RibbonEmitter = RibbonEmitterList[i];
			RibbonEmitter.Position = RibbonEmitter.Position * Scale;
		}
	}

	// particle emitters
	{
		uint32_t ParticleEmitterListLength = Elements[EElement_ParticleEmitter].Count;
		CElement_ParticleEmitter* ParticleEmitterList = Elements[EElement_ParticleEmitter].as<CElement_ParticleEmitter>();
		for (uint32_t i = 0; i < ParticleEmitterListLength; i++)
		{
			CElement_ParticleEmitter& ParticleEmitter = ParticleEmitterList[i];
			ParticleEmitter.Position = ParticleEmitter.Position * Scale;
		}
	}
}

void M2Lib::M2::MirrorCamera()
{
	CElement_Camera* Cameras = Elements[EElement_Camera].as<CElement_Camera>();
	uint32_t CameraCount = Elements[EElement_Camera].Count;
	for (uint32_t iCamera = 0; iCamera < CameraCount; iCamera++)
	{
		if (Cameras->Type == 0)
		{
			Cameras->Position.Y = -Cameras->Position.Y;
			Cameras->Target.Y = -Cameras->Target.Y;
			break;
		}
	}
}

void M2Lib::M2::FixSeamsSubMesh(float PositionalTolerance, float AngularTolerance)
{
	// gather up sub meshes
	std::vector< std::vector< M2SkinElement::CElement_SubMesh* > > SubMeshes;

	M2SkinElement::CElement_SubMesh* Subsets = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	uint32_t SubsetCount = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	for (uint32_t i = 0; i < SubsetCount; ++i)
	{
		uint16_t ThisID = Subsets[i].ID;
		bool MakeNew = true;
		for (uint32_t j = 0; j < SubMeshes.size(); j++)
		{
			for (uint32_t k = 0; k < SubMeshes[j].size(); k++)
			{
				if (SubMeshes[j][k]->ID == ThisID)
				{
					MakeNew = false;
					SubMeshes[j].push_back(&Subsets[i]);
					break;
				}
			}
			if (!MakeNew)
				break;
		}
		if (MakeNew)
		{
			std::vector< M2SkinElement::CElement_SubMesh* > NewSubmeshSubsetList;
			NewSubmeshSubsetList.push_back(&Subsets[i]);
			SubMeshes.push_back(NewSubmeshSubsetList);
		}
	}

	// find and merge duplicate vertices
	uint32_t VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (uint32_t iSubMesh1 = 0; iSubMesh1 < SubMeshes.size(); iSubMesh1++)
	{
		for (uint32_t iSubSet1 = 0; iSubSet1 < SubMeshes[iSubMesh1].size(); iSubSet1++)
		{
			M2SkinElement::CElement_SubMesh* pSubSet1 = SubMeshes[iSubMesh1][iSubSet1];

			uint32_t VertexAEnd = pSubSet1->VertexStart + pSubSet1->VertexCount;
			for (uint32_t iVertexA = pSubSet1->VertexStart; iVertexA < VertexAEnd; iVertexA++)
			{
				bool AddedVertexA = false;
				for (uint32_t iSubSet2 = 0; iSubSet2 < SubMeshes[iSubMesh1].size(); iSubSet2++)
				{
					M2SkinElement::CElement_SubMesh* pSubSet2 = SubMeshes[iSubMesh1][iSubSet2];

					uint32_t VertexBEnd = pSubSet2->VertexStart + pSubSet2->VertexCount;
					for (uint32_t iVertexB = pSubSet2->VertexStart; iVertexB < VertexBEnd; iVertexB++)
					{
						if (iVertexA == iVertexB)
							continue;

						if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, true, PositionalTolerance, AngularTolerance))
						{
							if (!AddedVertexA)
							{
								SimilarVertices.push_back(&VertexList[iVertexA]);
								AddedVertexA = true;
							}

							SimilarVertices.push_back(&VertexList[iVertexB]);
						}
					}
				}

				if (SimilarVertices.size() > 1)
				{
					// sum positions and normals
					C3Vector NewPosition, NewNormal;

					for (uint32_t iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition = NewPosition + pSimilarVertex->Position;
						NewNormal = NewNormal + pSimilarVertex->Normal;
					}

					// average position and normalize normal
					float invSimilarCount = 1.f / (float)SimilarVertices.size();

					NewPosition = NewPosition * invSimilarCount;
					NewNormal = NewNormal * invSimilarCount;

					uint8_t NewBoneWeights[BONES_PER_VERTEX], NewBoneIndices[BONES_PER_VERTEX];
					for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
					{
						NewBoneWeights[i] = SimilarVertices[0]->BoneWeights[i];
						NewBoneIndices[i] = SimilarVertices[0]->BoneIndices[i];
					}

					// assign new values back into similar vertices
					for (uint32_t iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position = NewPosition;
						pSimilarVertex->Normal = NewNormal;

						for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pSimilarVertex->BoneWeights[i] = NewBoneWeights[i];
							pSimilarVertex->BoneIndices[i] = NewBoneIndices[i];
						}
					}

					SimilarVertices.clear();
				}
			}
		}
	}
}

void M2Lib::M2::FixSeamsBody(float PositionalTolerance, float AngularTolerance)
{
	// sub meshes that are divided up accross multiple bone partitions will have multiple sub mesh entries with the same ID in the M2.
	// we need to gather each body submesh up into a list and average normals of vertices that are similar between other sub meshes.
	// this function is designed to be used on character models, so it may not work on other models.

	// list of submeshes that make up the body of the character
	std::vector< std::vector< M2SkinElement::CElement_SubMesh* > > CompiledSubMeshList;

	// gather up the body submeshes
	M2SkinElement::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	uint32_t SubsetCount = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	for (uint32_t i = 0; i < SubsetCount; i++)
	{
		// determine type of subset
		uint16_t ThisID = SubMeshList[i].ID;
		uint16_t Mod = ThisID;
		while (Mod > 10)
		{
			Mod %= 10;
		}
		if ((ThisID == 0) || (ThisID > 10 && Mod == 1) || (ThisID == 702))
		{
			// this subset is part of the character's body
			// add it to the list of submeshes
			bool MakeNew = true;
			for (uint32_t j = 0; j < CompiledSubMeshList.size(); j++)
			{
				for (uint32_t k = 0; k < CompiledSubMeshList[j].size(); k++)
				{
					if (CompiledSubMeshList[j][k]->ID == ThisID)
					{
						MakeNew = false;
						CompiledSubMeshList[j].push_back(&SubMeshList[i]);
						break;
					}
				}
				if (!MakeNew)
				{
					break;
				}
			}
			if (MakeNew)
			{
				std::vector< M2SkinElement::CElement_SubMesh* > NewSubmeshSubsetList;
				NewSubmeshSubsetList.push_back(&SubMeshList[i]);
				CompiledSubMeshList.push_back(NewSubmeshSubsetList);
			}
		}
	}

	// find and merge duplicate vertices
	uint32_t VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (int32_t iSubMesh1 = 0; iSubMesh1 < (int32_t)CompiledSubMeshList.size() - 1; iSubMesh1++)
	{
		for (int32_t iSubSet1 = 0; iSubSet1 < (int32_t)CompiledSubMeshList[iSubMesh1].size(); iSubSet1++)
		{
			// gather duplicate vertices
			// for each vertex in the subset, compare it against vertices in the other subsets
			// find duplicates and sum their normals
			uint32_t iVertexAEnd = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart + CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexCount;
			for (uint32_t iVertexA = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart; iVertexA < iVertexAEnd; iVertexA++)
			{
				// gather duplicate vertices from other submeshes
				bool AddedVertex1 = false;
				for (int32_t iSubMesh2 = iSubMesh1 + 1; iSubMesh2 < (int32_t)CompiledSubMeshList.size(); iSubMesh2++)
				{
					// check that we don't check against ourselves
					if (iSubMesh2 == iSubMesh1)
					{
						// other submesh is same as this submesh
						continue;
					}
					// go through subsets
					for (int32_t iSubSet2 = 0; iSubSet2 < (int32_t)CompiledSubMeshList[iSubMesh2].size(); iSubSet2++)
					{
						// go through vertices in subset
						uint32_t iVertexBEnd = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart + CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexCount;
						for (uint32_t iVertexB = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart; iVertexB < iVertexBEnd; iVertexB++)
						{
							if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, true, PositionalTolerance, AngularTolerance))
							{
								// found a duplicate
								if (!AddedVertex1)
								{
									SimilarVertices.push_back(&VertexList[iVertexA]);
									AddedVertex1 = true;
								}
								// add the vertex from the other sub mesh to the list of similar vertices
								SimilarVertices.push_back(&VertexList[iVertexB]);
							}
						}
					}
				}

				// average normals of similar vertices
				if (SimilarVertices.size())
				{
					// sum positions and normals
					C3Vector NewPosition, NewNormal;

					for (uint32_t iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition = NewPosition + pSimilarVertex->Position;
						NewNormal = NewNormal + pSimilarVertex->Normal;
					}

					// average position and normalize normal
					float invSimilarCount = 1.0f / (float)SimilarVertices.size();

					NewPosition = NewPosition * invSimilarCount;
					NewNormal = NewNormal * invSimilarCount;

					uint8_t NewBoneWeights[BONES_PER_VERTEX], NewBoneIndices[BONES_PER_VERTEX];
					for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
					{
						NewBoneWeights[i] = SimilarVertices[0]->BoneWeights[i];
						NewBoneIndices[i] = SimilarVertices[0]->BoneIndices[i];
					}

					// assign new values back into similar vertices
					for (uint32_t iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position = NewPosition;
						pSimilarVertex->Normal = NewNormal;

						for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pSimilarVertex->BoneWeights[i] = NewBoneWeights[i];
							pSimilarVertex->BoneIndices[i] = NewBoneIndices[i];
						}
					}

					// clear list
					SimilarVertices.clear();
				}
			}
		}
	}
}

void M2Lib::M2::FixSeamsClothing(float PositionalTolerance, float AngularTolerance)
{
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();

	uint32_t SubMeshListLength = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	M2SkinElement::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();

	std::vector< M2SkinElement::CElement_SubMesh* > SubMeshBodyList;	// gathered body sub meshes
	std::vector< M2SkinElement::CElement_SubMesh* > SubMeshGarbList;	// gathered clothing sub meshes

	for (uint32_t i = 0; i < SubMeshListLength; i++)
	{
		uint16_t ThisID = SubMeshList[i].ID;

		// gather body sub meshes
		uint16_t Mod = ThisID;
		if (Mod > 10)
		{
			Mod %= 10;
		}
		if (ThisID == 0 || (ThisID > 10 && Mod == 1))
		{
			SubMeshBodyList.push_back(&SubMeshList[i]);
		}
		// gather clothing sub meshes
		else if (
			ThisID == 402 ||	// cloth glove
			ThisID == 403 ||	// leather glove
			ThisID == 404 ||	// plate glove
			ThisID == 802 ||	// straight sleeve
			ThisID == 803 ||	// shaped sleeve
			ThisID == 902 ||	// low pant
			ThisID == 903 ||	// hight pant
			ThisID == 502 ||	// cloth boot
			ThisID == 503 ||	// leather boot
			ThisID == 504 ||	// plate boot
			ThisID == 505 ||	// plate boot 2
			ThisID == 1002 ||	// shirt frill short
			ThisID == 1102 ||	// shirt frill long
			ThisID == 1104 ||	// plate leg
			ThisID == 1202 ||	// tabard
			ThisID == 1302		// skirt
			//ThisID == 1802	// plate belt
			)
		{
			SubMeshGarbList.push_back(&SubMeshList[i]);
		}
	}

	// copy vertex properties from main body vertex to duplicate clothing vertices
	for (uint32_t iSubMeshGarb = 0; iSubMeshGarb < SubMeshGarbList.size(); iSubMeshGarb++)
	{
		M2SkinElement::CElement_SubMesh* pSubMeshGarb = SubMeshGarbList[iSubMeshGarb];
		for (uint32_t iSubMeshBody = 0; iSubMeshBody < SubMeshBodyList.size(); iSubMeshBody++)
		{
			M2SkinElement::CElement_SubMesh* pSubMeshBody = SubMeshBodyList[iSubMeshBody];

			for (int32_t iVertexGarb = pSubMeshGarb->VertexStart; iVertexGarb < pSubMeshGarb->VertexStart + pSubMeshGarb->VertexCount; iVertexGarb++)
			{
				for (int32_t iVertexBody = pSubMeshBody->VertexStart; iVertexBody < pSubMeshBody->VertexStart + pSubMeshBody->VertexCount; iVertexBody++)
				{
					if (CVertex::CompareSimilar(VertexList[iVertexGarb], VertexList[iVertexBody], false, false, true, PositionalTolerance, AngularTolerance))
					{
						// copy position, normal, and bone weights, and bone indices from body vertex to other(clothing) vertex
						CVertex* pVertexOther = &VertexList[iVertexGarb];
						CVertex* pVertexBody = &VertexList[iVertexBody];

						pVertexOther->Position = pVertexBody->Position;
						pVertexOther->Normal = pVertexBody->Normal;

						for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pVertexOther->BoneWeights[i] = pVertexBody->BoneWeights[i];
							pVertexOther->BoneIndices[i] = pVertexBody->BoneIndices[i];
						}
					}
				}
			}
		}
	}
}
