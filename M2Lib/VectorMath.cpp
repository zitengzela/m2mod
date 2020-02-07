#include "VectorMath.h"
#include <cmath>

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
