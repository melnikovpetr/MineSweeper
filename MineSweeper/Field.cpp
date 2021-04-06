#include "Field.h"

#include "Tessellation.h"
#include "MineSweeper.h"
#include "../Common/FP.h"

#include <ctime>

Field::Field(const Settings::ParamValues& paramValues, std::unique_ptr<Tessellation>&& tess)
	: _tess((assert(tess && tess->count() && (tess->count() < RAND_MAX)), std::move(tess))),
		_selectedIndex{_tess->count()}, _mineMarkCount{0}, _foundMineCount{0}, _openedCellCount{0}, _detonMineCount{0}
{
	auto minePercent = paramValues.at(IGame::MinePercent).castTo<Value::Int>();
	assert((minePercent > 0) && (minePercent <= 100));

	auto mineDetonProb = paramValues.at(IGame::MineDetonProb).castTo<Value::Double>();
	assert((mineDetonProb > 0.0f) && (FP::equal(mineDetonProb, 1.0) || (mineDetonProb < 1.0)));

	std::vector<bool> indexesWithMine(_tess->count(), false);

	_mineCount = size_t(std::round((double(minePercent) / 100.0) * double(_tess->count())));

	std::srand(unsigned(std::time(nullptr)));
	for (size_t mineIndex = 0, indexWithMine; mineIndex < _mineCount; ++mineIndex)
	{
		while (indexesWithMine[indexWithMine = std::rand() % _tess->count()]);
		indexesWithMine[indexWithMine] = true;
	}

	_cells.reserve(_tess->count());

	for (auto& it : *_tess)
	{
		_cells.push_back({ it,
			indexesWithMine[_cells.size()]
			? std::make_unique<CellMine>(mineDetonProb)	: std::unique_ptr<CellMine>() });
	}

	for (size_t index = 0; index < cellCount(); ++index)
	{
		size_t neighborsMineCount{0};
		auto cellRect = cell(index).boundingRect();

		for (const auto& neighborIndex : neighbors(index))
			neighborsMineCount += cell(neighborIndex).hasMine();
		cell(index).setNeighborsMineCount(neighborsMineCount);

		if (!index) _boundingRect = cellRect;
		else
		{
			if (_boundingRect.left() > cellRect.left()) _boundingRect.setLeft(cellRect.left());
			if (_boundingRect.bottom() > cellRect.bottom()) _boundingRect.setBottom(cellRect.bottom());

			if (_boundingRect.right() < cellRect.right()) _boundingRect.setRight(cellRect.right());
			if (_boundingRect.top() < cellRect.top()) _boundingRect.setTop(cellRect.top());
		}
	}
}

Field::~Field() {}

size_t Field::cellIndex(const Point2D& point) const
{
	auto polygonIndex = _tess->index(point);
	return polygonIndex.empty() ? cellCount() : _tess->crossCuttingIndex(polygonIndex);
}

std::vector<size_t> Field::neighbors(size_t index) const
{
	assert(index < cellCount());
	auto polygonNeighbors = _tess->neighbors(_tess->index(index));
	std::vector<size_t> neighbors_(polygonNeighbors.size());
	for (size_t i = 0; i < polygonNeighbors.size(); ++i)
		neighbors_[i] = _tess->crossCuttingIndex(polygonNeighbors[i]);
	return neighbors_;
}

Point2D Field::cellSize() const
{
	return _tess->polygonSize();
}

size_t Field::selectCell(size_t index, size_t& oldIndex)
{
	if (index != _selectedIndex)
	{
		if(_selectedIndex < cellCount())
		{
			cell().setSelected(false);
			oldIndex = _selectedIndex;
		}
		_selectedIndex = index;
		if (_selectedIndex < cellCount()) { cell().setSelected(true); }
		return _selectedIndex;
	}
	else
	{
		oldIndex = cellCount();
		return oldIndex;
	}
}

size_t Field::selectCell(const Point2D& point, size_t& oldIndex)
{
	return selectCell(cellIndex(point), oldIndex);
}

