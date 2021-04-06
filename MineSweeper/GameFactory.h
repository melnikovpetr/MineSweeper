#ifndef GAME_FACTORY_H
#define GAME_FACTORY_H

#include "Game.h"

template<typename TTessellation>
class GameFactory : public IGameFactory
{
public:
	GameFactory();

	Settings& settings() override { return _settings; }

	std::unique_ptr<IGame> game(const GameEvent::Post& postEvent) override
	{
		auto paramValues = globalSettings().paramValues();
		for (const auto& paramValue :_settings.paramValues()) paramValues.insert(paramValue);

		return std::make_unique<Game>(
			std::move(paramValues),
			std::make_unique<TTessellation>(paramValues.at(IGame::TessellationSize)),
			postEvent);
	}

	std::unique_ptr<IGame> game(const IGame::Memento&, const GameEvent::Post&) override
	{
		return std::unique_ptr<Game>{};
	}

private:
	Settings _settings;
};

#endif // GAME_FACTORY_H
