#include "GameState.h"

#include "Game.h"

template<>
bool GameState<IGame::Running>::setPause(bool pause)
{
	if (pause)
	{
		_game->_timer.stop();
		_game->changeState(std::make_unique<GameState<IGame::Paussed>>(_game));
		return true;
	}
	return false;
}

template<>
bool GameState<IGame::Paussed>::setPause(bool pause)
{
	if (!pause)
	{
		_game->startTimer();
		_game->changeState(std::make_unique<GameState<IGame::Running>>(_game));
		return true;
	}
	return false;
}

template<>
bool GameState<IGame::Created>::selectCell(size_t index)
{
	size_t oldIndex{};
	if ((_game->_field.selectCell(index, oldIndex) != oldIndex) && _game->_painter)
	{
		if (oldIndex < _game->cellCount()) drawCells({oldIndex});
		if (_game->selectedIndex() < _game->cellCount()) drawCell();
	}
	return _game->selectedIndex() < _game->cellCount();
}

template<>
bool GameState<IGame::Created>::selectCell(const Point2D& point)
{
	return selectCell(_game->_field.cellIndex(point));
}

template<>
bool GameState<IGame::Running>::selectCell(size_t index)
{
	return GameState<IGame::Created>(_game).selectCell(index);
}

template<>
bool GameState<IGame::Running>::selectCell(const Point2D& point)
{
	return GameState<IGame::Created>(_game).selectCell(point);
}

template<>
std::vector<size_t> GameState<IGame::Created>::openCell()
{
	auto game = _game;
	const auto& field = _game->_field;

	if (field.cell().hasMine())
	{
		size_t index = 0;
		while(field.cell(index).hasMine() && index < game->cellCount())
			++index;
		assert(!field.cell(index).hasMine());
		game->_field.moveMine(game->_field.selectedIndex(), index);
	}
	game->changeState(std::make_unique<GameState<IGame::Running>>(game));
	game->startTimer();
	return game->openCell();
}

template<>
std::vector<size_t> GameState<IGame::Running>::openCell()
{
	const auto& field = _game->_field;
	auto cells = _game->_field.openCell();
	std::vector<size_t> additionalCells{};
	std::unique_ptr<IGame> state{};

	if (cells.size() && field.cell().isDetonated())
	{
		_game->_timer.stop();
		additionalCells = _game->_field.openMines();
		state = std::make_unique<GameState<IGame::Over>>(_game);
	} else
	if (mineCount() == (cellCount() - field.openedCellCount()))
	{
		_game->_timer.stop();
		additionalCells = _game->_field.openMines();
		state = std::make_unique<GameState<IGame::Finished>>(_game);
	}

	cells.insert(cells.end(), additionalCells.cbegin(), additionalCells.cend());
	drawCells(cells);

	if (state) _game->changeState(std::move(state));

	return cells;
}

template<>
void GameState<IGame::Running>::markCell()
{
	auto mineMarkCount = _game->_field.mineMarkCount();
	_game->_field.markCell();
	if (_game->_field.mineMarkCount() != mineMarkCount)
		_game->postEvent(GameEvent::MineMarked, _game->_field.mineMarkCount());
	drawCell();
}

template<>
void GameState<IGame::Created>::markCell()
{
	GameState<IGame::Running>{_game}.markCell();
}
