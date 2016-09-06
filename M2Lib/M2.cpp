#include "M2.h"
#include "DataBinary.h"
#include "M2SkinBuilder.h"
#include <sstream>
#include <locale>
#include <codecvt>
#include <string>

// level of detail for output messages
int g_Verbose = 1;

struct IFFChunk
{
	Char8   ChunkId[4];
	SInt32  ChunkSize;
};

const Char8* M2Lib::M2::kChunkIDs[EChunk__Count__] = { "MD21", "PFID", "AFID", "SFID", "BFID" };

M2Lib::EError M2Lib::M2::Load(const Char16* FileName)
{
	// check path
	if (!FileName)
		return M2Lib::EError_FailedToLoadM2_NoFileSpecified;
	UInt32 Length = 0;
	while (FileName[Length] != '\0')
	{
		Length++;
	}

	if (Length >= 1024)
		return M2Lib::EError_PathTooLong;

	_FileName[Length] = '\0';
	for (UInt32 i = 0; i != Length; i++)
	{
		_FileName[i] = FileName[i];
	}

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return M2Lib::EError_FailedToLoadM2_CouldNotOpenFile;

	// find file size
	FileStream.seekg(0, std::ios::end);
	UInt32 FileSize = (UInt32)FileStream.tellg();
	m_OriginalSize = FileSize;
	FileStream.seekg(0, std::ios::beg);

	// load entire thing
	RawData = new UInt8[FileSize];
	FileStream.read((Char8*)RawData, FileSize);
	FileStream.seekg(0, std::ios::beg);

	// NEW !
	IFFChunk * Chunk = (IFFChunk *)RawData;
	bool ValidFile = false;

	M2Element::SetFileOffset(0);

	while ((UInt8*)Chunk < (RawData + FileSize) && Chunk->ChunkSize >= 0)
	{
		if (Chunk->ChunkId[0] == 'M' && Chunk->ChunkId[1] == 'D' && Chunk->ChunkId[2] == '2' && Chunk->ChunkId[3] == '0')
		{
			ValidFile = true;
			break;
		}
		else
		{
			SInt32 ChunkIndex = m_GetChunkIndex(Chunk->ChunkId);

			if (ChunkIndex >= 0)
			{
				Chunks[ChunkIndex].Count = 1;
				Chunks[ChunkIndex].Data.resize(Chunk->ChunkSize);
				Chunks[ChunkIndex].Offset = ((UInt8*)Chunk) + 8 - RawData;

				Chunks[ChunkIndex].Load(FileStream);
			}
		}
		Chunk = (IFFChunk *)((UInt8*)Chunk + Chunk->ChunkSize + 8);
	}

	if (!Chunks[EChunk_Model].Data.empty())
	{
		SInt32 MD20Offset = Chunks[EChunk_Model].Offset;
		FileSize = Chunks[EChunk_Model].Data.size();
		M2Element::SetFileOffset(MD20Offset);

		FileStream.seekg(MD20Offset, std::ios::beg);
		FileStream.read((Char8*)RawData, FileSize);
		FileStream.seekg(MD20Offset, std::ios::beg);

		ValidFile = true;
	}

	// load header
	FileStream.read((Char8*)&Header.Description, sizeof(Header.Description));

	// check header
	if (!ValidFile)
		return M2Lib::EError_FailedToLoadM2_FileCorrupt;
	UInt32 VersionInt = (UInt32&)Header.Description.Version;
	if ((263 > VersionInt) || (VersionInt > 274))
		return M2Lib::EError_FailedToLoadM2_VersionNotSupported;

	// load more header
	if (Header.Description.Flags & 0x08)
	{
		// special long header
		FileStream.read((Char8*)&Header.Elements, sizeof(Header.Elements));
	}
	else
	{
		// normal header
		FileStream.read((Char8*)&Header.Elements, sizeof(Header.Elements) - 8);
		Header.Elements.nUnknown1 = 0;
		Header.Elements.oUnknown1 = 0;
	}

	// fill elements header data
	m_LoadElements_CopyHeaderToElements();
	m_LoadElements_FindSizes(FileSize);

	// load elements
	UInt32 ElementCount = EElement__Count__;
	if (!(Header.Description.Flags & 0x08))
	{
		ElementCount--;
	}
	for (UInt32 i = 0; i != ElementCount; i++)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load(FileStream))
			return M2Lib::EError_FailedToLoadM2_FileCorrupt;
	}

	// close file stream
	FileStream.close();

	M2Element::SetFileOffset(0);

	// load skins
	if ((Header.Elements.nSkin == 0) || (Header.Elements.nSkin > 4))
		return M2Lib::EError_FailedToLoadM2_FileCorrupt;

	for (UInt32 i = 0; i != Header.Elements.nSkin; i++)
	{
		Char16 FileNameSkin[1024];
		GetFileSkin(FileNameSkin, FileName, i);

		// FMN 2015-02-03: LOD skins.
		/*
		if (i == 1 || i == 2)
		{
			std::string strFileName;
			std::wstring wstrFileName = (std::wstring)FileName;

			for (UInt8 j = 0; j < wstrFileName.size(); j++)
				strFileName += (char)wstrFileName.at(j);

			//std::string chrI = std::string(i);
			std::stringstream ss;
			std::string strI;
			ss << i;
			ss >> strI;

			std::string strFileNameLOD = strFileName.substr(0, strFileName.length() - 3) + "_LOD0" + strI + ".skin";
			//std::wstring  wstrFileNameLOD = (std::wstring)strFileNameLOD;

			//wchar_t wstrFileNameLOD = '\0';
			//for (UInt8 j = 0; j < strFileNameLOD.size(); j++)
			//	wstrFileNameLOD += (wchar_t)strFileNameLOD.at(j);

			//std::wstring name = (std::wstring)strFileNameLOD;
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			//std::string narrow = converter.to_bytes(wide_utf16_source_string);
			std::wstring wstrFileNameLOD = converter.from_bytes(strFileNameLOD);

			wchar_t* wchrFileNameLOD = (wchar_t*)wstrFileNameLOD.c_str();

			Char16* FileNameLOD = (Char16*)wchrFileNameLOD;

			GetFileSkin(FileNameLOD, FileName, i);

			// Add the LODs after the 4 main skins.
			Skins[i + 4] = new M2Lib::M2Skin(this);
			if (M2Lib::EError Error = Skins[i + 4]->Load(FileNameSkin))
				return Error;
		}
		*/

		Skins[i] = new M2Lib::M2Skin(this);
		if (M2Lib::EError Error = Skins[i]->Load(FileNameSkin))
		{
			delete Skins[i];
			Skins[i] = NULL;
			return Error;
		}
	}

	if (Header.Description.Flags & 0x80)
	{
		for (int i = 4; i < 6; ++i)
		{
			Char16 FileNameSkin[1024];
			GetFileSkin(FileNameSkin, FileName, i);

			Skins[i] = new M2Lib::M2Skin(this);
			if (M2Lib::EError Error = Skins[i]->Load(FileNameSkin))
			{
				delete Skins[i];
				Skins[i] = NULL;
				return Error;
			}
		}
	}

	// print info
	//PrintInfo();

	// done
	return M2Lib::EError_OK;
}


M2Lib::EError M2Lib::M2::Save(const Char16* FileName)
{
	// check path
	if (!FileName)
		return M2Lib::EError_FailedToSaveM2_NoFileSpecified;

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return M2Lib::EError_FailedToSaveM2;

	// fill elements header data
	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// Reserve model chunk header
	M2Lib::M2Element::SetFileOffset(8);
	FileStream.seekp(8, std::ios::beg);

	*(UInt16*)Header.Description.Version = 0x0110;
	Header.Description.Flags &= ~0x80;

	// save header
	FileStream.write((Char8*)&Header.Description, sizeof(Header.Description));
	FileStream.write((Char8*)&Header.Elements, Header.IsLongHeader() ? sizeof(Header.Elements) : sizeof(Header.Elements) - 8);

	// save elements
	UInt32 ElementCount = Header.IsLongHeader() ? EElement__Count__ : EElement__Count__ - 1;
	for (UInt32 i = 0; i < ElementCount; ++i)
	{
		if (!Elements[i].Save(FileStream))
			return M2Lib::EError_FailedToSaveM2;
	}

	UInt32 MD20Size = FileStream.tellp();
	MD20Size -= 8;

	for (UInt32 i = M2::EChunk_Model + 1; i < M2::EChunk__Count__; i++)
	{
		if (!Chunks[i].Data.empty())
		{
			FileStream.write(kChunkIDs[i], 4);
			UInt32 size = Chunks[i].Data.size();
			FileStream.write((Char8*)&size, 4);
			FileStream.write((Char8*)Chunks[i].Data.data(), Chunks[i].Data.size());
		}
	}

	FileStream.seekp(0, std::ios::beg);
	FileStream.write(kChunkIDs[M2::EChunk_Model], 4);
	FileStream.write((Char8*)(&MD20Size), 4);

	// close file stream
	FileStream.close();
	M2Lib::M2Element::SetFileOffset(0);

	// delete existing skin files
	for (UInt32 i = 0; i < 6; i++)
	{
		Char16 FileNameSkin[1024];
		GetFileSkin(FileNameSkin, FileName, i);
		_wremove(FileNameSkin);
	}

	// save skins
	if ((Header.Elements.nSkin == 0) || (Header.Elements.nSkin > 4))
		return M2Lib::EError_FailedToSaveM2;

	for (UInt32 i = 0; i < Header.Elements.nSkin; i++)
	{
		Char16 FileNameSkin[1024];
		GetFileSkin(FileNameSkin, FileName, i);

		if (i >= 4)
		{
			if (M2Lib::EError Error = Skins[i - 3]->Save(FileNameSkin))
				return Error;
		}
		else
		{
			if (M2Lib::EError Error = Skins[i]->Save(FileNameSkin))
				return Error;
		}
	}

	// 0x80 = flag_has_lod_skin_files
	if (Header.Description.Flags & 0x80)
	{
		for (int i = 0; i < 2; ++i)
		{
			Char16 FileNameSkin[1024];
			GetFileSkin(FileNameSkin, FileName, i + 4);
			if (M2Lib::EError Error = (Skins[(Skins[1]) ? 1 : 0])->Save(FileNameSkin))
				return Error;
		}
	}

	return M2Lib::EError_OK;
}

