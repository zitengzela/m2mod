#pragma once

#include "M2Types.h"
#include <memory>

namespace M2Lib::Geometry
{
	class Edge
	{
	public:
		static uint32_t GetHash(uint16_t indexA, uint16_t indexB)
		{
			if (indexA > indexB)
				std::swap(indexA, indexB);

			return (uint32_t(indexA) << 16) | uint32_t(indexB);
		}
	};

	class Plane
	{
	public:
		C3Vector A;
		C3Vector B;
		C3Vector C;

		C3Vector Normal;

		float a;
		float b;
		float c;
		float d;

		Plane(C3Vector const& A, C3Vector const& B, C3Vector const& C);
	};

	float CalculateAngle(C3Vector const& A, C3Vector const& B);

	float CalculateAngle(Plane const& A, Plane const& B);
}
