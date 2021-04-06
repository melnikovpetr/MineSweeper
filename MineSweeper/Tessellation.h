#ifndef TESSELLATION_H
#define TESSELLATION_H

#include "../Common/Value.h"
#include "../Common/Polygon.h"

class Tessellation
{
public:
	class Iterator
	{
	public:
		using difference_type = void;
		using value_type = Polygon;
		using pointer = const Polygon*;
		using reference = const Polygon&;
		using iterator_category = std::forward_iterator_tag;

	public:
		Iterator(const Tessellation& tess, const Value& index) : _tess(tess), _index(index), _polygon{ _tess.polygon(_index) } {}
		Iterator& operator++() { _index = _tess.nextIndex(_index); _polygon = _tess.polygon(_index); return *this; }
		Iterator& operator--() { _index = _tess.prevIndex(_index); _polygon = _tess.polygon(_index); return *this; }
		bool operator==(const Iterator& other) const { return _index == other._index; }
		bool operator!=(const Iterator& other) const { return !(*this == other); }
		reference operator*() { return _polygon; }
	
	private:
		const Tessellation& _tess;
		Value _index;
		Polygon _polygon;
	};

public:
	Tessellation(const Value& size) : _size(size) { }
	Iterator begin() { return Iterator(*this, beginIndex()); }
	Iterator end() { return Iterator(*this, endIndex()); }

	const Value& size() const { return _size; }

	virtual Value beginIndex() const = 0;
	virtual Value endIndex() const = 0;
	virtual Value nextIndex(const Value& index) const = 0;
	virtual Value prevIndex(const Value& index) const = 0;
	virtual std::vector<Value> neighbors(const Value& index) const = 0;
	virtual Value index(size_t crossCuttingIndex) const = 0;
	virtual Value index(const Point2D& point) const = 0;

	virtual size_t crossCuttingIndex(const Value& index) const = 0;
	virtual size_t count() const = 0;

	virtual Point2D point(const Value& index) const = 0;

	virtual Polygon polygon(const Value& index) const = 0;
	virtual Point2D polygonSize() const = 0;

private:
	Value _size;
};

#endif // TESSELLATION_H

