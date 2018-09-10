#include "M2SkinElement.h"

UInt32 M2Lib::GetSubSetType(UInt32 SubsetId)
{
	switch (SubsetId / 100)
	{
		case 1:
		case 2:
		case 3:
		case 17:
			return Subset_Facial;
	}

	switch (SubsetId / 100)
	{
		case 0:
			if (SubsetId > 0)
				return Subset_Hair;
			break;
		default:
			break;
	}

	switch (SubsetId / 100)
	{
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
		{
			if (SubsetId % 10 != 1)
				return Subset_Armor;
			break;
		}
		default:
			break;
	}

	switch (SubsetId / 100)
	{
		case 12:
		case 15:
			return Subset_Cloak;
		default:
			break;
	}

	switch (SubsetId)
	{
		case 0:
		case 401:
		case 501:
		case 1301:
		case 2001:
		case 2201:
		case 2301:
			return Subset_Body;
		default:
			break;
	}

	switch (SubsetId / 100)
	{
		case 7:
		case 19:
			return Subset_Body;
		default:
			break;
	}

	return Subset_Unknown;
}

bool M2Lib::IsAlignedSubset(UInt32 SubsetId)
{
	UInt32 SubsetType = GetSubSetType(SubsetId);
	if (SubsetType == Subset_Body)
		return true;

	if (SubsetType != Subset_Armor)
		return false;

	switch (SubsetId / 100)
	{
		case 8:
			break;

	}
	// 0803
	// 0402
	// 1104
	// 1302
	// 2202

	return false;
}