M2Lib::EError M2Lib::M2::ExportM2Intermediate(Char16* FileName)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return M2Lib::EError_FailedToExportM2I_CouldNotOpenFile;

	// open binary stream
	M2Lib::DataBinary DataBinary(&FileStream, M2Lib::EEndianness_Little);

	// get data to save
	M2Skin* pSkin = Skins[0];

	UInt32 SubsetCount = pSkin->Elements[M2Skin::EElement_SubMesh].Count;
	M2Skin::CElement_SubMesh* Subsets = pSkin->Elements[M2Skin::EElement_SubMesh].as<M2Skin::CElement_SubMesh>();

	M2Lib::CVertex* Vertices = Elements[EElement_Vertex].as<CVertex>();
	UInt16* Triangles = pSkin->Elements[M2Skin::EElement_TriangleIndex].as<UInt16>();
	UInt16* Indices = pSkin->Elements[M2Skin::EElement_VertexLookup].as<UInt16>();

	UInt32 BonesCount = Elements[EElement_Bone].Count;
	CElement_Bone* Bones = Elements[EElement_Bone].as<CElement_Bone>();

	UInt32 AttachmentsCount = Elements[EElement_Attachment].Count;
	CElement_Attachment* Attachments = Elements[EElement_Attachment].as<CElement_Attachment>();

	UInt32 CamerasCount = Elements[EElement_Camera].Count;
	CElement_Camera* Cameras = Elements[EElement_Camera].as<CElement_Camera>();

	// save signature
	DataBinary.WriteFourCC(Signature_M2I0);

	// save version
	UInt16 VersionMajor = 4;
	UInt16 VersionMinor = 5;
	DataBinary.WriteUInt16(VersionMajor);
	DataBinary.WriteUInt16(VersionMinor);

	// save subsets
	UInt32 TotalVertexCount = 0;
	UInt32 TotalTriangleCount = 0;
	DataBinary.WriteUInt32(SubsetCount);
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		M2Lib::M2Skin::CElement_SubMesh* pSubsetOut = &Subsets[i];

		// write subset ID

		UInt16 SubsetID = pSubsetOut->ID;
		UInt16 SubsetLevel = pSubsetOut->Level;

		DataBinary.WriteUInt16(SubsetID);
		DataBinary.WriteUInt16(SubsetLevel);

		// write vertices
		DataBinary.WriteUInt32(pSubsetOut->VertexCount);
		UInt32 VertexEnd = pSubsetOut->VertexStart + pSubsetOut->VertexCount;
		for (UInt32 k = pSubsetOut->VertexStart; k < VertexEnd; k++)
		{
			M2Lib::CVertex& Vertex = Vertices[Indices[k]];

			DataBinary.WriteFloat32(Vertex.Position[0]);
			DataBinary.WriteFloat32(Vertex.Position[1]);
			DataBinary.WriteFloat32(Vertex.Position[2]);

			DataBinary.WriteUInt8(Vertex.BoneWeights[0]);
			DataBinary.WriteUInt8(Vertex.BoneWeights[1]);
			DataBinary.WriteUInt8(Vertex.BoneWeights[2]);
			DataBinary.WriteUInt8(Vertex.BoneWeights[3]);

			DataBinary.WriteUInt8(Vertex.BoneIndices[0]);
			DataBinary.WriteUInt8(Vertex.BoneIndices[1]);
			DataBinary.WriteUInt8(Vertex.BoneIndices[2]);
			DataBinary.WriteUInt8(Vertex.BoneIndices[3]);

			DataBinary.WriteFloat32(Vertex.Normal[0]);
			DataBinary.WriteFloat32(Vertex.Normal[1]);
			DataBinary.WriteFloat32(Vertex.Normal[2]);

			DataBinary.WriteFloat32(Vertex.Texture[0]);
			DataBinary.WriteFloat32(Vertex.Texture[1]);
		}
		TotalVertexCount += pSubsetOut->VertexCount;

		// write triangles
		UInt32 SubsetTriangleCountOut = pSubsetOut->TriangleIndexCount / 3;
		DataBinary.WriteUInt32(SubsetTriangleCountOut);

		// FMN 2015-01-26: changing TriangleIndexstart, depending on ID. See http://forums.darknestfantasyerotica.com/showthread.php?20446-TUTORIAL-Here-is-how-WoD-.skin-works.&p=402561
		UInt32 TriangleIndexStart = 0;

		TriangleIndexStart = UInt32(pSubsetOut->TriangleIndexStart) + (pSubsetOut->Level << 16);

		//UInt32 TriangleIndexEnd = pSubsetOut->TriangleIndexStart + pSubsetOut->TriangleIndexCount;
		//for (UInt32 k = pSubsetOut->TriangleIndexStart; k < TriangleIndexEnd; k++)
		UInt32 TriangleIndexEnd = TriangleIndexStart + pSubsetOut->TriangleIndexCount;
		for (UInt32 k = TriangleIndexStart; k < TriangleIndexEnd; k++)
		{
			UInt16 TriangleIndexOut = Triangles[k] - pSubsetOut->VertexStart;
			assert(TriangleIndexOut < pSubsetOut->VertexCount);
			DataBinary.WriteUInt16(TriangleIndexOut);
		}
		TotalTriangleCount += pSubsetOut->TriangleIndexCount;
	}

	// write bones
	DataBinary.WriteUInt32(BonesCount);
	for (UInt16 i = 0; i < BonesCount; i++)
	{
		CElement_Bone& Bone = Bones[i];

		DataBinary.WriteUInt16(i);
		DataBinary.WriteSInt16(Bone.ParentBone);
		DataBinary.WriteFloat32(Bone.Position[0]);
		DataBinary.WriteFloat32(Bone.Position[1]);
		DataBinary.WriteFloat32(Bone.Position[2]);
	}

	// write attachments
	DataBinary.WriteUInt32(AttachmentsCount);
	for (UInt16 i = 0; i < AttachmentsCount; i++)
	{
		CElement_Attachment& Attachment = Attachments[i];

		DataBinary.WriteUInt32(Attachment.ID);
		DataBinary.WriteSInt16(Attachment.ParentBone);
		DataBinary.WriteFloat32(Attachment.Position[0]);
		DataBinary.WriteFloat32(Attachment.Position[1]);
		DataBinary.WriteFloat32(Attachment.Position[2]);
		DataBinary.WriteFloat32(1.0f);
	}

	// write cameras
	DataBinary.WriteUInt32(CamerasCount);
	for (UInt16 i = 0; i < CamerasCount; i++)
	{
		CElement_Camera& Camera = Cameras[i];

		DataBinary.WriteSInt32(Camera.Type);

		// extract field of view of camera from animation block
		if (Camera.AnimationBlock_FieldOfView.nKeys > 0)
		{
			SExternalAnimation* ExternalAnimations = (SExternalAnimation*)Elements[EElement_Camera].GetLocalPointer(Camera.AnimationBlock_FieldOfView.oKeys);
			Float32* FieldOfView_Keys = (Float32*)(&RawData[ExternalAnimations[0].Offset]);
			DataBinary.WriteFloat32(FieldOfView_Keys[0]);
		}
		else
		{
			Float32 TempFoV = 0.785398163f;	// 45 degrees in radians, assuming that WoW stores camera FoV in radians. or maybe it's half FoV.
			DataBinary.WriteFloat32(TempFoV);
		}

		DataBinary.WriteFloat32(Camera.ClipFar);
		DataBinary.WriteFloat32(Camera.ClipNear);
		DataBinary.WriteFloat32(Camera.Position[0]);
		DataBinary.WriteFloat32(Camera.Position[1]);
		DataBinary.WriteFloat32(Camera.Position[2]);
		DataBinary.WriteFloat32(Camera.Target[0]);
		DataBinary.WriteFloat32(Camera.Target[1]);
		DataBinary.WriteFloat32(Camera.Target[2]);
	}

	FileStream.close();

	return M2Lib::EError_OK;
}

