#ifndef FIELD_H
#define FIELD_H

#include "Cell.h"

class Field
{
public:
	typedef std::vector<Cell>::iterator CellIt;
	typedef std::vector<Cell>::const_iterator CellCIt;

	typedef std::function< bool(CellIt, CellIt, CellIt)> CellHandler;
	typedef std::function< bool(CellCIt, CellCIt, CellCIt)> CCellHandler;

public:
	Field(const Settings::ParamValues& paramValues, std::unique_ptr<class Tessellation>&& tess);
	~Field();

	size_t cellIndex(const Point2D& point) const;
	size_t cellCount() const { return _cells.size(); }
	size_t mineCount() const { return _mineCount; }
	size_t mineMarkCount() const { return _mineMarkCount; }
	size_t foundMineCount() const { return _foundMineCount; }
	size_t openedCellCount() const { return _openedCellCount; }
	size_t denotMineCount() const { return _detonMineCount; }
	std::vector<size_t> neighbors(size_t index) const;
	Point2D cellSize() const;
	const Cell& cell(size_t index) const { return _cells[index]; }

	size_t selectCell(size_t index, size_t& oldIndex);
	size_t selectCell(const Point2D& point, size_t& oldIndex);
	void moveMine(size_t fromIndex, size_t toIndex);

	size_t selectedIndex() const { return _selectedIndex; }
	const Cell& cell() const { return cell(_selectedIndex); }
	std::vector<size_t> neighbors() const { return neighbors(_selectedIndex); }
	std::vector<size_t> openCell(bool touchMine = true);
	std::vector<size_t> openMines();
	const CellMark* markCell();
	void drawCell(const Settings::ParamValues& paramValues, struct IPainter* painter) const;

	bool traverse(const CellHandler& handler);
	bool traverse(const CCellHandler& handler) const;
	
	const Rect& boundingRect() const { return _boundingRect; }
	void draw(const Settings::ParamValues& paramValues, struct IPainter* painter) const;
	void drawCells(const std::vector<size_t>& indexes,
			  const Settings::ParamValues& paramValues,
			  struct IPainter* painter) const;

protected:
	Cell& cell(size_t index) { return _cells[index]; }
	Cell& cell() { return cell(_selectedIndex); }

private:
	std::unique_ptr<class Tessellation> _tess;
	std::vector<Cell> _cells;
	Rect _boundingRect;
	
	size_t _mineCount;
	size_t _selectedIndex;
	size_t _mineMarkCount;
	size_t _foundMineCount;
	size_t _openedCellCount;
	size_t _detonMineCount;
};

#endif // FIELD_H

