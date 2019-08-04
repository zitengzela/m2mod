#include "M2I.h"
#include "M2Types.h"
#include <assert.h>
#include "DataBinary.h"
#include "M2.h"
#include "Logger.h"

M2Lib::EError M2Lib::M2I::Load(wchar_t const* FileName, M2Lib::M2* pM2, bool IgnoreBones, bool IgnoreAttachments, bool IgnoreCameras, bool IgnoreOriginalMeshIndexes)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToImportM2I_CouldNotOpenFile;
	DataBinary DataBinary(&FileStream, EEndianness_Little);

	// load signature
	uint32_t InSignature = 0;
	InSignature = DataBinary.ReadFourCC();
	if (InSignature != 1 && InSignature != Signature_M2I0)
		return EError_FailedToImportM2I_FileCorrupt;

	// load version
	uint32_t Version = 0;
	if (InSignature == Signature_M2I0)
	{
		uint16_t VersionMajor = DataBinary.ReadUInt16();
		uint16_t VersionMinor = DataBinary.ReadUInt16();

		Version = MAKE_VERSION(VersionMajor, VersionMinor);

		if (!(Version >= MAKE_VERSION(4, 5) && Version <= MAKE_VERSION(4, 9)) && !(Version >= MAKE_VERSION(8, 0) && Version <= MAKE_VERSION(8, 1)))
			return EError_FailedToImportM2I_UnsupportedVersion;
	}

	// load sub meshes, build new vertex list
	uint32_t VertexStart = 0;
	uint32_t InSubsetCount = 0;
	InSubsetCount = DataBinary.ReadUInt32();
	uint32_t iTriangle = 0;

	for (uint32_t i = 0; i < InSubsetCount; i++)
	{
		M2I::CSubMesh* pNewSubMesh = new M2I::CSubMesh();

		// read id
		pNewSubMesh->ID = DataBinary.ReadUInt16();
		pNewSubMesh->ExtraData.ID = pNewSubMesh->ID;

		if (Version >= MAKE_VERSION(4, 6))
			pNewSubMesh->ExtraData.Description = DataBinary.ReadASCIIString();
		if (Version >= MAKE_VERSION(4, 7))
		{
			pNewSubMesh->ExtraData.MaterialOverride = DataBinary.ReadSInt16();
			
			if (Version >= MAKE_VERSION(8, 1))
			{
				pNewSubMesh->ExtraData.ShaderId = DataBinary.ReadSInt32();

				pNewSubMesh->ExtraData.BlendMode = DataBinary.ReadSInt16();
				pNewSubMesh->ExtraData.RenderFlags = DataBinary.ReadUInt16();

				for (uint32_t j = 0; j < MAX_SUBMESH_TEXTURES; ++j)
				{
					pNewSubMesh->ExtraData.TextureType[j] = DataBinary.ReadSInt16();
					pNewSubMesh->ExtraData.TextureName[j] = DataBinary.ReadASCIIString();
				}
			}
			else
			{
				if (Version >= MAKE_VERSION(4, 9))
				{
					if (DataBinary.ReadUInt8() != 0)
					{
						pNewSubMesh->ExtraData.ShaderId = TRANSPARENT_SHADER_ID;
						pNewSubMesh->ExtraData.TextureType[0] = (int32_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded;
						pNewSubMesh->ExtraData.TextureName[0] = DataBinary.ReadASCIIString();

						pNewSubMesh->ExtraData.RenderFlags = (int32_t)M2Element::CElement_TextureFlag::EFlags::EFlags_TwoSided;
						pNewSubMesh->ExtraData.BlendMode = DataBinary.ReadUInt16();
					}
					else
					{
						DataBinary.ReadASCIIString();
						DataBinary.ReadUInt16();
					}

					if (DataBinary.ReadUInt8() != 0)
					{
						pNewSubMesh->ExtraData.ShaderId = GLOSS_SHADER_ID;

						pNewSubMesh->ExtraData.TextureType[1] = (int32_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded;
						pNewSubMesh->ExtraData.TextureName[1] = DataBinary.ReadASCIIString();
					}
					else
						DataBinary.ReadASCIIString();
				}
				else
				{
					pNewSubMesh->ExtraData.TextureName[0] = DataBinary.ReadASCIIString();
					if (pNewSubMesh->ExtraData.TextureName[0].length() > 0)
					{
						pNewSubMesh->ExtraData.ShaderId = TRANSPARENT_SHADER_ID;
						pNewSubMesh->ExtraData.TextureType[0] = (int32_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded;
						pNewSubMesh->ExtraData.RenderFlags = (int32_t)M2Element::CElement_TextureFlag::EFlags::EFlags_TwoSided;
						pNewSubMesh->ExtraData.BlendMode = (int32_t)M2Element::CElement_TextureFlag::EBlend::EBlend_Decal;
					}

					pNewSubMesh->ExtraData.TextureName[1] = DataBinary.ReadASCIIString();
					if (pNewSubMesh->ExtraData.TextureName[1].length() > 0)
					{
						pNewSubMesh->ExtraData.ShaderId = GLOSS_SHADER_ID;
						pNewSubMesh->ExtraData.TextureType[1] = (int32_t)M2Element::CElement_Texture::ETextureType::Final_Hardcoded;
					}
				}
			}
		}

		if (Version >= MAKE_VERSION(8, 0))
		{
			if (!IgnoreOriginalMeshIndexes)
				pNewSubMesh->ExtraData.OriginalSubmeshIndex = DataBinary.ReadSInt32();
			else
				DataBinary.ReadSInt32();
		}

		// FMN 2015-02-13: read level
		DataBinary.ReadUInt16();
		pNewSubMesh->Level = 0; 

		// read vertices
		uint32_t InVertexCount = 0;
		InVertexCount = DataBinary.ReadUInt32();
		if (VertexList.size() + InVertexCount > 0xFFFF)
			return EError_FailedToImportM2I_TooManyVertices;

		std::vector<CVertex> submeshVertices;
		for (uint32_t j = 0; j < InVertexCount; ++j)
		{
			CVertex InVertex;

			InVertex.Position = DataBinary.ReadC3Vector();

			for (uint32_t k = 0; k < BONES_PER_VERTEX; ++k)
				InVertex.BoneWeights[k] = DataBinary.ReadUInt8();

			for (uint32_t k = 0; k < BONES_PER_VERTEX; ++k)
				InVertex.BoneIndices[k] = DataBinary.ReadUInt8();

			InVertex.Normal = DataBinary.ReadC3Vector();
			InVertex.Texture = DataBinary.ReadC2Vector();
			if (Version >= MAKE_VERSION(8, 0))
				InVertex.Texture2 = DataBinary.ReadC2Vector();

			uint16_t VertexIndex = VertexList.size();
			VertexList.push_back(InVertex);
			pNewSubMesh->Indices.push_back(VertexIndex);

			submeshVertices.push_back(InVertex);
		}

		pNewSubMesh->ExtraData.Boundary.Calculate(submeshVertices);

		// read triangles
		uint32_t InTriangleCount = DataBinary.ReadUInt32();

		for (uint32_t j = 0; j < InTriangleCount; ++j)
		{
			CTriangle NewTriangle;

			NewTriangle.TriangleIndex = iTriangle;
			++iTriangle;

			for (uint32_t k = 0; k < VERTEX_PER_TRIANGLE; ++k)
				NewTriangle.Vertices[k] = DataBinary.ReadUInt16() + VertexStart;

			pNewSubMesh->Triangles.push_back(NewTriangle);
		}

		VertexStart += InVertexCount;

		SubMeshList.push_back(pNewSubMesh);
	}

	std::map<uint16_t, uint16_t> BoneRemap;

	if (!IgnoreBones)
	{
		// read bones, overwrite existing
		auto boneElement = pM2->GetBones();
		uint32_t BoneCount = boneElement->Count;
		uint32_t BoneCountIn = DataBinary.ReadUInt32();
		for (uint32_t i = 0; i < BoneCountIn; ++i)
		{
			uint16_t InBoneIndex = DataBinary.ReadUInt16();
			int16_t ParentBone = DataBinary.ReadSInt16();
			C3Vector Position = DataBinary.ReadC3Vector();
			bool HasExtraData = false;
			uint32_t Flags;
			uint16_t SubmeshId;
			uint16_t Unknown[2];

			if (Version >= MAKE_VERSION(4, 8))
			{
				HasExtraData = DataBinary.ReadUInt8() != 0;
				Flags = DataBinary.ReadUInt32();
				SubmeshId = DataBinary.ReadUInt16();
				Unknown[0] = DataBinary.ReadUInt16();
				Unknown[1] = DataBinary.ReadUInt16();
			}

			uint16_t ModBoneIndex = -1;
			if (InBoneIndex < BoneCount)
			{
				ModBoneIndex = InBoneIndex;
			}
			else
			{
				sLogger.LogInfo("Extra bones detected: skipping");
				continue;
			}
			/*else if (HasExtraData)
			{
				M2Element::CElement_Bone newBone;
				newBone.BoneLookupID = -1;
				ModBoneIndex = pM2->AddBone(newBone);
				BoneRemap[InBoneIndex] = ModBoneIndex;
			}*/

			if (ModBoneIndex == -1)
				continue;

			auto Bones = boneElement->as<M2Element::CElement_Bone>();
			auto& BoneToMod = Bones[ModBoneIndex];

			BoneToMod.ParentBone = ParentBone;
			BoneToMod.Position = Position;

			if (HasExtraData)
			{
				BoneToMod.Flags = (M2Element::CElement_Bone::EFlags)Flags;
				BoneToMod.SubmeshId = SubmeshId;
				BoneToMod.Unknown[0] = Unknown[0];
				BoneToMod.Unknown[1] = Unknown[1];
			}
		}
	}
	else
	{
		uint32_t BoneCountIn = DataBinary.ReadUInt32();
		for (uint32_t i = 0; i < BoneCountIn; ++i)
		{
			DataBinary.ReadUInt16();
			DataBinary.ReadSInt16();
			DataBinary.ReadC3Vector();

			if (Version >= MAKE_VERSION(4, 8))
			{
				DataBinary.ReadUInt8();
				DataBinary.ReadUInt32();
				DataBinary.ReadUInt16();
				DataBinary.ReadUInt16();
				DataBinary.ReadUInt16();
			}
		}
	}

	// this is only executed when new bones are present, skip
	/*if (!BoneRemap.empty())
	{
		uint32_t BoneCount = pM2->Elements[M2Element::EElement_Bone].Count;
		auto Bones = pM2->Elements[M2Element::EElement_Bone].as<M2Element::CElement_Bone>();
		for (uint32_t i = 0; i < BoneCount; ++i)
		{
			auto& Bone = Bones[i];
			if (BoneRemap.find(Bone.ParentBone) == BoneRemap.end())
				continue;

			Bone.ParentBone = BoneRemap[Bone.ParentBone];
		}

		for (auto& Vertex : VertexList)
		{
			for (uint32_t i = 0; i < BONES_PER_VERTEX; ++i)
			{
				if (BoneRemap.find(Vertex.BoneIndices[i]) == BoneRemap.end())
					continue;

				Vertex.BoneIndices[i] = BoneRemap[Vertex.BoneIndices[i]];
			}
		}
	}*/

	if (!IgnoreAttachments)
	{
		// read attachments, overwrite existing
		auto attachmentElement = pM2->GetAttachments();
		uint32_t AttachmentsCount = attachmentElement->Count;
		auto Attachments = attachmentElement->as<M2Element::CElement_Attachment>();
		uint32_t AttachmentCountIn;
		AttachmentCountIn = DataBinary.ReadUInt32();
		for (uint32_t i = 0; i < AttachmentCountIn; ++i)
		{
			uint32_t InAttachmentID = 0;
			InAttachmentID = DataBinary.ReadUInt32();
			M2Element::CElement_Attachment* pAttachmentToMod = NULL;
			for (uint32_t j = 0; j < AttachmentsCount; ++j)
			{
				if (Attachments[j].ID == InAttachmentID)
				{
					pAttachmentToMod = &Attachments[j];
					break;
				}
			}
			if (pAttachmentToMod)
			{
				pAttachmentToMod->ParentBone = DataBinary.ReadSInt16();
				if (BoneRemap.find(pAttachmentToMod->ParentBone) != BoneRemap.end())
					pAttachmentToMod->ParentBone = BoneRemap[pAttachmentToMod->ParentBone];

				pAttachmentToMod->Position = DataBinary.ReadC3Vector();
				float Scale = DataBinary.ReadFloat32();
			}
			else
			{
				DataBinary.ReadUInt16();

				DataBinary.ReadC3Vector();
				DataBinary.ReadFloat32();
			}
		}
	}
	else
	{
		uint32_t AttachmentCountIn = DataBinary.ReadUInt32();
		for (uint32_t i = 0; i < AttachmentCountIn; ++i)
		{
			DataBinary.ReadUInt32();
			DataBinary.ReadUInt16();
			DataBinary.ReadC3Vector();
			DataBinary.ReadFloat32();
		}
	}

	if (!IgnoreCameras)
	{
		// read cameras, overwrite existing
		uint32_t CameraCount = pM2->Elements[M2Element::EElement_Camera].Count;
		auto Cameras = pM2->Elements[M2Element::EElement_Camera].as<M2Element::CElement_Camera>();
		uint32_t CameraCountIn = DataBinary.ReadUInt32();
		for (uint32_t i = 0; i < CameraCountIn; ++i)
		{
			auto hasData = true;
			if (Version >= MAKE_VERSION(4, 9))
				hasData = DataBinary.ReadUInt8() != 0;

			M2Element::CElement_Camera* pCameraToMod = NULL;
			if (hasData)
			{
				auto InType = (M2Element::CElement_Camera::ECameraType)DataBinary.ReadSInt32();
				for (uint32_t j = 0; j < CameraCount; ++j)
				{
					if (Cameras[j].Type == InType)
					{
						pCameraToMod = &Cameras[j];
						break;
					}
				}
			}
			if (pCameraToMod)
			{
				if (pCameraToMod->AnimationBlock_FieldOfView.Keys.Count > 0)
				{
					auto ExternalAnimations = (M2Array*)pM2->Elements[M2Element::EElement_Camera].GetLocalPointer(pCameraToMod->AnimationBlock_FieldOfView.Keys.Offset);

					auto LastElementIndex = pM2->GetLastElementIndex();
					assert(LastElementIndex != M2Element::EElement__Count__);
					auto& LastElement = pM2->Elements[LastElementIndex];
					assert(ExternalAnimations[0].Offset >= LastElement.Offset && ExternalAnimations[0].Offset < LastElement.Offset + LastElement.Data.size());

					float* FieldOfView_Keys = (float*)LastElement.GetLocalPointer(ExternalAnimations[0].Offset);
					auto value = DataBinary.ReadFloat32();
					FieldOfView_Keys[0] = value;
				}
				else
					DataBinary.ReadFloat32();

				pCameraToMod->ClipFar = DataBinary.ReadFloat32();
				pCameraToMod->ClipNear = DataBinary.ReadFloat32();
				pCameraToMod->Position = DataBinary.ReadC3Vector();
				pCameraToMod->Target = DataBinary.ReadC3Vector();
			}
			else
			{
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadC3Vector();
				DataBinary.ReadC3Vector();
			}
		}
	}
	else
	{
		uint32_t CameraCountIn = DataBinary.ReadUInt32();
		for (uint32_t i = 0; i < CameraCountIn; ++i)
		{
			if (Version >= MAKE_VERSION(4, 9))
				DataBinary.ReadUInt8();
			DataBinary.ReadSInt32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
		}
	}

	return EError_OK;
}