M2Lib::EError M2Lib::M2::ImportM2Intermediate(Char16* FileName, bool IgnoreBones, bool IgnoreAttachments, bool IgnoreCameras, bool FixSeams)
{
	Float32 SubmeshPositionalTolerance = 0.0001f;
	Float32 SubmeshAngularTolerance = 45.0f;
	Float32 BodyPositionalTolerance = 0.0001f;
	Float32 BodyAngularTolerance = 90.0f;
	Float32 ClothingPositionalTolerance = 0.0001f;
	Float32 ClothingAngularTolerance = 90.0f;

	if (!FileName)
		return EError_FailedToImportM2I_NoFileSpecified;

	// check that we have an M2 already loaded and ready to be injected
	if (!Header.Elements.nSkin)
		return M2Lib::EError_FailedToExportM2I_M2NotLoaded;

	// loaded M2I will be stored here.
	M2Lib::M2I InM2I;
	// loaded vertices will be stored here. this will become the new vertex list for the M2.
	std::vector< CVertex > NewVertexList;

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return M2Lib::EError_FailedToImportM2I_CouldNotOpenFile;
	M2Lib::DataBinary DataBinary(&FileStream, M2Lib::EEndianness_Little);

	// load signature
	UInt32 InSignature = 0;
	InSignature = DataBinary.ReadFourCC();
	if (InSignature != 1 && InSignature != Signature_M2I0)
		return M2Lib::EError_FailedToImportM2I_FileCorrupt;

	// load version
	UInt16 VersionMajor;
	UInt16 VersionMinor;
	if (InSignature == Signature_M2I0)
	{
		VersionMajor = DataBinary.ReadUInt16();
		VersionMinor = DataBinary.ReadUInt16();
		if (VersionMajor != 4 && VersionMinor != 5)
			return EError_FailedToImportM2I_UnsupportedVersion;
	}

	// load sub meshes, build new vertex list
	UInt32 VertexStart = 0;
	UInt32 InSubsetCount = 0;
	InSubsetCount = DataBinary.ReadUInt32();
	UInt32 iTriangle = 0;

	for (UInt32 i = 0; i < InSubsetCount; i++)
	{
		M2Lib::M2I::CSubMesh* pNewSubMesh = new M2Lib::M2I::CSubMesh();

		// read id
		pNewSubMesh->ID = DataBinary.ReadUInt16();
		pNewSubMesh->ComparisonData.ID = pNewSubMesh->ID;
		// FMN 2015-02-13: read level
		pNewSubMesh->Level = DataBinary.ReadUInt16();

		// read vertices
		UInt32 InVertexCount = 0;
		InVertexCount = DataBinary.ReadUInt32();
		if (NewVertexList.size() + InVertexCount > 0xFFFF)
		{
			return M2Lib::EError_FailedToImportM2I_TooManyVertices;
		}

		std::vector<CVertex> submeshVertices;
		for (UInt32 j = 0; j < InVertexCount; j++)
		{
			M2Lib::CVertex InVertex;

			InVertex.Position[0] = DataBinary.ReadFloat32();
			InVertex.Position[1] = DataBinary.ReadFloat32();
			InVertex.Position[2] = DataBinary.ReadFloat32();

			InVertex.BoneWeights[0] = DataBinary.ReadUInt8();
			InVertex.BoneWeights[1] = DataBinary.ReadUInt8();
			InVertex.BoneWeights[2] = DataBinary.ReadUInt8();
			InVertex.BoneWeights[3] = DataBinary.ReadUInt8();

			int WeightSum = InVertex.BoneWeights[0] + InVertex.BoneWeights[1] + InVertex.BoneWeights[2] + InVertex.BoneWeights[3];
			if (WeightSum < 255)
			{
				int pause = 1;
			}

			InVertex.BoneIndices[0] = DataBinary.ReadUInt8();
			InVertex.BoneIndices[1] = DataBinary.ReadUInt8();
			InVertex.BoneIndices[2] = DataBinary.ReadUInt8();
			InVertex.BoneIndices[3] = DataBinary.ReadUInt8();

			InVertex.Normal[0] = DataBinary.ReadFloat32();
			InVertex.Normal[1] = DataBinary.ReadFloat32();
			InVertex.Normal[2] = DataBinary.ReadFloat32();

			InVertex.Texture[0] = DataBinary.ReadFloat32();
			InVertex.Texture[1] = DataBinary.ReadFloat32();

			UInt16 VertexTriangleIndex = NewVertexList.size();
			NewVertexList.push_back(InVertex);
			pNewSubMesh->Indices.push_back(VertexTriangleIndex);

			submeshVertices.push_back(InVertex);
		}

		pNewSubMesh->ComparisonData.Boundary.Calculate(submeshVertices);

		// read triangles
		UInt32 InTriangleCount = 0;
		InTriangleCount = DataBinary.ReadUInt32();

		for (UInt32 j = 0; j < InTriangleCount; j++)
		{
			M2Lib::CTriangle NewTriangle;

			NewTriangle.TriangleIndex = iTriangle;
			iTriangle++;

			NewTriangle.Vertices[0] = DataBinary.ReadUInt16() + VertexStart;
			NewTriangle.Vertices[1] = DataBinary.ReadUInt16() + VertexStart;
			NewTriangle.Vertices[2] = DataBinary.ReadUInt16() + VertexStart;

			pNewSubMesh->Triangles.push_back(NewTriangle);
		}

		VertexStart += InVertexCount;

		InM2I.SubMeshList.push_back(pNewSubMesh);
	}

	if (!IgnoreBones)
	{
		// read bones, overwrite existing
		UInt32 BoneCount = Elements[EElement_Bone].Count;
		CElement_Bone* Bones = Elements[EElement_Bone].as<CElement_Bone>();
		UInt32 BoneCountIn;
		BoneCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < BoneCountIn; i++)
		{
			UInt16 InBoneIndex = 0;
			InBoneIndex = DataBinary.ReadUInt16();
			assert(InBoneIndex < BoneCount);
			CElement_Bone& BoneToMod = Bones[InBoneIndex];

			BoneToMod.ParentBone = DataBinary.ReadSInt16();

			BoneToMod.Position[0] = DataBinary.ReadFloat32();
			BoneToMod.Position[1] = DataBinary.ReadFloat32();
			BoneToMod.Position[2] = DataBinary.ReadFloat32();
		}
	}
	else
	{
		UInt32 BoneCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < BoneCountIn; ++i)
		{
			DataBinary.ReadUInt16();
			DataBinary.ReadSInt16();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
		}
	}

	if (!IgnoreAttachments)
	{
		// read attachments, overwrite existing
		UInt32 AttachmentsCount = Elements[EElement_Attachment].Count;
		CElement_Attachment* Attachments = Elements[EElement_Attachment].as<CElement_Attachment>();
		UInt32 AttachmentCountIn;
		AttachmentCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < AttachmentCountIn; i++)
		{
			UInt32 InAttachmentID = 0;
			InAttachmentID = DataBinary.ReadUInt32();
			CElement_Attachment* pAttachmentToMod = 0;
			for (UInt32 j = 0; j < AttachmentsCount; j++)
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

				pAttachmentToMod->Position[0] = DataBinary.ReadFloat32();
				pAttachmentToMod->Position[1] = DataBinary.ReadFloat32();
				pAttachmentToMod->Position[2] = DataBinary.ReadFloat32();
				Float32 Scale = DataBinary.ReadFloat32();
			}
			else
			{
				DataBinary.ReadUInt16();

				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
				DataBinary.ReadFloat32();
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
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
			DataBinary.ReadFloat32();
		}
	}

	if (!IgnoreCameras)
	{
		// read cameras, overwrite existing
		UInt32 CameraCount = Elements[EElement_Camera].Count;
		CElement_Camera* Cameras = Elements[EElement_Camera].as<CElement_Camera>();
		UInt32 CameraCountIn;
		CameraCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < CameraCountIn; i++)
		{
			SInt32 InType;
			InType = DataBinary.ReadSInt32();

			CElement_Camera* pCameraToMod = 0;
			for (UInt32 j = 0; j < CameraCount; j++)
			{
				if (Cameras[j].Type == InType)
				{
					pCameraToMod = &Cameras[j];
					break;
				}
			}
			if (pCameraToMod)
			{
				pCameraToMod->Type = InType;

				if (pCameraToMod->AnimationBlock_FieldOfView.nKeys > 0)
				{
					SExternalAnimation* ExternalAnimations = (SExternalAnimation*)Elements[EElement_Camera].GetLocalPointer(pCameraToMod->AnimationBlock_FieldOfView.oKeys);
					Float32* FieldOfView_Keys = (Float32*)(&RawData[ExternalAnimations[0].Offset]);
					Float32 InFoV;
					InFoV = DataBinary.ReadFloat32();	// we are writing to RawData in memory, this change will not be saved to the M2 because it lies out of bounds of any elements.
					FieldOfView_Keys[0] = InFoV;
				}
				else
				{
					DataBinary.ReadFloat32();
				}
				pCameraToMod->ClipFar = DataBinary.ReadFloat32();
				pCameraToMod->ClipNear = DataBinary.ReadFloat32();
				pCameraToMod->Position[0] = DataBinary.ReadFloat32();
				pCameraToMod->Position[1] = DataBinary.ReadFloat32();
				pCameraToMod->Position[2] = DataBinary.ReadFloat32();
				pCameraToMod->Target[0] = DataBinary.ReadFloat32();
				pCameraToMod->Target[1] = DataBinary.ReadFloat32();
				pCameraToMod->Target[2] = DataBinary.ReadFloat32();
			}
			else
			{
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
	}
	else
	{
		UInt32 CameraCountIn = DataBinary.ReadUInt32();
		for (UInt32 i = 0; i < CameraCountIn; ++i)
		{
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

	// copy new vertex list from M2I to M2
	Elements[EElement_Vertex].SetDataSize(NewVertexList.size(), NewVertexList.size() * sizeof(CVertex), false);
	memcpy(Elements[EElement_Vertex].Data.data(), &NewVertexList[0], NewVertexList.size() * sizeof(CVertex));

	BoundaryData GlobalBoundary;
	GlobalBoundary.Calculate(NewVertexList);

	// fix seams
	// this is hacky, but we gotta fix seams first
	// build skin 0
	// only build skin 0 for now, so we can fix seams and then build skin for real later
	M2Lib::M2SkinBuilder SkinBuilder;
	std::vector< UInt16 > NewBoneLookup;
	SInt32 BoneStart = 0;
	M2Lib::M2Skin* pNewSkin0 = new M2Lib::M2Skin(this);
	assert(SkinBuilder.Build(pNewSkin0, 256, &InM2I, &NewVertexList[0], BoneStart));

	// set skin 0 so we can begin seam fixing
	M2Lib::M2Skin* pOriginalSkin0 = Skins[0];	// save this because we will need to copy materials from it later.
	Header.Elements.nSkin = 1;
	for (UInt32 i = 0; i < 4; i++)
	{
		if (i != 0 && Skins[i])
		{
			delete Skins[i];
		}
		Skins[i] = i == 0 ? pNewSkin0 : 0;
	}

	if (FixSeams)
	{
		// fix normals within submeshes
		FixSeamsSubMesh(SubmeshPositionalTolerance, SubmeshAngularTolerance * DegreesToRadians);

		// fix normals between body submeshes
		FixSeamsBody(BodyPositionalTolerance, BodyAngularTolerance * DegreesToRadians);

		// close gaps between clothes and body
		FixSeamsClothing(ClothingPositionalTolerance, ClothingAngularTolerance * DegreesToRadians);
	}

	//
	//
	//
	// build skins for real this time
	// because a few bone indices might have changed during seam and gap fixing
	// this list will store the new skins
	M2Lib::M2Skin* NewSkinList[4];
	NewSkinList[0] = 0;
	NewSkinList[1] = 0;
	NewSkinList[2] = 0;
	NewSkinList[3] = 0;

	// for easy looping
	UInt32 MaxBoneList[5];
	MaxBoneList[0] = 256;
	MaxBoneList[1] = 64;
	MaxBoneList[2] = 53;
	MaxBoneList[3] = 21;
	MaxBoneList[4] = 0;		// extra entry needed for LoD check

	NewBoneLookup.clear();
	BoneStart = 0;
	UInt32 iSkin = 0;

	for (UInt32 iLoD = 0; iLoD < 4; iLoD++)
	{
		M2Lib::M2Skin* pNewSkin = new M2Lib::M2Skin(this);
		assert(SkinBuilder.Build(pNewSkin, MaxBoneList[iLoD], &InM2I, Elements[EElement_Vertex].as<M2Lib::CVertex>(), BoneStart));
		// if there are more bones than the next lowest level of detail
		if (SkinBuilder.m_Bones.size() > MaxBoneList[iLoD + 1])
		{
			// copy skin to result list
			NewSkinList[iSkin] = pNewSkin;
			iSkin++;

			// copy skin's bone lookup to the global bone lookup list
			for (UInt32 i = 0; i < SkinBuilder.m_Bones.size(); i++)
				NewBoneLookup.push_back(SkinBuilder.m_Bones[i]);

			// advance for where next skin's bone lookup will begin
			BoneStart += SkinBuilder.m_Bones.size();
		}
		else
		{
			// this skin does not have enough bones and so it is not needed because the next lowest level of detail can contain the whole thing just fine, so discard this skin.
			delete pNewSkin;
		}
	}

	// set skin count
	Header.Elements.nSkin = iSkin;
	if (iSkin == 0)
	{
		int pause = 1;
	}

	// copy materials from old sub meshes to new sub meshes
	for (UInt32 i = 0; i < 4; i++)
	{
		if (NewSkinList[i])
		{
			NewSkinList[i]->CopyMaterials(pOriginalSkin0);
			//NewSkinList[i]->SortSubMeshes();
		}
	}
	delete pOriginalSkin0;

	// replace old skins with new
	for (UInt32 i = 0; i < 4; i++)
	{
		if (Skins[i])
		{
			delete Skins[i];
		}
		Skins[i] = NewSkinList[i];
	}


	static_assert(sizeof(UInt16) == sizeof(CElement_BoneLookup), "");

	// copy new bone lookup
	Elements[EElement_SkinnedBoneLookup].SetDataSize(NewBoneLookup.size(), NewBoneLookup.size() * sizeof(UInt16), false);
	memcpy(Elements[EElement_SkinnedBoneLookup].Data.data(), &NewBoneLookup[0], NewBoneLookup.size() * sizeof(UInt16));

	// build vertex bone indices
	for (UInt32 i = 0; i != Header.Elements.nSkin; i++)
	{
		if (Skins[i])
		{
			Skins[i]->BuildVertexBoneIndices();
		}
	}

	// done, ready to be saved
	return M2Lib::EError_OK;
}


void M2Lib::M2::PrintInfo()
{
	//
	//
	// just print out any sort of data that we want to analize when troubleshooting

	UInt32 Count = 0;

	Char8 szItoA[32];

	Char16 szFileOut[1024];
	UInt32 Length = 0;
	while (_FileName[Length] != '\0')
	{
		szFileOut[Length] = _FileName[Length];
		Length++;
	}
	szFileOut[Length++] = '.';
	szFileOut[Length++] = 't';
	szFileOut[Length++] = 'x';
	szFileOut[Length++] = 't';
	szFileOut[Length++] = '\0';

	std::fstream FileStream;
	FileStream.open(szFileOut, std::ios::out | std::ios::trunc);

	FileStream << "ID       " << Header.Description.ID[0] << Header.Description.ID[1] << Header.Description.ID[2] << Header.Description.ID[3] << std::endl;		// 'MD20'
	FileStream << "Version  ";
	FileStream << _itoa_s((int)Header.Description.Version[0], szItoA, 32, 10) << " ";
	FileStream << _itoa_s((int)Header.Description.Version[1], szItoA, 32, 10) << " ";
	FileStream << _itoa_s((int)Header.Description.Version[2], szItoA, 32, 10) << " ";
	FileStream << _itoa_s((int)Header.Description.Version[3], szItoA, 32, 10) << std::endl;
	FileStream << std::endl;

	FileStream << "nName                     " << Header.Description.nName << std::endl;
	FileStream << "oName                     " << Header.Description.oName << std::endl;
	FileStream << " Value                    " << Elements[EElement_Name].as<Char8>() << std::endl;
	FileStream << std::endl;

	FileStream << "Flags                     " << Header.Description.Flags << std::endl;
	FileStream << std::endl;

	FileStream << "nGlobalSequences          " << Header.Elements.nGlobalSequence << std::endl;
	FileStream << "oGlobalSequences          " << Header.Elements.oGlobalSequence << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_GlobalSequence].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAnimations               " << Header.Elements.nAnimation << std::endl;
	FileStream << "oAnimations               " << Header.Elements.oAnimation << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Animation].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAnimationsLookup         " << Header.Elements.nAnimationLookup << std::endl;
	FileStream << "oAnimationsLookup         " << Header.Elements.oAnimationLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_AnimationLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBones                    " << Header.Elements.nBone << std::endl;
	FileStream << "oBones                    " << Header.Elements.oBone << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Bone].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nKeyBoneLookup            " << Header.Elements.nKeyBoneLookup << std::endl;
	FileStream << "oKeyBoneLookup            " << Header.Elements.oKeyBoneLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_KeyBoneLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nVertices                 " << Header.Elements.nVertex << std::endl;
	FileStream << "oVertices                 " << Header.Elements.oVertex << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Vertex].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nViews                    " << Header.Elements.nSkin << std::endl;
	FileStream << std::endl;

	FileStream << "nColors                   " << Header.Elements.nColor << std::endl;
	FileStream << "oColors                   " << Header.Elements.oColor << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Color].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextures                 " << Header.Elements.nTexture << std::endl;
	FileStream << "oTextures                 " << Header.Elements.oTexture << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Texture].Data.size() << std::endl;
	FileStream << std::endl;

    for (int i = 0; i < Header.Elements.nTexture; ++i)
    {
		CElement_Texture* texture = Elements[EElement_Texture].as<CElement_Texture>();

        FileStream << "\t" << i << std::endl;
        FileStream << "\tFlags: " << texture[i].Flags << std::endl;
        FileStream << "\tType: " << texture[i].Type << std::endl;
        if (texture[i].nTexturePath > 1)
            FileStream << "\tPath: " << (char*)(texture[i].oTexturePath + this->RawData + M2Element::GetFileOffset()) << std::endl;
        else
            FileStream << "\tPath: unk" << std::endl;
        FileStream << std::endl;
    }

	FileStream << "nTransparencies           " << Header.Elements.nTransparency << std::endl;
	FileStream << "oTransparencies           " << Header.Elements.oTransparency << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Transparency].Data.size() << std::endl;

	CElement_Transparency* Transparencies = Elements[EElement_Transparency].as<CElement_Transparency>();
    for (auto i = 0; i < Header.Elements.nTransparency; ++i)
    {
        auto transparency = Transparencies[i];
        FileStream << "\t" << i << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.InterpolationType << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.GlobalSequenceID << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.nTimes << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.oTimes << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.nKeys << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.oKeys << std::endl;

            /*
            EInterpolationType InterpolationType;
            SInt16 GlobalSequenceID;
            UInt32 nTimes;
            UInt32 oTimes;
            UInt32 nKeys;
            UInt32 oKeys;
            */
    }
	FileStream << std::endl;

	FileStream << "nTextureAnimation         " << Header.Elements.nTextureAnimation << std::endl;
	FileStream << "oTextureAnimation         " << Header.Elements.nTextureAnimation << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureAnimation].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureReplace           " << Header.Elements.nTextureReplace << std::endl;
	FileStream << "oTextureReplace           " << Header.Elements.oTextureReplace << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureReplace].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureFlags             " << Header.Elements.nTextureFlags << std::endl;
	FileStream << "oTextureFlags             " << Header.Elements.oTextureFlags << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureFlags].Data.size() << std::endl;
	CElement_TextureFlag* TextureFlags = Elements[EElement_TextureFlags].as<CElement_TextureFlag>();
    for (auto i = 0; i < Header.Elements.nTransparency; ++i)
    {
        auto flag = TextureFlags[i];
        FileStream << "\t-- " << i << std::endl;
        FileStream << "\t" << flag.Flags << std::endl;
        FileStream << "\t" << flag.Blend << std::endl;
    }
	FileStream << std::endl;

	FileStream << "nSkinnedBoneLookup        " << Header.Elements.nSkinnedBoneLookup << std::endl;
	FileStream << "oSkinnedBoneLookup        " << Header.Elements.oSkinnedBoneLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_SkinnedBoneLookup].Data.size() << std::endl;
