#include "TessSquare.h"

#include "MineSweeper.h"

#define PARSE_INDEX(index,r,c) \
	auto& r = index[RowIndex].castTo<Value::typeIds<size_t>>(); \
	auto& c = index[ColumnIndex].castTo<Value::typeIds<size_t>>(); \
	assert((r < size()) && (a < r * prototype().vertexCount()))

size_t TessSquare::count(size_t rowCount, size_t columnCount)
{
	return rowCount * columnCount;
}

const Point2D& TessSquare::prototypeSize()
{
	static const Point2D protosize_{};
	return protosize_;
}

const Polygon& TessSquare::prototype()
{
	static const Polygon prototype_{ {}, {}, {}, {} };
	return prototype_;
}

TessSquare::TessSquare(const Value& size) : Tessellation(size) {}

size_t TessSquare::rowCount() const
{
	return size()[EnumValue{ IGame::RowCount }].castTo<Value::typeIds<size_t>>();
}

size_t TessSquare::columnCount() const
{
	return size()[EnumValue{ IGame::ColumnCount }].castTo<Value::typeIds<size_t>>();
}

Value TessSquare::makeIndex(size_t row, size_t column) const
{
	return Value{ row, column };
}

Value TessSquare::beginIndex() const
{
	return Value{};
}

Value TessSquare::endIndex() const
{
	return Value{};
}

Value TessSquare::nextIndex(const Value& index) const
{
	return Value{};
}

Value TessSquare::prevIndex(const Value& index) const
{
	return Value{};
}

std::vector<Value> TessSquare::neighbors(const Value& index) const
{
	return std::vector<Value>{};
}


Value TessSquare::index(size_t crossCuttingIndex) const
{
	return Value{};
}

Value TessSquare::index(const Point2D& point) const
{
	return Value{};
}

size_t TessSquare::crossCuttingIndex(const Value& index) const
{
	return size_t{};
}

size_t TessSquare::count() const
{
	return count(rowCount(), columnCount());
}

Point2D TessSquare::point(const Value& index) const
{
	return Point2D{};
}

Polygon TessSquare::polygon(const Value& index) const
{
	return Polygon{};
}

Point2D TessSquare::polygonSize() const
{
	return prototypeSize();
}
