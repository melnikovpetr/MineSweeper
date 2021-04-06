#ifndef GAME_H
#define GAME_H

#include "../Common/Timer.h"

#include "MineSweeper.h"
#include "Field.h"

class Game : public IGame
{
	template<IGame::States state>
	friend class GameState;

public:
	Game(
		Settings::ParamValues&& paramValues,
		std::unique_ptr<class Tessellation>&& tess,
		const GameEvent::Post& postEvent);
	~Game();

	States state() const override { assert(_state); return _state->state(); }
	bool isRunning() const override { return (state() == Running) || (state() == Paussed); }
	bool hasParam(ParamIds paramId) const override;
	const Value& paramValue(ParamIds paramId) const override { return _paramValues.at(paramId); }
	bool setPause(bool pause) override { assert(_state); return _state->setPause(pause); }

	Memento memento() const override { return Memento{}; }

	size_t cellCount() const override { return _field.cellCount(); }
	size_t mineCount() const override { return _field.mineCount(); }
	size_t selectedIndex() const override { return _field.selectedIndex(); }

	bool selectCell(const Point2D& point) override { assert(_state); return _state->selectCell(point); }
	bool selectCell(size_t index) override { assert(_state); return _state->selectCell(index); }
	std::vector<size_t> neighbors() const override { return _field.neighbors();	}
	std::vector<size_t> openCell() override { assert(_state); return _state->openCell(); }
	void markCell() override { assert(_state); _state->markCell(); }
	void drawCell() const override;

	void setPainter(IPainter* painter) override { assert(_state); _painter = painter; _state->drawField(); }
	const Rect& fieldRect() const override { return _field.boundingRect(); }
	void drawField() const override;
	void drawCells(const std::vector<size_t>& indexes) const override;

protected:
	void startTimer();
	template<typename TInfo>
	void postEvent(GameEvent::Types type, TInfo&& info) const;
	void changeState(std::unique_ptr<IGame>&& state);

private:
	Settings::ParamValues _paramValues;
	Field _field;
	GameEvent::Post _postEvent;
	Timer _timer;
	Timer::Value _time;
	std::unique_ptr<IGame> _state;
	IPainter* _painter;
};

#endif // GAME_H