/*    EElement_SkinnedBoneLookup* SkinnedBonesLookup = (CElement_TextuEElement_SkinnedBoneLookupreFlag*)Elements[EElement_SkinnedBoneLookup].Data;
    for (auto i = 0; i < Header.Elements.nTransparency; ++i)
    {
        auto flag = TextureFlags[i];
        FileStream << "\t-- " << i << std::endl;
        FileStream << "\t" << flag.Flags << std::endl;
        FileStream << "\t" << flag.Blend << std::endl;
    }*/
	FileStream << std::endl;

	FileStream << "nTexturesLookup           " << Header.Elements.nTextureLookup << std::endl;
	FileStream << "oTexturesLookup           " << Header.Elements.oTextureLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureLookup].Data.size() << std::endl;
	FileStream << std::endl;

    for (int i = 0; i < Header.Elements.nTexture; ++i)
    {
        CElement_TextureLookup* textureLookup = Elements[EElement_TextureLookup].as<CElement_TextureLookup>();

        FileStream << "\t" << i << std::endl;
        FileStream << "\tIndex: " << textureLookup[i].TextureIndex << std::endl;
        FileStream << std::endl;
    }

	FileStream << "nTextureUnitsLookup       " << Header.Elements.nTextureUnitLookup << std::endl;
	FileStream << "oTextureUnitsLookup       " << Header.Elements.oTextureUnitLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureUnitLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTransparenciesLookup     " << Header.Elements.nTransparencyLookup << std::endl;
	FileStream << "oTransparenciesLookup     " << Header.Elements.oTransparencyLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TransparencyLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureAnimationsLookup  " << Header.Elements.nTextureAnimationLookup << std::endl;
	FileStream << "oTextureAnimationsLookup  " << Header.Elements.oTextureAnimationLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureAnimationLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "Volumes " << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min[0] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min[1] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min[2] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max[0] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max[1] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max[2] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Radius << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min[0] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min[1] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min[2] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max[0] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max[1] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max[2] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Radius << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingTriangles        " << Header.Elements.nBoundingTriangle << std::endl;
	FileStream << "oBoundingTriangles        " << Header.Elements.oBoundingTriangle << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingTriangle].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingVertices         " << Header.Elements.nBoundingVertex << std::endl;
	FileStream << "oBoundingVertices         " << Header.Elements.oBoundingVertex << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingVertex].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingNormals          " << Header.Elements.nBoundingNormal << std::endl;
	FileStream << "oBoundingNormals          " << Header.Elements.oBoundingNormal << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingNormal].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAttachments              " << Header.Elements.nAttachment << std::endl;
	FileStream << "oAttachments              " << Header.Elements.oAttachment << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Attachment].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAttachmentsLookup        " << Header.Elements.nAttachmentLookup << std::endl;
	FileStream << "oAttachmentsLookup        " << Header.Elements.oAttachmentLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_AttachmentLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nEvents                   " << Header.Elements.nEvent << std::endl;
	FileStream << "oEvents                   " << Header.Elements.oEvent << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Event].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nLights                   " << Header.Elements.nLight << std::endl;
	FileStream << "oLights                   " << Header.Elements.oLight << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Light].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nCameras                  " << Header.Elements.nCamera << std::endl;
	FileStream << "oCameras                  " << Header.Elements.oCamera << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Camera].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nCamerasLookup            " << Header.Elements.nCameraLookup << std::endl;
	FileStream << "oCamerasLookup            " << Header.Elements.oCameraLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_CameraLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nRibbonEmitters           " << Header.Elements.nRibbonEmitter << std::endl;
	FileStream << "oRibbonEmitters           " << Header.Elements.oRibbonEmitter << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_RibbonEmitter].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nParticleEmitters         " << Header.Elements.nParticleEmitter << std::endl;
	FileStream << "oParticleEmitters         " << Header.Elements.oParticleEmitter << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_ParticleEmitter].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nUnknown1                 " << Header.Elements.nUnknown1 << std::endl;
	FileStream << "oUnknown1                 " << Header.Elements.oUnknown1 << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Unknown1].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream.close();
}


