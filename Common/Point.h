#ifndef POINT_H
#define POINT_H

#include <array>
#include <cassert>
#include <cmath>

typedef float Coord;

template<typename Coords>
class Transform;

template<typename TCoords>
class Size
{
public:
	typedef TCoords Coords;
	typedef std::array<size_t, (size_t)Coords::Dim> ValueArray;

public:
	Size() : _values{} {}
	Size(const ValueArray& values) : _values{values} {}

	constexpr size_t dim() const { return (size_t)Coords::Dim; }

	bool operator == (const Size& other) const { return _values == other._values; }

	const size_t& operator ()(size_t coord) const { assert(coord < dim()); return _values[coord]; }
	size_t& operator ()(size_t coord) { assert(coord < dim()); return _values[coord]; }

	const size_t& operator ()(Coords coord) const { return (*this)((size_t)coord); }
	size_t& operator ()(Coords coord) { return (*this)((size_t)coord); }

private:
	ValueArray _values;
};

template<typename TCoords>
class Point
{
public:
	typedef TCoords Coords;
	typedef std::array<Coord, (size_t)Coords::Dim> ValueArray;

public:
	Point() : _values{} {}
	Point(const ValueArray& values) : _values{values} {}

	constexpr size_t dim() const { return (size_t)Coords::Dim; }

	Coord dotProd(const Point& other) const
	{
		Coord result = 0;
		for (size_t i = 0; i < dim(); ++i) result += (*this)(i) * other(i);
		return result;
	}

	Coord magnitude() const
	{
		Coord result = 0;
		for (size_t i = 0; i < dim(); ++i) result += (*this)(i) * (*this)(i);
		return sqrt(result);
	}

	Point& operator =(const ValueArray& values) { _values = values; return *this; }

	bool operator ==(const Point& other) const { return _values == other._values; }

	Point operator +(const Point& other) const
	{
		auto result = *this;
		for (size_t i = 0; i < dim() ; ++i) result(i) += other(i);
		return result;
	}
	Point operator -(const Point& other) const
	{
		auto result = *this;
		for (size_t i = 0; i < dim() ; ++i) result(i) -= other(i);
		return result;
	}
	Point operator -() const
	{
		auto result = *this;
		for (size_t i = 0; i < dim() ; ++i) result(i) = -result(i);
		return result;
	}

	const Coord& operator()(size_t coord) const { assert(coord < dim()); return _values[coord]; }
	Coord& operator()(size_t coord) { assert(coord < dim()); return _values[coord]; }

	const Coord& operator()(Coords coord) const { return (*this)((size_t)coord); }
	Coord& operator()(Coords coord) { return (*this)((size_t)coord); }

	Point& apply(const Transform<Coords>& trasform) { *this = trasform * (*this); return *this; }
	Point applyToCopy(const Transform<Coords>& trasform) const { return trasform * (*this); }

private:
	ValueArray _values;
};

template<typename TCoords>
Point<TCoords> operator *(Coord factor, Point<TCoords> point)
{
	for (size_t i = 0; i < (size_t)TCoords::Dim; ++i) point(i) *= factor;
	return point;
}

template<typename TCoords>
class Transform
{
public:
	typedef TCoords Coords;
	typedef std::array < Coord, ((size_t)Coords::Dim + 1)*((size_t)Coords::Dim + 1) > ValueArray;

public:
	static constexpr size_t dim() { return (size_t)Coords::Dim; }
	static constexpr size_t dimPlus1() { return dim() + 1; }

	static Transform move(const Point<Coords>& point)
	{
		Transform result{};
		for (size_t i = 0; i <  dim(); ++i) { result(i, point.dim()) = point(i); }
		return result;
	}
	static Transform resize(const Point<Coords>& point)
	{
		Transform result{};
		for (size_t i = 0; i < dim(); ++i) { result(i, i) = point(i); }
		return result;
	}
	static Transform rotate(Coord angle, TCoords axis = TCoords::Dim);

public:
	Transform() : _values{} { for (size_t i = 0; i < dimPlus1(); ++i) { (*this)(i, i) = 1; } }
	Transform(const ValueArray& values) : _values{values} {}

	Transform operator* (const Transform& other) const
	{
		auto& a = *this;
		auto& b = other;
		Transform c{{0}};
		for (size_t ra = 0; ra < dimPlus1(); ++ra)
			for (size_t cb = 0; cb < dimPlus1(); ++cb)
				for (size_t rb = 0; rb < dimPlus1(); ++rb)
					c(ra, cb) += a(ra, rb) * b(rb, cb);
		return c;
	}

	Point<Coords> operator* (const Point<Coords>& point) const
	{
		Point<Coords> result{};
		Coord lambda = 0;
		auto& transform = *this;

		for (size_t j = 0; j < dim(); ++j)
		{
			lambda += transform(dim(), j) * point(j);
		}
		lambda += transform(dim(), dim());

		for (size_t i = 0; i < (dim()); ++i)
		{
			for (size_t j = 0; j < dim(); ++j)
			{
				result(i) += transform(i, j) * point(j);
			}
			result(i) += transform(i, dim());
			result(i) /= lambda;
		}
		return result;
	}

	const Coord& operator()(size_t coord1, size_t coord2) const
	{
		auto index = coord1 * dimPlus1() + coord2;
		assert(index < dimPlus1() * dimPlus1());
		return _values[index];
	}

	Coord& operator()(size_t coord1, size_t coord2)
	{
		auto index = coord1 * dimPlus1() + coord2;
		assert(index < dimPlus1() * dimPlus1());
		return _values[index];
	}

	const Coord& operator()(Coords coord1, Coords coord2) const
	{ return (*this)((size_t)coord1, (size_t)coord2); }

	Coord& operator()(Coords coord1, Coords coord2)
	{ return (*this)((size_t)coord1, (size_t)coord2); }

private:
	ValueArray _values;
};

enum class C2D : size_t
{
	X, Y, Dim
};

enum class C3D : size_t
{
	X, Y, Z, Dim
};

typedef Size<C2D> Size2D;
typedef Point<C2D> Point2D;
typedef Transform<C2D> Transform2D;

typedef Size<C3D> Size3D;
typedef Point<C3D> Point3D;
typedef Transform<C3D> Transform3D;

#endif // POINT_H
