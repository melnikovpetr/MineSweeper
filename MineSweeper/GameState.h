#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "Game.h"

template<IGame::States tState>
class GameState : public IGame
{
public:
	GameState(class Game* game) : _game(game) { assert(_game); }

	States state() const override { return tState; }
	bool isRunning() const override { return _game->isRunning(); };
	bool hasParam(ParamIds paramId) const override { return _game->hasParam(paramId); }
	const Value& paramValue(ParamIds paramId) const override { return _game->paramValue(paramId); }
	bool setPause(bool pause) override { return false; }

	Memento memento() const override { return _game->memento(); }

	size_t cellCount() const override { return _game->cellCount(); }
	size_t mineCount() const override { return _game->mineCount(); }
	size_t selectedIndex() const override { return _game->selectedIndex(); }

	bool selectCell(const Point2D&) override { return false; }
	bool selectCell(size_t) override { return false; }
	std::vector<size_t> neighbors() const override { return _game->neighbors(); }
	std::vector<size_t> openCell() override { return std::vector<size_t>{}; }
	void markCell() override {}
	void drawCell() const override	{ _game->drawCell(); }

	void setPainter(IPainter* painter) override { _game->setPainter(painter); }
	const Rect& fieldRect() const override { return _game->fieldRect(); }
	void drawField() const override { _game->drawField(); }
	void drawCells(const std::vector<size_t>& indexes) const override { _game->drawCells(indexes); }

private:
	Game* _game;
};
/*
template<> bool GameState<IGame::Running>::setPause(bool pause);
template<> bool GameState<IGame::Paussed>::setPause(bool pause);

template<> bool GameState<IGame::Created>::selectCell(size_t index);
template<> bool GameState<IGame::Created>::selectCell(const Point2D& point);

template<> bool GameState<IGame::Running>::selectCell(size_t index);
template<> bool GameState<IGame::Running>::selectCell(const Point2D& point);

template<> std::vector<size_t> GameState<IGame::Created>::openCell();
template<> std::vector<size_t> GameState<IGame::Running>::openCell();

template<> void GameState<IGame::Created>::markCell();
template<> void GameState<IGame::Running>::markCell();
*/
#endif // GAME_STATE_H
