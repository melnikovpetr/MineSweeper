#include "Game.h"

#include "GameState.h"
#include "GameState.cpp"

#include <limits>

Game::Game(Settings::ParamValues&& paramValues,
	std::unique_ptr<class Tessellation>&& tess,
	const GameEvent::Post& postEvent)
	: _paramValues{paramValues}, _field{paramValues, std::move(tess)}, _postEvent{postEvent}, _time{0},
	_state{ std::make_unique<GameState<IGame::Created>>(this) }, _painter{ nullptr }
{
	_paramValues[CellMineSize] =
		_paramValues.at(CellMineRatio).castTo<Value::typeIds<Coord>>() * _field.cellSize();

	_paramValues[CellMarkSize] =
		_paramValues.at(CellMarkRatio).castTo<Value::typeIds<Coord>>() * _field.cellSize();
	
	for (auto brushParam : { ClosedCellBrush, OpenedCellBrush, SelectedCellBrush, DetonatedCellBrush, CellMarkBrush, CellMineBrush })
	{
		Color color{};
		Color::Argb transp{};
		IPainter::parseBrush(_paramValues.at(brushParam), &color, &transp);
		color.setChannel(Color::Alfa, transp);
		_paramValues.at(brushParam) = IPainter::makeBrush(color, transp);
	}
}

Game::~Game() {}

bool Game::hasParam(ParamIds paramId) const
{
	return _paramValues.find(paramId) != _paramValues.cend();
}

void Game::drawCell() const
{
	if (_painter)
	{
		_field.drawCell(_paramValues, _painter);
		postEvent(GameEvent::Paint, _field.cell().boundingRect());
	}
}

void Game::drawField() const
{
	if (_painter)
	{
		_field.draw(_paramValues, _painter);
		postEvent(GameEvent::Paint, _field.boundingRect());
	}
}

void Game::drawCells(const std::vector<size_t>& indexes) const
{
	if (_painter && indexes.size())
	{
		std::vector<Value> rects(indexes.size());
		_field.drawCells(indexes, _paramValues, _painter);
		for (size_t i = 0; i < indexes.size(); ++i)
			rects[i] = _field.cell(indexes[i]).boundingRect();
		postEvent(GameEvent::Paint, std::move(rects));
	}
}

void Game::startTimer()
{
	_timer.start(1000 /*1 sec*/, _time, std::numeric_limits<Timer::Value>::max(), [this](Timer::Value time) {
		_time = time;
		postEvent(GameEvent::TickTack, time);
	});
}

template<typename TInfo>
void Game::postEvent(GameEvent::Types type, TInfo&& info) const
{
	_postEvent(GameEvent{ type, Value{ std::forward<TInfo>(info) } });
}

void Game::changeState(std::unique_ptr<IGame>&& state)
{
	_state = std::move(state);
	postEvent(GameEvent::State, EnumValue{_state->state()});
}