// Gets the .skin file names.
bool M2Lib::M2::GetFileSkin(Char16* SkinFileNameResultBuffer, const Char16* M2FileName, UInt32 SkinTriangleIndex)
{
	// ghetto string ops
	SInt32 LastDot = -1;
	UInt32 Length = 0;
	for (; Length < 1024; Length++)
	{
		if (M2FileName[Length] == L'\0')
		{
			break;
		}
		else if (M2FileName[Length] == L'.')
		{
			LastDot = Length;
		}
	}
	if ((LastDot == -1) || (LastDot > 1018))
	{
		return false;
	}
	UInt32 j = 0;
	for (; j != LastDot; j++)
	{
		SkinFileNameResultBuffer[j] = M2FileName[j];
	}
	switch (SkinTriangleIndex)
	{
	case 0:
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'0';
		break;
	case 1:
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'1';
		break;
	case 2:
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'2';
		break;
	case 3:
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'3';
		break;
	case 4:
		SkinFileNameResultBuffer[j++] = L'_';
		SkinFileNameResultBuffer[j++] = L'L';
		SkinFileNameResultBuffer[j++] = L'O';
		SkinFileNameResultBuffer[j++] = L'D';
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'1';
		break;
	case 5:
		SkinFileNameResultBuffer[j++] = L'_';
		SkinFileNameResultBuffer[j++] = L'L';
		SkinFileNameResultBuffer[j++] = L'O';
		SkinFileNameResultBuffer[j++] = L'D';
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'2';
		break;
	}
	SkinFileNameResultBuffer[j++] = L'.';
	SkinFileNameResultBuffer[j++] = L's';
	SkinFileNameResultBuffer[j++] = L'k';
	SkinFileNameResultBuffer[j++] = L'i';
	SkinFileNameResultBuffer[j++] = L'n';
	SkinFileNameResultBuffer[j] = L'\0';

	return true;
}


void M2Lib::M2::FixSeamsSubMesh(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	// gather up sub meshes
	std::vector< std::vector< M2Skin::CElement_SubMesh* > > SubMeshes;

	M2Lib::M2Skin::CElement_SubMesh* Subsets = Skins[0]->Elements[M2Skin::EElement_SubMesh].as<M2Skin::CElement_SubMesh>();
	UInt32 SubsetCount = Skins[0]->Elements[M2Lib::M2Skin::EElement_SubMesh].Count;
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		UInt16 ThisID = Subsets[i].ID;
		bool MakeNew = true;
		for (UInt32 j = 0; j < SubMeshes.size(); j++)
		{
			for (UInt32 k = 0; k < SubMeshes[j].size(); k++)
			{
				if (SubMeshes[j][k]->ID == ThisID)
				{
					MakeNew = false;
					SubMeshes[j].push_back(&Subsets[i]);
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
			std::vector< M2Skin::CElement_SubMesh* > NewSubmeshSubsetList;
			NewSubmeshSubsetList.push_back(&Subsets[i]);
			SubMeshes.push_back(NewSubmeshSubsetList);
		}
	}

	// find and merge duplicate vertices
	UInt32 VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (UInt32 iSubMesh1 = 0; iSubMesh1 < SubMeshes.size(); iSubMesh1++)
	{
		for (UInt32 iSubSet1 = 0; iSubSet1 < SubMeshes[iSubMesh1].size(); iSubSet1++)
		{
			M2Skin::CElement_SubMesh* pSubSet1 = SubMeshes[iSubMesh1][iSubSet1];

			UInt32 VertexAEnd = pSubSet1->VertexStart + pSubSet1->VertexCount;
			for (UInt32 iVertexA = pSubSet1->VertexStart; iVertexA < VertexAEnd; iVertexA++)
			{
				bool AddedVertexA = false;
				for (UInt32 iSubSet2 = 0; iSubSet2 < SubMeshes[iSubMesh1].size(); iSubSet2++)
				{
					M2Skin::CElement_SubMesh* pSubSet2 = SubMeshes[iSubMesh1][iSubSet2];

					UInt32 VertexBEnd = pSubSet2->VertexStart + pSubSet2->VertexCount;
					for (UInt32 iVertexB = pSubSet2->VertexStart; iVertexB < VertexBEnd; iVertexB++)
					{
						if (iVertexA == iVertexB)
						{
							continue;
						}

						if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, PositionalTolerance, AngularTolerance))
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
					Float32 NewPosition[3];
					NewPosition[0] = 0.0f;
					NewPosition[1] = 0.0f;
					NewPosition[2] = 0.0f;

					Float32 NewNormal[3];
					NewNormal[0] = 0.0f;
					NewNormal[1] = 0.0f;
					NewNormal[2] = 0.0f;

					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						M2Lib::CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition[0] += pSimilarVertex->Position[0];
						NewPosition[1] += pSimilarVertex->Position[1];
						NewPosition[2] += pSimilarVertex->Position[2];

						NewNormal[0] += pSimilarVertex->Normal[0];
						NewNormal[1] += pSimilarVertex->Normal[1];
						NewNormal[2] += pSimilarVertex->Normal[2];
					}

					// average position and normalize normal
					Float32 invSimilarCount = 1.f / (Float32)SimilarVertices.size();

					NewPosition[0] *= invSimilarCount;
					NewPosition[1] *= invSimilarCount;
					NewPosition[2] *= invSimilarCount;

					NewNormal[0] *= invSimilarCount;
					NewNormal[1] *= invSimilarCount;
					NewNormal[2] *= invSimilarCount;

					UInt8 NewBoneWeights[4];
					NewBoneWeights[0] = SimilarVertices[0]->BoneWeights[0];
					NewBoneWeights[1] = SimilarVertices[0]->BoneWeights[1];
					NewBoneWeights[2] = SimilarVertices[0]->BoneWeights[2];
					NewBoneWeights[3] = SimilarVertices[0]->BoneWeights[3];

					UInt8 NewBoneIndices[4];
					NewBoneIndices[0] = SimilarVertices[0]->BoneIndices[0];
					NewBoneIndices[1] = SimilarVertices[0]->BoneIndices[1];
					NewBoneIndices[2] = SimilarVertices[0]->BoneIndices[2];
					NewBoneIndices[3] = SimilarVertices[0]->BoneIndices[3];

					// assign new values back into similar vertices
					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						M2Lib::CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position[0] = NewPosition[0];
						pSimilarVertex->Position[1] = NewPosition[1];
						pSimilarVertex->Position[2] = NewPosition[2];

						pSimilarVertex->BoneWeights[0] = NewBoneWeights[0];
						pSimilarVertex->BoneWeights[1] = NewBoneWeights[1];
						pSimilarVertex->BoneWeights[2] = NewBoneWeights[2];
						pSimilarVertex->BoneWeights[3] = NewBoneWeights[3];

						pSimilarVertex->BoneIndices[0] = NewBoneIndices[0];
						pSimilarVertex->BoneWeights[1] = NewBoneWeights[1];
						pSimilarVertex->BoneWeights[2] = NewBoneWeights[2];
						pSimilarVertex->BoneWeights[3] = NewBoneWeights[3];

						pSimilarVertex->Normal[0] = NewNormal[0];
						pSimilarVertex->Normal[1] = NewNormal[1];
						pSimilarVertex->Normal[2] = NewNormal[2];
					}

					SimilarVertices.clear();
				}
			}
		}
	}
}


void M2Lib::M2::FixSeamsBody(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	// sub meshes that are divided up accross multiple bone partitions will have multiple sub mesh entries with the same ID in the M2.
	// we need to gather each body submesh up into a list and average normals of vertices that are similar between other sub meshes.
	// this function is designed to be used on character models, so it may not work on other models.

	// list of submeshes that make up the body of the character
	std::vector< std::vector< M2Lib::M2Skin::CElement_SubMesh* > > CompiledSubMeshList;

	// gather up the body submeshes
	M2Lib::M2Skin::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2Lib::M2Skin::EElement_SubMesh].as<M2Lib::M2Skin::CElement_SubMesh>();
	UInt32 SubsetCount = Skins[0]->Elements[M2Lib::M2Skin::EElement_SubMesh].Count;
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		// determine type of subset
		UInt16 ThisID = SubMeshList[i].ID;
		UInt16 Mod = ThisID;
		while (Mod > 10)
		{
			Mod %= 10;
		}
		if ((ThisID == 0) || (ThisID > 10 && Mod == 1) || (ThisID == 702))
		{
			// this subset is part of the character's body
			// add it to the list of submeshes
			bool MakeNew = true;
			for (UInt32 j = 0; j < CompiledSubMeshList.size(); j++)
			{
				for (UInt32 k = 0; k < CompiledSubMeshList[j].size(); k++)
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
				std::vector< M2Lib::M2Skin::CElement_SubMesh* > NewSubmeshSubsetList;
				NewSubmeshSubsetList.push_back(&SubMeshList[i]);
				CompiledSubMeshList.push_back(NewSubmeshSubsetList);
			}
		}
	}

	// find and merge duplicate vertices
	UInt32 VertexListLength = Elements[EElement_Vertex].Count;
	M2Lib::CVertex* VertexList = Elements[EElement_Vertex].as<M2Lib::CVertex>();
	std::vector< M2Lib::CVertex* > SimilarVertices;
	for (SInt32 iSubMesh1 = 0; iSubMesh1 < (SInt32)CompiledSubMeshList.size() - 1; iSubMesh1++)
	{
		for (SInt32 iSubSet1 = 0; iSubSet1 < (SInt32)CompiledSubMeshList[iSubMesh1].size(); iSubSet1++)
		{
			// gather duplicate vertices
			// for each vertex in the subset, compare it against vertices in the other subsets
			// find duplicates and sum their normals
			UInt32 iVertexAEnd = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart + CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexCount;
			for (UInt32 iVertexA = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart; iVertexA < iVertexAEnd; iVertexA++)
			{
				// gather duplicate vertices from other submeshes
				bool AddedVertex1 = false;
				for (SInt32 iSubMesh2 = iSubMesh1 + 1; iSubMesh2 < (SInt32)CompiledSubMeshList.size(); iSubMesh2++)
				{
					// check that we don't check against ourselves
					if (iSubMesh2 == iSubMesh1)
					{
						// other submesh is same as this submesh
						continue;
					}
					// go through subsets
					for (SInt32 iSubSet2 = 0; iSubSet2 < (SInt32)CompiledSubMeshList[iSubMesh2].size(); iSubSet2++)
					{
						// go through vertices in subset
						UInt32 iVertexBEnd = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart + CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexCount;
						for (UInt32 iVertexB = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart; iVertexB < iVertexBEnd; iVertexB++)
						{
							if (M2Lib::CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, PositionalTolerance, AngularTolerance))
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
					Float32 NewPosition[3];
					NewPosition[0] = 0.0f;
					NewPosition[1] = 0.0f;
					NewPosition[2] = 0.0f;

					Float32 NewNormal[3];
					NewNormal[0] = 0.0f;
					NewNormal[1] = 0.0f;
					NewNormal[2] = 0.0f;

					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						M2Lib::CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition[0] += pSimilarVertex->Position[0];
						NewPosition[1] += pSimilarVertex->Position[1];
						NewPosition[2] += pSimilarVertex->Position[2];

						NewNormal[0] += pSimilarVertex->Normal[0];
						NewNormal[1] += pSimilarVertex->Normal[1];
						NewNormal[2] += pSimilarVertex->Normal[2];
					}

					// average position and normalize normal
					Float32 invSimilarCount = 1.0f / (Float32)SimilarVertices.size();

					NewPosition[0] *= invSimilarCount;
					NewPosition[1] *= invSimilarCount;
					NewPosition[2] *= invSimilarCount;

					NewNormal[0] *= invSimilarCount;
					NewNormal[1] *= invSimilarCount;
					NewNormal[2] *= invSimilarCount;

					UInt8 NewBoneWeights[4];
					NewBoneWeights[0] = SimilarVertices[0]->BoneWeights[0];
					NewBoneWeights[1] = SimilarVertices[0]->BoneWeights[1];
					NewBoneWeights[2] = SimilarVertices[0]->BoneWeights[2];
					NewBoneWeights[3] = SimilarVertices[0]->BoneWeights[3];

					UInt8 NewBoneIndices[4];
					NewBoneIndices[0] = SimilarVertices[0]->BoneIndices[0];
					NewBoneIndices[1] = SimilarVertices[0]->BoneIndices[1];
					NewBoneIndices[2] = SimilarVertices[0]->BoneIndices[2];
					NewBoneIndices[3] = SimilarVertices[0]->BoneIndices[3];

					// assign new values back into similar vertices
					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						M2Lib::CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position[0] = NewPosition[0];
						pSimilarVertex->Position[1] = NewPosition[1];
						pSimilarVertex->Position[2] = NewPosition[2];

						pSimilarVertex->Normal[0] = NewNormal[0];
						pSimilarVertex->Normal[1] = NewNormal[1];
						pSimilarVertex->Normal[2] = NewNormal[2];

						pSimilarVertex->BoneWeights[0] = NewBoneWeights[0];
						pSimilarVertex->BoneWeights[1] = NewBoneWeights[1];
						pSimilarVertex->BoneWeights[2] = NewBoneWeights[2];
						pSimilarVertex->BoneWeights[3] = NewBoneWeights[3];

						pSimilarVertex->BoneIndices[0] = NewBoneIndices[0];
						pSimilarVertex->BoneIndices[1] = NewBoneIndices[1];
						pSimilarVertex->BoneIndices[2] = NewBoneIndices[2];
						pSimilarVertex->BoneIndices[3] = NewBoneIndices[3];
					}

					// clear list
					SimilarVertices.clear();
				}
			}
		}
	}
}


