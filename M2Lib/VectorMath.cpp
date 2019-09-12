#include "VectorMath.h"
#include <algorithm>
#include <cmath>
#include <array>

M2Lib::Geometry::Edge::Edge(uint16_t VertexA, uint16_t VertexB)
{
	if (VertexA < VertexB)
	{
		A = std::make_shared<Vertex>(VertexA);
		B = std::make_shared<Vertex>(VertexB);
	}
	else
	{
		A = std::make_shared<Vertex>(VertexB);
		B = std::make_shared<Vertex>(VertexA);
	}

	A->pEdge = this;
	B->pEdge = this;
}

M2Lib::Geometry::Triangle::Triangle(uint16_t VertexA, uint16_t VertexB, uint16_t VertexC)
{
	A = VertexA;
	B = VertexB;
	C = VertexC;

	std::array<uint16_t, 3> list = { VertexA, VertexB, VertexC };
	std::sort(list.begin(), list.end());

	EdgeA = std::make_shared<Edge>(list[0], list[1]);
	EdgeB = std::make_shared<Edge>(list[0], list[2]);
	EdgeC = std::make_shared<Edge>(list[1], list[2]);

	EdgeA->pTriangle = this;
	EdgeB->pTriangle = this;
	EdgeC->pTriangle = this;
}

bool M2Lib::Geometry::Triangle::IsOnEdge() const
{
	return EdgeA->IsUnique || EdgeB->IsUnique || EdgeC->IsUnique;
}

M2Lib::Geometry::Plane::Plane(C3Vector const& A, C3Vector const& B, C3Vector const& C)
{
	this->A = A;
	this->B = B;
	this->C = C;

	Normal = C3Vector::CalculateNormal(A, B, C, false);

	// a * x + b * y + c * z + d = 0
	this->a = Normal.X;
	this->b = Normal.Y;
	this->c = Normal.Z;
	this->d = -A.X * Normal.X - A.Y * Normal.Y - A.Z * Normal.Z;
}

float M2Lib::Geometry::CalculateAngle(C3Vector const& A, C3Vector const& B)
{
	return std::acosf(A.Dot(B) / A.Length() / B.Length());
}

float M2Lib::Geometry::CalculateAngle(Plane const& A, Plane const& B)
{
	return CalculateAngle(A.Normal, B.Normal);
}
