#include "Polygon.h"

#include "FP.h"

#include <cmath>

#define INIT_METRIK(metriks,metrik_id) (metriks)[metrik_id] = Value::Type<metrikTypeIds[metrik_id]>{}
#define  GET_METRIK(metriks,metrik_id) (metriks)[metrik_id].castTo<metrikTypeIds[metrik_id]>()

Polygon::Polygon(std::initializer_list<Point2D> vertexes) : _vertexes(vertexes), _metriks(Metriks::_Count)
{
	resetMetriks(_metriks);
}

bool Polygon::traverse(const VertexHandler& handler)
{
	auto begin = _vertexes.begin();
	auto end = _vertexes.end();
	for (auto curr = begin; curr < end; ++curr)
		if (auto succeed = handler(begin, end, curr)) return succeed;
	return false;
}

bool Polygon::traverse(const CVertexHandler& handler) const
{
	auto begin = _vertexes.cbegin();
	auto end = _vertexes.cend();
	for (auto curr = begin; curr < end; ++curr)
		if (auto succeed = handler(begin, end, curr)) return succeed;
	return false;
}

const Point2D& Polygon::minPoint() const
{
	return GET_METRIK(_metriks, MinPoint);
}

const Point2D& Polygon::maxPoint() const
{
	return GET_METRIK(_metriks, MaxPoint);
}

bool Polygon::isConvex() const
{
	return vertexCount() == (GET_METRIK(_metriks, IsConvex)(C2D::X)
							 + GET_METRIK(_metriks, IsConvex)(C2D::Y));
}

Polygon& Polygon::calculateMetriks()
{
	auto metriks = _metriks;
	resetMetriks(metriks);

	traverse([&metriks](VertexCIt begin, VertexCIt end, VertexCIt vertex) {
		auto returnVal = false;
		auto prevVertex = vertex;
		auto nextVertex = vertex + 1;

		if (vertex == begin)
		{
			GET_METRIK(metriks, MinPoint) = *vertex;
			GET_METRIK(metriks, MaxPoint) = *vertex;
			prevVertex = end - 1;
		}
		else
		{
			--prevVertex;
			if (nextVertex == end) { nextVertex = begin; returnVal = true; }

			for (size_t coord = 0; coord < vertex->dim(); ++coord)
			{
				if ((*vertex)(coord) < GET_METRIK(metriks, MinPoint)(coord))
					GET_METRIK(metriks, MinPoint)(coord) = (*vertex)(coord);
				if ((*vertex)(coord) > GET_METRIK(metriks, MaxPoint)(coord))
					GET_METRIK(metriks, MaxPoint)(coord) = (*vertex)(coord);
			}
		}

		auto prevEdge = *prevVertex - *vertex;
		auto nextEdge = *nextVertex - *vertex;
		auto crossProdZ = prevEdge(C2D::X) * nextEdge(C2D::Y) - prevEdge(C2D::Y) * nextEdge(C2D::X);

		if (FP::equal(crossProdZ, decltype(crossProdZ){ 0 }))
			GET_METRIK(metriks, Metriks::IsConvex)(C2D::X)++; else
		if (crossProdZ > 0)
			GET_METRIK(metriks, Metriks::IsConvex)(C2D::Y)++;
		else
			GET_METRIK(metriks, Metriks::IsConvex)(C2D::Y)--;

		return returnVal;
	});

	_metriks = std::move(metriks);
	return *this;
}

bool Polygon::hasPoint(const Point2D& point) const
{
	auto result = false;

	if (isConvex())
	{
		auto copy = applyToCopy(Transform2D::move(-point));
		double angle{ 0 };
		copy.traverse([&angle](VertexCIt begin, VertexCIt end, VertexCIt vertex) {
			auto returnVal = false;
			auto nextVertex = vertex + 1;

			if (nextVertex == end) { nextVertex = begin; returnVal = true; }

			angle += std::acos(vertex->dotProd(*nextVertex) / (vertex->magnitude() * nextVertex->magnitude()));

			return returnVal;
		});

		if ((std::abs(2.0*M_PI - angle) / 2.0*M_PI) < 0.001 /*picked up 0.1%*/) result = true;
	}

	return result;
}

Polygon& Polygon::apply(const Transform2D& transform)
{
	traverse([&transform](VertexCIt, VertexCIt end, VertexIt vertex) {
		auto returnVal = false;
		if (vertex + 1 == end) { returnVal = true; }
		vertex->apply(transform);
		return returnVal;
	});
	GET_METRIK(_metriks, MinPoint).apply(transform);
	GET_METRIK(_metriks, MaxPoint).apply(transform);
	return *this;
}

Polygon Polygon::applyToCopy(const Transform2D& transform) const
{
	auto copy = *this;
	copy.apply(transform);
	return copy;
}

void Polygon::resetMetriks(std::vector<Value>& metriks)
{
	INIT_METRIK(metriks, Metriks::MinPoint);
	INIT_METRIK(metriks, Metriks::MaxPoint);
	INIT_METRIK(metriks, Metriks::IsConvex);
}
