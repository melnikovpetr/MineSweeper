#include "Point.h"

#include <cmath>

template<>
Transform2D Transform<C2D>::rotate(Coord angle, C2D)
{
	Transform2D result{};
	result(C2D::X, C2D::X) =  std::cos(angle);
	result(C2D::X, C2D::Y) =  std::sin(angle);
	result(C2D::Y, C2D::X) = -result(C2D::X, C2D::Y);
	result(C2D::Y, C2D::Y) =  result(C2D::X, C2D::X);
	return result;
}
