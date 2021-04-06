#include "Cell.h"

#include "MineSweeper.h"

#include "../Common/FP.h"

#include <ctime>

CellMine::Probability::Probability(double prob)
{
	assert((prob > 0) && (prob <= 1));
	FP::rational(prob, RAND_MAX, _num, _denom);
}

bool CellMine::Probability::trigger() const
{
	std::srand(unsigned(std::time(nullptr))); // use current time as seed for random generator
	return (std::rand() % _denom) < _num;
}

Cell::Cell(const Polygon& polygon, std::unique_ptr<CellMine>&& mine)
	: _polygon(polygon), _isOpen(false), _isSelected(false), _neighborsMineCount(0), _mine(std::move(mine)), _mark()
{}

bool Cell::open(bool touchMine)
{
	if (!isOpen() && !hasMark())
	{
		_isOpen = true;
		if(touchMine && hasMine()) { _mine->detonate(); }
		return true;
	}
	return false;
}

bool Cell::nextMark()
{
	if (!isOpen())
	{
		_mark = _mark
			? (_mark->type() == (CellMark::_COUNT - 1)
				? std::unique_ptr<CellMark>{}
				: std::make_unique<CellMark>((CellMark::Types)(_mark->type() + 1)))
			: std::make_unique<CellMark>((CellMark::Types)0);
		return true;
	}
	return false;
}

Rect Cell::boundingRect() const
{
	return Rect{_polygon.minPoint(), _polygon.maxPoint()};
}

void CellMark::draw(const Settings::ParamValues& paramValues, IPainter* painter) const
{
	Point2D size = paramValues.at(IGame::CellMarkSize).castTo<Value::Point2D>();

	painter->save();
	painter->setPen(paramValues.at(IGame::CellMarkPen));
	painter->setBrush(paramValues.at(IGame::CellMarkBrush));

	if (_type == Mine)
	{
		Polygon polygon{
			{{ -size(C2D::X) / 2, size(C2D::Y) }}, {{ size(C2D::X) / 2, size(C2D::Y) }},
			{{ size(C2D::X) / 2, 0 }}, {{ -size(C2D::X) / 2, 0 }} };
		painter->drawPolygon(polygon);
		painter->fillPolygon(polygon);
		painter->drawLine({{ -size(C2D::X) / 2, 0 }}, {{ -size(C2D::X) / 2, -size(C2D::Y) }});
	} else
	if (_type == QUESTION)
	{
		painter->drawText("?", Rect({{ -size(C2D::X) / 2, -size(C2D::Y) / 2 }}, {{ size(C2D::X) / 2, size(C2D::Y) / 2 }}));
	}
	else
	{
		assert(false);
	}
	painter->restore();
}

void CellMine::draw(const Settings::ParamValues& paramValues, IPainter* painter) const
{
	Point2D size = paramValues.at(IGame::CellMineSize).castTo<Value::Point2D>();
	auto radius = std::min(size(C2D::X), size(C2D::Y));

	painter->save();

	painter->setBrush(paramValues.at(IGame::CellMineBrush));
	painter->setPen(paramValues.at(IGame::CellMinePen));

	painter->fillCircle({}, radius);
	painter->drawCircle({}, radius);
	
	painter->restore();
}

void Cell::draw(const Settings::ParamValues& paramValues, IPainter* painter) const
{
	Point2D size = paramValues.at(IGame::CellMarkSize).castTo<Value::Point2D>();

	painter->setPen(paramValues.at(IGame::GridPen));

	painter->setBrush(paramValues.at(isDetonated() ? IGame::DetonatedCellBrush : IGame::OpenedCellBrush));

	painter->fillPolygon(_polygon);

	if (hasMine() || _neighborsMineCount)
	{
		painter->save();
		painter->setTransform(Transform2D::move(boundingRect().center()), true /*combine*/);
		if (hasMine()) { _mine->draw(paramValues, painter); } else
		if (_neighborsMineCount)
		{
			painter->setPen(paramValues.at(IGame::CellTextPen));
			painter->drawText(std::to_string(_neighborsMineCount),
				Rect({ { -size(C2D::X) / 2, -size(C2D::Y) / 2 } }, { { size(C2D::X) / 2, size(C2D::Y) / 2 } }));
		}
		painter->restore();
	}

	if (!isOpen())
	{
		painter->setBrush(paramValues.at(IGame::ClosedCellBrush));
		painter->fillPolygon(_polygon);

		if (hasMark())
		{
			painter->save();
			painter->setTransform(Transform2D::move(boundingRect().center()), true /*combine*/);
			_mark->draw(paramValues, painter);
			painter->restore();
		}

		if (isSelected())
		{
			painter->setBrush(paramValues.at(IGame::SelectedCellBrush));
			painter->fillPolygon(_polygon);
		}
	}
	painter->drawPolygon(_polygon);
}