void M2Lib::M2::FixSeamsClothing(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();

	UInt32 SubMeshListLength = Skins[0]->Elements[M2Skin::EElement_SubMesh].Count;
	M2Skin::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2Skin::EElement_SubMesh].as<M2Skin::CElement_SubMesh>();

	std::vector< M2Skin::CElement_SubMesh* > SubMeshBodyList;	// gathered body sub meshes
	std::vector< M2Skin::CElement_SubMesh* > SubMeshGarbList;	// gathered clothing sub meshes

	for (UInt32 i = 0; i < SubMeshListLength; i++)
	{
		UInt16 ThisID = SubMeshList[i].ID;

		// gather body sub meshes
		UInt16 Mod = ThisID;
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
	for (UInt32 iSubMeshGarb = 0; iSubMeshGarb < SubMeshGarbList.size(); iSubMeshGarb++)
	{
		M2Skin::CElement_SubMesh* pSubMeshGarb = SubMeshGarbList[iSubMeshGarb];
		for (UInt32 iSubMeshBody = 0; iSubMeshBody < SubMeshBodyList.size(); iSubMeshBody++)
		{
			M2Skin::CElement_SubMesh* pSubMeshBody = SubMeshBodyList[iSubMeshBody];

			for (SInt32 iVertexGarb = pSubMeshGarb->VertexStart; iVertexGarb < pSubMeshGarb->VertexStart + pSubMeshGarb->VertexCount; iVertexGarb++)
			{
				for (SInt32 iVertexBody = pSubMeshBody->VertexStart; iVertexBody < pSubMeshBody->VertexStart + pSubMeshBody->VertexCount; iVertexBody++)
				{
					if (CVertex::CompareSimilar(VertexList[iVertexGarb], VertexList[iVertexBody], false, false, PositionalTolerance, AngularTolerance))
					{
						// copy position, normal, and bone weights, and bone indices from body vertex to other(clothing) vertex
						CVertex* pVertexOther = &VertexList[iVertexGarb];
						CVertex* pVertexBody = &VertexList[iVertexBody];

						pVertexOther->Position[0] = pVertexBody->Position[0];
						pVertexOther->Position[1] = pVertexBody->Position[1];
						pVertexOther->Position[2] = pVertexBody->Position[2];

						pVertexOther->Normal[0] = pVertexBody->Normal[0];
						pVertexOther->Normal[1] = pVertexBody->Normal[1];
						pVertexOther->Normal[2] = pVertexBody->Normal[2];

						pVertexOther->BoneWeights[0] = pVertexBody->BoneWeights[0];
						pVertexOther->BoneWeights[1] = pVertexBody->BoneWeights[1];
						pVertexOther->BoneWeights[2] = pVertexBody->BoneWeights[2];
						pVertexOther->BoneWeights[3] = pVertexBody->BoneWeights[3];

						pVertexOther->BoneIndices[0] = pVertexBody->BoneIndices[0];
						pVertexOther->BoneIndices[1] = pVertexBody->BoneIndices[1];
						pVertexOther->BoneIndices[2] = pVertexBody->BoneIndices[2];
						pVertexOther->BoneIndices[3] = pVertexBody->BoneIndices[3];
					}
				}
			}
		}
	}
}


void M2Lib::M2::NormalizeBoneWeights()
{
}


void M2Lib::M2::Scale(Float32 Scale)
{
	// vertices
	{
		UInt32 VertexListLength = Elements[EElement_Vertex].Count;
		CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
		for (UInt32 i = 0; i < VertexListLength; i++)
		{
			CVertex& Vertex = VertexList[i];
			Vertex.Position[0] *= Scale;
			Vertex.Position[1] *= Scale;
			Vertex.Position[2] *= Scale;
		}
	}

	// bones
	{
		UInt32 BoneListLength = Elements[EElement_Bone].Count;
		CElement_Bone* BoneList = Elements[EElement_Bone].as<CElement_Bone>();
		for (UInt32 i = 0; i < BoneListLength; i++)
		{
			CElement_Bone& Bone = BoneList[i];
			Bone.Position[0] *= Scale;
			Bone.Position[1] *= Scale;
			Bone.Position[2] *= Scale;
		}
	}

	// attachments
	{
		UInt32 AttachmentListLength = Elements[EElement_Attachment].Count;
		CElement_Attachment* AttachmentList = Elements[EElement_Attachment].as<CElement_Attachment>();
		for (UInt32 i = 0; i < AttachmentListLength; i++)
		{
			CElement_Attachment& Attachment = AttachmentList[i];
			Attachment.Position[0] *= Scale;
			Attachment.Position[1] *= Scale;
			Attachment.Position[2] *= Scale;
		}
	}

	// events
	{
		UInt32 EventListLength = Elements[EElement_Event].Count;
		CElement_Event* EventList = Elements[EElement_Event].as<CElement_Event>();
		for (UInt32 i = 0; i < EventListLength; i++)
		{
			CElement_Event& Event = EventList[i];
			Event.Position[0] *= Scale;
			Event.Position[1] *= Scale;
			Event.Position[2] *= Scale;
		}
	}

	// lights
	{
		UInt32 LightListLength = Elements[EElement_Light].Count;
		CElement_Light* LightList = Elements[EElement_Light].as<CElement_Light>();
		for (UInt32 i = 0; i < LightListLength; i++)
		{
			CElement_Light& Light = LightList[i];
			Light.Position[0] *= Scale;
			Light.Position[1] *= Scale;
			Light.Position[2] *= Scale;
		}
	}

	// cameras
	{
		UInt32 CameraListLength = Elements[EElement_Camera].Count;
		CElement_Camera* CameraList = Elements[EElement_Camera].as<CElement_Camera>();
		for (UInt32 i = 0; i < CameraListLength; i++)
		{
			CElement_Camera& Camera = CameraList[i];
			Camera.Position[0] *= Scale;
			Camera.Position[1] *= Scale;
			Camera.Position[2] *= Scale;
			Camera.Target[0] *= Scale;
			Camera.Target[1] *= Scale;
			Camera.Target[2] *= Scale;
		}
	}

	// ribbon emitters
	{
		UInt32 RibbonEmitterListLength = Elements[EElement_RibbonEmitter].Count;
		CElement_RibbonEmitter* RibbonEmitterList = Elements[EElement_RibbonEmitter].as<CElement_RibbonEmitter>();
		for (UInt32 i = 0; i < RibbonEmitterListLength; i++)
		{
			CElement_RibbonEmitter& RibbonEmitter = RibbonEmitterList[i];
			RibbonEmitter.Position[0] *= Scale;
			RibbonEmitter.Position[1] *= Scale;
			RibbonEmitter.Position[2] *= Scale;
		}
	}

	// particle emitters
	{
		UInt32 ParticleEmitterListLength = Elements[EElement_ParticleEmitter].Count;
		CElement_ParticleEmitter* ParticleEmitterList = Elements[EElement_ParticleEmitter].as<CElement_ParticleEmitter>();
		for (UInt32 i = 0; i < ParticleEmitterListLength; i++)
		{
			CElement_ParticleEmitter& ParticleEmitter = ParticleEmitterList[i];
			ParticleEmitter.Position[0] *= Scale;
			ParticleEmitter.Position[1] *= Scale;
			ParticleEmitter.Position[2] *= Scale;
		}
	}
}


void M2Lib::M2::MirrorCamera()
{
	CElement_Camera* Cameras = Elements[EElement_Camera].as<CElement_Camera>();
	UInt32 CameraCount = Elements[EElement_Camera].Count;
	for (UInt32 iCamera = 0; iCamera < CameraCount; iCamera++)
	{
		if (Cameras->Type == 0)
		{
			Cameras->Position[1] = -Cameras->Position[1];
			Cameras->Target[1] = -Cameras->Target[1];
			break;
		}
	}

}


