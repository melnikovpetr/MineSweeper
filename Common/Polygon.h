#ifndef POLYGON_H
#define POLYGON_H

#include "Value.h"

#include <vector>
#include <functional>

class Polygon
{
public:
	enum Metriks : size_t
	{
		MinPoint,
		MaxPoint,
		IsConvex,
		_Count
	};

public:
	static constexpr std::array<Value::TypeIds, Metriks::_Count>
		metrikTypeIds{Value::Point2D, Value::Point2D, Value::Size2D};

public:
	typedef std::vector<Point2D>::iterator VertexIt;
	typedef std::vector<Point2D>::const_iterator VertexCIt;

	typedef std::function<bool(VertexIt, VertexIt, VertexIt)>  VertexHandler;
	typedef std::function<bool(VertexCIt, VertexCIt, VertexCIt)> CVertexHandler;

public:
	Polygon(std::initializer_list<Point2D> vertexes);

	bool traverse(const  VertexHandler& handler);
	bool traverse(const CVertexHandler& handler) const;

	const Point2D& minPoint() const;
	const Point2D& maxPoint() const;
	bool isConvex() const;

	Polygon& calculateMetriks();

	bool hasPoint(const Point2D& point) const;

	Polygon& apply(const Transform2D& transform);
	Polygon applyToCopy(const Transform2D& transform) const;

	size_t vertexCount() const { return _vertexes.size(); }
	const Point2D& vertex(size_t index) const { return _vertexes[index]; }
	Point2D& vertex(size_t index) { return _vertexes[index]; }

protected:
	void resetMetriks(std::vector<Value>& metriks);

private:
	std::vector<Point2D> _vertexes;
	std::vector<Value> _metriks;
};

#endif // POLYGON_H
