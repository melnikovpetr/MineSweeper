#ifndef TESS_SQUARE_H
#define TESS_SQUARE_H

#include "Tessellation.h"

class TessSquare : public Tessellation
{
public:
	enum IndexElems : size_t { RowIndex, ColumnIndex };

public:
	static size_t count(size_t rowCount, size_t columnCount);
	
	static const Point2D& prototypeSize();
	static const Polygon& prototype();

public:
	TessSquare(const Value& size);
	
	size_t rowCount() const;
	size_t columnCount() const;
	Value makeIndex(size_t row, size_t column) const;

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

#endif // TESS_SQUARE_H
