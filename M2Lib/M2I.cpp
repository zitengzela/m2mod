#include "M2I.h"
#include "M2Types.h"
#include <assert.h>
#include "DataBinary.h"
#include "M2.h"
#include "Logger.h"

M2Lib::EError M2Lib::M2I::Load(Char16 const* FileName, M2Lib::M2* pM2, bool IgnoreBones, bool IgnoreAttachments, bool IgnoreCameras, bool IgnoreOriginalMeshIndexes)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToImportM2I_CouldNotOpenFile;
	DataBinary DataBinary(&FileStream, EEndianness_Little);

	// load signature
	UInt32 InSignature = 0;
	InSignature = DataBinary.ReadFourCC();
	if (InSignature != 1 && InSignature != Signature_M2I0)
		return EError_FailedToImportM2I_FileCorrupt;

	// load version
	UInt32 Version = 0;
	if (InSignature == Signature_M2I0)
	{
		UInt16 VersionMajor = DataBinary.ReadUInt16();
		UInt16 VersionMinor = DataBinary.ReadUInt16();

		Version = MAKE_VERSION(VersionMajor, VersionMinor);

		if (!(Version >= MAKE_VERSION(4, 5) && Version <= MAKE_VERSION(4, 9)) && Version != MAKE_VERSION(8, 0))
			return EError_FailedToImportM2I_UnsupportedVersion;
	}

	// load sub meshes, build new vertex list
	UInt32 VertexStart = 0;
	UInt32 InSubsetCount = 0;
	InSubsetCount = DataBinary.ReadUInt32();
	UInt32 iTriangle = 0;

	for (UInt32 i = 0; i < InSubsetCount; i++)
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
			
			if (Version >= MAKE_VERSION(4, 9))
			{
				if (DataBinary.ReadUInt8() != 0)
				{
					pNewSubMesh->ExtraData.CustomTextureName = DataBinary.ReadASCIIString();
					pNewSubMesh->ExtraData.TextureStyle = DataBinary.ReadUInt16();
				}
				else
				{
					DataBinary.ReadASCIIString();
					DataBinary.ReadUInt16();
				}
				if (DataBinary.ReadUInt8() != 0)
					pNewSubMesh->ExtraData.GlossTextureName = DataBinary.ReadASCIIString();
				else
					DataBinary.ReadASCIIString();
			}
			else
			{
				pNewSubMesh->ExtraData.CustomTextureName = DataBinary.ReadASCIIString();
				pNewSubMesh->ExtraData.GlossTextureName = DataBinary.ReadASCIIString();
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
		pNewSubMesh->Level = DataBinary.ReadUInt16();

		// read vertices
		UInt32 InVertexCount = 0;
		InVertexCount = DataBinary.ReadUInt32();
		if (VertexList.size() + InVertexCount > 0xFFFF)
			return EError_FailedToImportM2I_TooManyVertices;

		std::vector<CVertex> submeshVertices;
		for (UInt32 j = 0; j < InVertexCount; ++j)
		{
			CVertex InVertex;

			InVertex.Position = DataBinary.ReadC3Vector();

			for (UInt32 k = 0; k < BONES_PER_VERTEX; ++k)
				InVertex.BoneWeights[k] = DataBinary.ReadUInt8();

			for (UInt32 k = 0; k < BONES_PER_VERTEX; ++k)
				InVertex.BoneIndices[k] = DataBinary.ReadUInt8();

			InVertex.Normal = DataBinary.ReadC3Vector();
			InVertex.Texture = DataBinary.ReadC2Vector();
			if (Version >= MAKE_VERSION(8, 0))
				InVertex.Texture2 = DataBinary.ReadC2Vector();

			UInt16 VertexIndex = VertexList.size();
			VertexList.push_back(InVertex);
			pNewSubMesh->Indices.push_back(VertexIndex);

			submeshVertices.push_back(InVertex);
		}

		pNewSubMesh->ExtraData.Boundary.Calculate(submeshVertices);

		// read triangles
		UInt32 InTriangleCount = DataBinary.ReadUInt32();

		for (UInt32 j = 0; j < InTriangleCount; ++j)
		{
			CTriangle NewTriangle;

			NewTriangle.TriangleIndex = iTriangle;
			++iTriangle;

			for (UInt32 k = 0; k < VERTEX_PER_TRIANGLE; ++k)
				NewTriangle.Vertices[k] = DataBinary.ReadUInt16() + VertexStart;

			pNewSubMesh->Triangles.push_back(NewTriangle);
		}

		VertexStart += InVertexCount;

		SubMeshList.push_back(pNewSubMesh);
	}

	std::map<UInt16, UInt16> BoneRemap;

	if (!IgnoreBones)
	{
		// read bones, overwrite existing
		auto boneElement = pM2->GetBones();
		UInt32 BoneCount = boneElement->Count;
		UInt32 BoneCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < BoneCountIn; ++i)
		{
			UInt16 InBoneIndex = DataBinary.ReadUInt16();
			SInt16 ParentBone = DataBinary.ReadSInt16();
			C3Vector Position = DataBinary.ReadC3Vector();
			bool HasExtraData = false;
			UInt32 Flags;
			UInt16 SubmeshId;
			UInt16 Unknown[2];

			if (Version >= MAKE_VERSION(4, 8))
			{
				HasExtraData = DataBinary.ReadUInt8() != 0;
				Flags = DataBinary.ReadUInt32();
				SubmeshId = DataBinary.ReadUInt16();
				Unknown[0] = DataBinary.ReadUInt16();
				Unknown[1] = DataBinary.ReadUInt16();
			}

			UInt16 ModBoneIndex = -1;
			if (InBoneIndex < BoneCount)
			{
				ModBoneIndex = InBoneIndex;
			}
			else
			{
				sLogger.Log("Extra bones detected: skipping");
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
		UInt32 BoneCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < BoneCountIn; ++i)
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
		UInt32 BoneCount = pM2->Elements[M2Element::EElement_Bone].Count;
		auto Bones = pM2->Elements[M2Element::EElement_Bone].as<M2Element::CElement_Bone>();
		for (UInt32 i = 0; i < BoneCount; ++i)
		{
			auto& Bone = Bones[i];
			if (BoneRemap.find(Bone.ParentBone) == BoneRemap.end())
				continue;

			Bone.ParentBone = BoneRemap[Bone.ParentBone];
		}

		for (auto& Vertex : VertexList)
		{
			for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
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
		UInt32 AttachmentsCount = attachmentElement->Count;
		auto Attachments = attachmentElement->as<M2Element::CElement_Attachment>();
		UInt32 AttachmentCountIn;
		AttachmentCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < AttachmentCountIn; ++i)
		{
			UInt32 InAttachmentID = 0;
			InAttachmentID = DataBinary.ReadUInt32();
			M2Element::CElement_Attachment* pAttachmentToMod = NULL;
			for (UInt32 j = 0; j < AttachmentsCount; ++j)
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
				Float32 Scale = DataBinary.ReadFloat32();
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
		UInt32 AttachmentCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < AttachmentCountIn; ++i)
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
		UInt32 CameraCount = pM2->Elements[M2Element::EElement_Camera].Count;
		auto Cameras = pM2->Elements[M2Element::EElement_Camera].as<M2Element::CElement_Camera>();
		UInt32 CameraCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < CameraCountIn; ++i)
		{
			auto hasData = true;
			if (Version >= MAKE_VERSION(4, 9))
				hasData = DataBinary.ReadUInt8() != 0;

			M2Element::CElement_Camera* pCameraToMod = NULL;
			if (hasData)
			{
				auto InType = (M2Element::CElement_Camera::ECameraType)DataBinary.ReadSInt32();
				for (UInt32 j = 0; j < CameraCount; ++j)
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

					auto LastElement = pM2->GetLastElement();
					assert(LastElement != NULL);
					assert(ExternalAnimations[0].Offset >= LastElement->Offset && ExternalAnimations[0].Offset < LastElement->Offset + LastElement->Data.size());

					Float32* FieldOfView_Keys = (Float32*)LastElement->GetLocalPointer(ExternalAnimations[0].Offset);
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
		UInt32 CameraCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < CameraCountIn; ++i)
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
