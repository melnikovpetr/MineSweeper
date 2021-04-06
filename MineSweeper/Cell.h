#ifndef CELL_H
#define CELL_H

#include "../Common/Polygon.h"
#include "../Common/Rect.h"
#include "../Common/Settings.h"

#include <memory>

class CellMark
{
public:
	enum Types : size_t
	{
		Mine,
		QUESTION,
		_COUNT
	};

	CellMark(Types type) : _type(type) {}

	Types type() const { return _type; }
	void draw(const Settings::ParamValues& paramValues, struct IPainter* painter) const;

private:
	Types _type;
};

class CellMine
{
public:
	class Probability
	{
	public:
		Probability(double prob);
		bool trigger() const;

	private:
		ptrdiff_t _num;
		ptrdiff_t _denom;
	};

public:
	CellMine(double prob) : _isDetonate(false), _prob(prob) {}

	bool isDetonated() const { return _isDetonate; }
	const Probability& probability() const { return _prob; }
	void detonate() { if (!_isDetonate) _isDetonate = _prob.trigger(); }
	void draw(const Settings::ParamValues& paramValues, struct IPainter* painter) const;

private:
	bool _isDetonate;
	Probability _prob;
};

class Cell
{
public:
	Cell(const Polygon& polygon, std::unique_ptr<CellMine>&& mine = {});

	bool isOpen() const { return _isOpen; }
	bool open(bool touchMine = true);

	void setSelected(bool select) { _isSelected = select; }
	bool isSelected() const { return _isSelected; }
	
	bool hasMark() const { return (bool)_mark; }
	bool hasMark(CellMark::Types type) const { return _mark && (_mark->type() == type); }
	bool nextMark();
	const CellMark* mark() const { return _mark.get(); }

	bool hasMine() const { return (bool)_mine; }
	void setMine(std::unique_ptr<CellMine>&& mine) { _mine = std::move(mine); }
	const CellMine* mine() const { return _mine.get(); }
	std::unique_ptr<CellMine>&& takeMine() { return std::move(_mine); }
	bool isDetonated() const { if(_mine) return _mine->isDetonated(); else return false; }

	bool hasPoint(const Point2D& point) const { return _polygon.hasPoint(point); }

	void setNeighborsMineCount(size_t neighborsMineCount) { _neighborsMineCount = neighborsMineCount; }
	size_t neighborsMineCount() const { return _neighborsMineCount; }

	Rect boundingRect() const;

	void draw(const Settings::ParamValues& paramValues, struct IPainter* painter) const;

private:
	const Polygon _polygon;
	bool _isOpen;
	bool _isSelected;
	size_t _neighborsMineCount;
	std::unique_ptr<CellMine> _mine;
	std::unique_ptr<CellMark> _mark;
};

#endif // CELL_H