void M2Lib::M2::m_LoadElements_CopyHeaderToElements()
{
	Elements[EElement_Name].Count = Header.Description.nName;
	Elements[EElement_Name].Offset = Header.Description.oName;

	Elements[EElement_GlobalSequence].Count = Header.Elements.nGlobalSequence;
	Elements[EElement_GlobalSequence].Offset = Header.Elements.oGlobalSequence;

	Elements[EElement_Animation].Count = Header.Elements.nAnimation;
	Elements[EElement_Animation].Offset = Header.Elements.oAnimation;

	Elements[EElement_AnimationLookup].Count = Header.Elements.nAnimationLookup;
	Elements[EElement_AnimationLookup].Offset = Header.Elements.oAnimationLookup;

	Elements[EElement_Bone].Count = Header.Elements.nBone;
	Elements[EElement_Bone].Offset = Header.Elements.oBone;

	Elements[EElement_KeyBoneLookup].Count = Header.Elements.nKeyBoneLookup;
	Elements[EElement_KeyBoneLookup].Offset = Header.Elements.oKeyBoneLookup;

	Elements[EElement_Vertex].Count = Header.Elements.nVertex;
	Elements[EElement_Vertex].Offset = Header.Elements.oVertex;

	Elements[EElement_Color].Count = Header.Elements.nColor;
	Elements[EElement_Color].Offset = Header.Elements.oColor;

	Elements[EElement_Texture].Count = Header.Elements.nTexture;
	Elements[EElement_Texture].Offset = Header.Elements.oTexture;

	Elements[EElement_Transparency].Count = Header.Elements.nTransparency;
	Elements[EElement_Transparency].Offset = Header.Elements.oTransparency;

	Elements[EElement_TextureAnimation].Count = Header.Elements.nTextureAnimation;
	Elements[EElement_TextureAnimation].Offset = Header.Elements.oTextureAnimation;

	Elements[EElement_TextureReplace].Count = Header.Elements.nTextureReplace;
	Elements[EElement_TextureReplace].Offset = Header.Elements.oTextureReplace;

	Elements[EElement_TextureFlags].Count = Header.Elements.nTextureFlags;
	Elements[EElement_TextureFlags].Offset = Header.Elements.oTextureFlags;

	Elements[EElement_SkinnedBoneLookup].Count = Header.Elements.nSkinnedBoneLookup;
	Elements[EElement_SkinnedBoneLookup].Offset = Header.Elements.oSkinnedBoneLookup;

	Elements[EElement_TextureLookup].Count = Header.Elements.nTextureLookup;
	Elements[EElement_TextureLookup].Offset = Header.Elements.oTextureLookup;

	Elements[EElement_TextureUnitLookup].Count = Header.Elements.nTextureUnitLookup;
	Elements[EElement_TextureUnitLookup].Offset = Header.Elements.oTextureUnitLookup;

	Elements[EElement_TransparencyLookup].Count = Header.Elements.nTransparencyLookup;
	Elements[EElement_TransparencyLookup].Offset = Header.Elements.oTransparencyLookup;

	Elements[EElement_TextureAnimationLookup].Count = Header.Elements.nTextureAnimationLookup;
	Elements[EElement_TextureAnimationLookup].Offset = Header.Elements.oTextureAnimationLookup;

	Elements[EElement_BoundingTriangle].Count = Header.Elements.nBoundingTriangle;
	Elements[EElement_BoundingTriangle].Offset = Header.Elements.oBoundingTriangle;

	Elements[EElement_BoundingVertex].Count = Header.Elements.nBoundingVertex;
	Elements[EElement_BoundingVertex].Offset = Header.Elements.oBoundingVertex;

	Elements[EElement_BoundingNormal].Count = Header.Elements.nBoundingNormal;
	Elements[EElement_BoundingNormal].Offset = Header.Elements.oBoundingNormal;

	Elements[EElement_Attachment].Count = Header.Elements.nAttachment;
	Elements[EElement_Attachment].Offset = Header.Elements.oAttachment;

	Elements[EElement_AttachmentLookup].Count = Header.Elements.nAttachmentLookup;
	Elements[EElement_AttachmentLookup].Offset = Header.Elements.oAttachmentLookup;

	Elements[EElement_Event].Count = Header.Elements.nEvent;
	Elements[EElement_Event].Offset = Header.Elements.oEvent;

	Elements[EElement_Light].Count = Header.Elements.nLight;
	Elements[EElement_Light].Offset = Header.Elements.oLight;

	Elements[EElement_Camera].Count = Header.Elements.nCamera;
	Elements[EElement_Camera].Offset = Header.Elements.oCamera;

	Elements[EElement_CameraLookup].Count = Header.Elements.nCameraLookup;
	Elements[EElement_CameraLookup].Offset = Header.Elements.oCameraLookup;

	Elements[EElement_RibbonEmitter].Count = Header.Elements.nRibbonEmitter;
	Elements[EElement_RibbonEmitter].Offset = Header.Elements.oRibbonEmitter;

	Elements[EElement_ParticleEmitter].Count = Header.Elements.nParticleEmitter;
	Elements[EElement_ParticleEmitter].Offset = Header.Elements.oParticleEmitter;

	Elements[EElement_Unknown1].Count = Header.Elements.nUnknown1;
	Elements[EElement_Unknown1].Offset = Header.Elements.oUnknown1;
}


void M2Lib::M2::m_LoadElements_FindSizes(UInt32 FileSize)
{
	for (UInt32 i = 0; i != EElement__Count__; i++)
	{
		Elements[i].OffsetOriginal = Elements[i].Offset;

		if ((Elements[i].Count == 0) || (Elements[i].Offset == 0))
		{
			Elements[i].Data.clear();
		}
		else
		{
			UInt32 NextOffset = FileSize;
			for (UInt32 j = 0; j < EElement__Count__; j++)
			{
				if ((Elements[j].Count != 0) && (Elements[j].Offset > Elements[i].Offset))
				{
					if (Elements[j].Offset < NextOffset)
					{
						NextOffset = Elements[j].Offset;
					}
					break;
				}
			}
			Elements[i].Data.resize(NextOffset - Elements[i].Offset);
		}
	}
}


#define VERIFY_OFFSET_LOCAL( offset ) assert( ( Elements[iElement].Offset <= offset ) && ( offset < ( Elements[iElement].Offset + Elements[iElement].Data.size() ) ) );
#define VERIFY_OFFSET_NOTLOCAL( offset ) assert( offset > ( Elements[iElement].OffsetOriginal + Elements[iElement].Data.size() ) );

void M2Lib::M2::m_SaveElements_FindOffsets()
{
	// fix animation offsets and find element offsets
	SInt32 CurrentOffset = 0;
	if (Header.IsLongHeader())
		CurrentOffset = sizeof(CM2Header) + 8;	// +8 to align data at 16 byte bounds
	else
		CurrentOffset = sizeof(CM2Header) - 8;	// -8 because last 2 UInt32s are not saved

	// totaldiff needed to fix animations that are in the end of a chunk
	SInt32 totalDiff = -(SInt32)Chunks[EChunk_Model].Data.size() + (Header.IsLongHeader() ? sizeof(Header) : sizeof(Header) - 8);
	for (UInt32 iElement = 0; iElement < EElement__Count__; ++iElement)
		totalDiff += Elements[iElement].Data.size();

	SInt32 OffsetDelta = 0;
	for (UInt32 iElement = 0; iElement < EElement__Count__; ++iElement)
	{
		// if this element has data...
		if (Elements[iElement].Data.empty())
		{
			Elements[iElement].Offset = 0;
			continue;
		}

		// if the current element's current offset doesn't match the calculated offset, some data has resized and we need to fix...
		OffsetDelta = CurrentOffset - Elements[iElement].Offset;

		switch (iElement)
		{
			case EElement_Name:
			case EElement_GlobalSequence:
			case EElement_Animation:
			case EElement_AnimationLookup:
				break;

			case EElement_Bone:
			{
				CElement_Bone* Bones = Elements[iElement].as<CElement_Bone>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Rotation, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Scale, iElement);
				}
				break;
			}
			case EElement_KeyBoneLookup:
			case EElement_Vertex:
				break;

			case EElement_Color:
			{
				CElement_Color* Colors = Elements[iElement].as<CElement_Color>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Colors[j].AnimationBlock_Color, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Colors[j].AnimationBlock_Opacity, iElement);
				}
				break;
			}

			case EElement_Texture:
			{
				CElement_Texture* Textures = Elements[iElement].as<CElement_Texture>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					VERIFY_OFFSET_LOCAL(Textures[j].oTexturePath);
					Textures[j].oTexturePath += OffsetDelta;
				}
				break;
			}

			case EElement_Transparency:
			{
				CElement_Transparency* Transparencies = Elements[iElement].as<CElement_Transparency>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Transparencies[j].AnimationBlock_Transparency, iElement);
				}
				break;
			}
			case EElement_TextureAnimation:
			{
				CElement_UVAnimation* Animations = Elements[iElement].as<CElement_UVAnimation>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Rotation, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Scale, iElement);
				}
				break;
			}

			case EElement_TextureReplace:
			case EElement_TextureFlags:
			case EElement_SkinnedBoneLookup:
			case EElement_TextureLookup:
			case EElement_TextureUnitLookup:
			case EElement_TransparencyLookup:
			case EElement_TextureAnimationLookup:
			case EElement_BoundingTriangle:
			case EElement_BoundingVertex:
			case EElement_BoundingNormal:
				break;

			case EElement_Attachment:
			{
				CElement_Attachment* Attachments = Elements[iElement].as<CElement_Attachment>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Attachments[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_AttachmentLookup:
				break;

			case EElement_Event:
			{
				CElement_Event* Events = Elements[iElement].as<CElement_Event>();
				auto animations = Elements[EElement_Animation].as<CElement_Animation>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					if (Events[j].oTimeLines)
					{
						VERIFY_OFFSET_LOCAL(Events[j].oTimeLines);
						M2Array* M2Arrays = (M2Array*)Elements[iElement].GetLocalPointer(Events[j].oTimeLines);
						for (UInt32 i = 0; i < Events[j].nTimeLines; i++)
						{
							if (Events[j].GlobalSequenceID == -1 && (animations[i].Flags & 0x20) == 0)
								continue;

							M2Array& Array = M2Arrays[i];
							if (Array.Count && Array.Offset)
								Array.Offset += Events[j].GlobalSequenceID != -1 && (animations[i].Flags & 0x20) == 0 ?
									OffsetDelta : totalDiff;
							else
								Array.Offset = 0;
						}

						Events[j].oTimeLines += OffsetDelta;
					}
				}
				break;
			}

			case EElement_Light:
			{
				CElement_Light* Lights = Elements[iElement].as<CElement_Light>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AmbientColor, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AmbientIntensity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_DiffuseColor, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_DiffuseIntensity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AttenuationStart, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AttenuationEnd, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_Camera:
			{
				CElement_Camera* Cameras = Elements[iElement].as<CElement_Camera>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Target, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Roll, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_FieldOfView, iElement);
				}
				break;
			}

			case EElement_CameraLookup:
				break;

			case EElement_RibbonEmitter:
			{
				// untested!
				CElement_RibbonEmitter* RibbonEmitters = Elements[iElement].as<CElement_RibbonEmitter>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					VERIFY_OFFSET_LOCAL(RibbonEmitters[j].oTexture);
					RibbonEmitters[j].oTexture += OffsetDelta;
					VERIFY_OFFSET_LOCAL(RibbonEmitters[j].oRenderFlag);
					RibbonEmitters[j].oRenderFlag += OffsetDelta;

					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Color, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Opacity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Above, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Below, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Unknown1, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_ParticleEmitter:
			{
				CElement_ParticleEmitter* ParticleEmitters = Elements[iElement].as<CElement_ParticleEmitter>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					VERIFY_OFFSET_LOCAL(ParticleEmitters[j].oFileNameModel);
					ParticleEmitters[j].oFileNameModel += OffsetDelta;

					if (ParticleEmitters[j].nChildEmitter)
					{
						VERIFY_OFFSET_LOCAL(ParticleEmitters[j].oChildEmitter);
						ParticleEmitters[j].oChildEmitter += OffsetDelta;
					}
					else
						ParticleEmitters[j].oChildEmitter = 0;
					if (ParticleEmitters[j].nUnk)
					{
						VERIFY_OFFSET_LOCAL(ParticleEmitters[j].oUnk);
						ParticleEmitters[j].oUnk += OffsetDelta;
					}
					else
						ParticleEmitters[j].oUnk = 0;

					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitSpeed, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_SpeedVariance, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_VerticalRange, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_HorizontalRange, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_Gravity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_Lifespan, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitRate, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitLength, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitWidth, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_GravityStrong, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_Visibility, iElement);

					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].ColorTrack, iElement);
					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].AlphaTrack, iElement);
					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].ScaleTrack, iElement);
					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].HeadCellTrack, iElement);
					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].TailCellTrack, iElement);
				}
				break;
			}
		}

		// set the element's new offset
		Elements[iElement].Offset = CurrentOffset;
		CurrentOffset += Elements[iElement].Data.size();
	}
}

