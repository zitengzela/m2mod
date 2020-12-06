#include "M2SkinElement.h"

uint32_t M2Lib::GetGeosetType(uint32_t GeosetId)
{
	switch (GeosetId / 100)
	{
		case 0:
		{
			if (GeosetId > 0)
				return GeosetType_Hair;

			return GeosetType_Body;
		}
		case 1:
		case 2:
		case 3:
		case 16:	// dwarf nose earrings, mechagnome chin
		case 34:
			return GeosetType_Facial;
		case 4:
		case 5:
		case 8:
		case 9:
		case 10:
		case 11:
		case 13:
		case 18:
		case 20:
		case 22:
		case 23:
		{
			if (GeosetId % 10 != 1)
				return GeosetType_Armor;

			return GeosetType_Body;
		}
		case 6:
		case 7:
		case 19:
		case 38:
		case 40:
		case 41:
			return GeosetType_Body;
		case 12:
		case 14:
		case 15:
			return GeosetType_Cloak;
		case 17:
		case 33:
			return GeosetType_Eyes;
		case 24:
		case 25:
		case 29:
		case 30:
		case 31:
		case 35:
		case 36:
		case 37:
		case 39:
			return GeosetType_Accessories;
		case 32:
			return GeosetType_Body;
		default:
			break;
	}

	return GeosetType_Unknown;
}

bool M2Lib::IsAlignedGeoset(uint32_t GeosetId)
{
	uint32_t GeosetType = GetGeosetType(GeosetId);
	if (GeosetType == GeosetType_Body)
		return true;

	if (GeosetType != GeosetType_Armor)
		return false;

	switch (GeosetId / 100)
	{
		case 4:
		case 8:
		case 13:
		case 22:
			return true;
		default:
			break;

	}

	return false;
}

bool M2Lib::IsFaceGeoset(uint32_t GeosetId)
{
	return (GeosetId / 100) == 32;
}