std::vector<size_t> Field::openCell(bool touchMine)
{
	std::vector<size_t> openCellsIndexes{};
	if (cell().open(touchMine))
	{
		if (cell().isDetonated()) _detonMineCount++;
		openCellsIndexes.push_back(_selectedIndex);
		if (!cell().hasMine() && !cell().neighborsMineCount())
		{
			size_t indexOfIndex{0};
			while (indexOfIndex < openCellsIndexes.size())
			{
				if (!cell(openCellsIndexes[indexOfIndex]).neighborsMineCount())
				{
					for (auto neighborIndex : neighbors(openCellsIndexes[indexOfIndex]))
					{
						if (cell(neighborIndex).open()) { openCellsIndexes.push_back(neighborIndex); }
					}
				}
				indexOfIndex++;
			}
		}
	}
	_openedCellCount += openCellsIndexes.size();
	return openCellsIndexes;
}

std::vector<size_t> Field::openMines()
{
	std::vector<size_t> cells{};
	size_t mineCount = foundMineCount() + denotMineCount();
	if (mineCount < _mineCount)
	{
		cells.reserve(_mineCount - mineCount);
		traverse([&cells, &mineCount, this](Field::CellIt begin, Field::CellIt, Field::CellIt cellIt) {
			if (!cellIt->isOpen() && cellIt->hasMine() && !cellIt->hasMark(CellMark::Mine))
			{
				cellIt->open(false /*don't touch mine*/);
				cells.push_back(cellIt - begin);
				mineCount++;
			}
			return mineCount == _mineCount;
		});
	}
	return cells;
}

const CellMark* Field::markCell()
{
	auto hadMineMark = cell().hasMark(CellMark::Mine);
	if (cell().nextMark() && hadMineMark) { --_mineMarkCount; if (cell().hasMine()) { --_foundMineCount; } }

	if (cell().hasMark(CellMark::Mine)) { ++_mineMarkCount; if (cell().hasMine()) { ++_foundMineCount; } }

	return cell().mark();
}

void Field::drawCell(const Settings::ParamValues & paramValues, IPainter * painter) const
{
	cell().draw(paramValues, painter);
}

void Field::moveMine(size_t fromIndex, size_t toIndex)
{
	assert((fromIndex < cellCount()) && (toIndex < cellCount()));
	assert(cell(fromIndex).hasMine() && !cell(fromIndex).isDetonated() && !cell(toIndex).hasMine());

	cell(toIndex).setMine(cell(fromIndex).takeMine());

	for (const auto& neighborIndex : neighbors(fromIndex))
		cell(neighborIndex).setNeighborsMineCount(cell(neighborIndex).neighborsMineCount() - 1);

	for (const auto& neighborIndex : neighbors(toIndex))
		cell(neighborIndex).setNeighborsMineCount(cell(neighborIndex).neighborsMineCount() + 1);
}

bool Field::traverse(const Field::CellHandler& handler)
{
	auto begin = _cells.begin();
	auto end = _cells.end();
	for (auto curr = begin; curr < end; ++curr)
		if (auto succeed = handler(begin, end, curr)) return succeed;
	return false;
}

bool Field::traverse(const Field::CCellHandler& handler) const
{
	auto begin = _cells.cbegin();
	auto end = _cells.cend();
	for (auto curr = begin; curr < end; ++curr)
		if (auto succeed = handler(begin, end, curr)) return succeed;
	return false;
}

void Field::draw(const Settings::ParamValues& paramValues, IPainter* painter) const
{
	traverse([&paramValues, painter](CellCIt,CellCIt end,CellCIt cell) {
		cell->draw(paramValues, painter); return (cell + 1) == end; });
}

void Field::drawCells(
	const std::vector<size_t>& indexes,
	const Settings::ParamValues& paramValues,
	IPainter* painter) const
{
	for (auto index : indexes) _cells[index].draw(paramValues, painter);
}