void M2Lib::M2::m_FixAnimationOffsets(SInt32 OffsetDelta, SInt32 TotalDiff, CElement_AnimationBlock& AnimationBlock, SInt32 iElement)
{
	auto animations = Elements[EElement_Animation].as<CElement_Animation>();

	// TP is the best
	if (AnimationBlock.nTimes)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.oTimes);
		M2Array* M2Arrays = (M2Array*)Elements[iElement].GetLocalPointer(AnimationBlock.oTimes);
		for (UInt32 i = 0; i < AnimationBlock.nTimes; i++)
		{
			if (AnimationBlock.GlobalSequenceID == -1 && (animations[i].Flags & 0x20) == 0)
				continue;

			M2Array& Array = M2Arrays[i];
			if (Array.Count && Array.Offset)
				Array.Offset += AnimationBlock.GlobalSequenceID != -1 && (animations[i].Flags & 0x20) == 0 ?
					OffsetDelta : TotalDiff;
			else
				Array.Offset = 0;
		}

		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.oTimes) && (AnimationBlock.oTimes < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.oTimes);
		assert(AnimationBlock.oTimes > 0);
		AnimationBlock.oTimes += OffsetDelta;
	}

	if (AnimationBlock.nKeys)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.oKeys);
		M2Array* M2Arrays = (M2Array*)Elements[iElement].GetLocalPointer(AnimationBlock.oKeys);
		for (UInt32 i = 0; i < AnimationBlock.nKeys; i++)
		{
			if (AnimationBlock.GlobalSequenceID == -1 && (animations[i].Flags & 0x20) == 0)
				continue;

			M2Array& Array = M2Arrays[i];
			if (Array.Count && Array.Offset)
				Array.Offset += AnimationBlock.GlobalSequenceID != -1 && (animations[i].Flags & 0x20) == 0 ?
					OffsetDelta : TotalDiff;
			else
				Array.Offset = 0;
		}

		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.oKeys) && (AnimationBlock.oKeys < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.oKeys);
		assert(AnimationBlock.oKeys > 0);
		AnimationBlock.oKeys += OffsetDelta;
	}
}

void M2Lib::M2::m_FixFakeAnimationBlockOffsets(SInt32 OffsetDelta, CElement_FakeAnimationBlock& AnimationBlock, SInt32 iElement)
{
	// TP is the best
	if (AnimationBlock.nTimes)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.oTimes);

		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.oTimes) && (AnimationBlock.oTimes < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.oTimes);
		assert(AnimationBlock.oTimes > 0);
		AnimationBlock.oTimes += OffsetDelta;
	}

	if (AnimationBlock.nKeys)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.oKeys);
		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.oKeys) && (AnimationBlock.oKeys < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.oKeys);
		assert(AnimationBlock.oKeys > 0);
		AnimationBlock.oKeys += OffsetDelta;
	}
}

void M2Lib::M2::m_SaveElements_CopyElementsToHeader()
{
	Header.Description.nName = Elements[EElement_Name].Count;
	Header.Description.oName = Elements[EElement_Name].Offset;

	Header.Elements.nGlobalSequence = Elements[EElement_GlobalSequence].Count;
	Header.Elements.oGlobalSequence = Elements[EElement_GlobalSequence].Offset;

	Header.Elements.nAnimation = Elements[EElement_Animation].Count;
	Header.Elements.oAnimation = Elements[EElement_Animation].Offset;

	Header.Elements.nAnimationLookup = Elements[EElement_AnimationLookup].Count;
	Header.Elements.oAnimationLookup = Elements[EElement_AnimationLookup].Offset;

	Header.Elements.nBone = Elements[EElement_Bone].Count;
	Header.Elements.oBone = Elements[EElement_Bone].Offset;

	Header.Elements.nKeyBoneLookup = Elements[EElement_KeyBoneLookup].Count;
	Header.Elements.oKeyBoneLookup = Elements[EElement_KeyBoneLookup].Offset;

	Header.Elements.nVertex = Elements[EElement_Vertex].Count;
	Header.Elements.oVertex = Elements[EElement_Vertex].Offset;

	Header.Elements.nColor = Elements[EElement_Color].Count;
	Header.Elements.oColor = Elements[EElement_Color].Offset;

	Header.Elements.nTexture = Elements[EElement_Texture].Count;
	Header.Elements.oTexture = Elements[EElement_Texture].Offset;

	Header.Elements.nTransparency = Elements[EElement_Transparency].Count;
	Header.Elements.oTransparency = Elements[EElement_Transparency].Offset;

	Header.Elements.nTextureAnimation = Elements[EElement_TextureAnimation].Count;
	Header.Elements.oTextureAnimation = Elements[EElement_TextureAnimation].Offset;

	Header.Elements.nTextureReplace = Elements[EElement_TextureReplace].Count;
	Header.Elements.oTextureReplace = Elements[EElement_TextureReplace].Offset;

	Header.Elements.nTextureFlags = Elements[EElement_TextureFlags].Count;
	Header.Elements.oTextureFlags = Elements[EElement_TextureFlags].Offset;

	Header.Elements.nSkinnedBoneLookup = Elements[EElement_SkinnedBoneLookup].Count;
	Header.Elements.oSkinnedBoneLookup = Elements[EElement_SkinnedBoneLookup].Offset;

	Header.Elements.nTextureLookup = Elements[EElement_TextureLookup].Count;
	Header.Elements.oTextureLookup = Elements[EElement_TextureLookup].Offset;

	Header.Elements.nTextureUnitLookup = Elements[EElement_TextureUnitLookup].Count;
	Header.Elements.oTextureUnitLookup = Elements[EElement_TextureUnitLookup].Offset;

	Header.Elements.nTransparencyLookup = Elements[EElement_TransparencyLookup].Count;
	Header.Elements.oTransparencyLookup = Elements[EElement_TransparencyLookup].Offset;

	Header.Elements.nTextureAnimationLookup = Elements[EElement_TextureAnimationLookup].Count;
	Header.Elements.oTextureAnimationLookup = Elements[EElement_TextureAnimationLookup].Offset;

	Header.Elements.nBoundingTriangle = Elements[EElement_BoundingTriangle].Count;
	Header.Elements.oBoundingTriangle = Elements[EElement_BoundingTriangle].Offset;

	Header.Elements.nBoundingVertex = Elements[EElement_BoundingVertex].Count;
	Header.Elements.oBoundingVertex = Elements[EElement_BoundingVertex].Offset;

	Header.Elements.nBoundingNormal = Elements[EElement_BoundingNormal].Count;
	Header.Elements.oBoundingNormal = Elements[EElement_BoundingNormal].Offset;

	Header.Elements.nAttachment = Elements[EElement_Attachment].Count;
	Header.Elements.oAttachment = Elements[EElement_Attachment].Offset;

	Header.Elements.nAttachmentLookup = Elements[EElement_AttachmentLookup].Count;
	Header.Elements.oAttachmentLookup = Elements[EElement_AttachmentLookup].Offset;

	Header.Elements.nEvent = Elements[EElement_Event].Count;
	Header.Elements.oEvent = Elements[EElement_Event].Offset;

	Header.Elements.nLight = Elements[EElement_Light].Count;
	Header.Elements.oLight = Elements[EElement_Light].Offset;

	Header.Elements.nCamera = Elements[EElement_Camera].Count;
	Header.Elements.oCamera = Elements[EElement_Camera].Offset;

	Header.Elements.nCameraLookup = Elements[EElement_CameraLookup].Count;
	Header.Elements.oCameraLookup = Elements[EElement_CameraLookup].Offset;

	Header.Elements.nRibbonEmitter = Elements[EElement_RibbonEmitter].Count;
	Header.Elements.oRibbonEmitter = Elements[EElement_RibbonEmitter].Offset;

	Header.Elements.nParticleEmitter = Elements[EElement_ParticleEmitter].Count;
	Header.Elements.oParticleEmitter = Elements[EElement_ParticleEmitter].Offset;

	Header.Elements.nUnknown1 = Elements[EElement_Unknown1].Count;
	Header.Elements.oUnknown1 = Elements[EElement_Unknown1].Offset;
}

SInt32  M2Lib::M2::m_GetChunkIndex(const Char8* ChunkID) const
{
	for (int a = 0; a < EChunk__Count__; a++)
	{
		bool Equal = true;
		for (int b = 0; b < 4; b++)
		{
			Equal = Equal && (kChunkIDs[a][b] == ChunkID[b]);
		}
		if (Equal)
			return a;
	}

	return -1;
}
