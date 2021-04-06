#ifndef TESS_HEXAGON_H
#define TESS_HEXAGON_H

#include "Tessellation.h"

class TessHexagon :	public Tessellation
{
public:
	enum IndexElems : size_t { RadiusIndex, AlfaIndex };

public:
	static size_t count(size_t radiusCount);
	static size_t countOnRadius(size_t radius);
	
	static const Point2D& prototypeSize();
	static const Polygon& prototype();

public:
	TessHexagon(const Value& size);

	size_t radiusCount() const;
	Value makeIndex(size_t r, size_t a) const;
	size_t nextAlfa(const Value& index) const;
	size_t prevAlfa(const Value& index) const;

	Value beginIndex() const override;
	Value endIndex() const override;
	Value nextIndex(const Value& index) const override;
	Value prevIndex(const Value& index) const override;
	std::vector<Value> neighbors(const Value& index) const override;
	Value index(size_t crossCuttingIndex) const override;
	Value index(const Point2D& point) const override;

	size_t crossCuttingIndex(const Value& index) const override;
	size_t count() const override;

	Point2D point(const Value& index) const override;

	Polygon polygon(const Value& index) const override;
	Point2D polygonSize() const override;
};

#endif // TESS_HEXAGON_H
