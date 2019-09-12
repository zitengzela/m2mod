#pragma once

#include "M2Types.h"
#include <memory>

namespace M2Lib::Geometry
{
	class Edge;
	class Triangle;

	class Vertex
	{
	public:
		Vertex() = default;
		Vertex(uint16_t Index)
		{
			this->Index = Index;
		}

		uint16_t Index;
		Edge* pEdge;
	};

	class Edge
	{
	public:
		Edge() = default;

		Edge(uint16_t VertexA, uint16_t VertexB);

		bool operator==(const Edge& other) const
		{
			return GetHash() == other.GetHash();
		}

		uint32_t GetHash() const
		{
			return (uint32_t(A->Index) << 16) | uint32_t(B->Index);
		}

		std::shared_ptr<Vertex> A;
		std::shared_ptr<Vertex> B;
		bool IsUnique = false;
		Triangle* pTriangle;
	};

	class Triangle
	{
	public:
		Triangle() = default;
		Triangle(uint16_t VertexA, uint16_t VertexB, uint16_t VertexC);

		uint16_t A;
		uint16_t B;
		uint16_t C;

		std::shared_ptr<Edge> EdgeA;
		std::shared_ptr<Edge> EdgeB;
		std::shared_ptr<Edge> EdgeC;
		bool IsOnEdge() const;
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
