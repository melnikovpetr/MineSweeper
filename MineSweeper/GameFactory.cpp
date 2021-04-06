#include "GameFactory.h"

#include "TessHexagon.h"
#include "TessSquare.h"

ENUM_NAMES(IGame::ParamIds, {
	{ IGame::GridPen,            "Grid pen",                    "" },

	{ IGame::ClosedCellBrush,    "Closed cell brush",           "" },
	{ IGame::OpenedCellBrush,    "Opened cell brush",           "" },
	{ IGame::SelectedCellBrush,  "Selected cell brush",         "" },
	{ IGame::DetonatedCellBrush, "Detonated cell brush",        "" },

	{ IGame::CellMarkRatio,      "Cell mark size ratio",        "" },
	{ IGame::CellMarkSize,       "Cell mark size",              "" },
	{ IGame::CellMarkPen,        "Cell mark pen",               "" },
	{ IGame::CellMarkBrush,      "Cell mark brush",             "" },

	{ IGame::CellMineRatio,      "Cell mine size ratio",        "" },
	{ IGame::CellMineSize,       "Cell mine size",              "" },
	{ IGame::CellMinePen,        "Cell mine pen",               "" },
	{ IGame::CellMineBrush,      "Cell mine brush",             "" },

	{ IGame::CellTextPen,        "Cell text pen",               "" },

	{ IGame::MinePercent,        "Percent of cells with mine",  "" },
	{ IGame::MineDetonProb,      "Mine detonation probability", "" },

	{ IGame::Tessellation,       "Tessellation",                "" },
	{ IGame::TessellationSize,   "Field size",                  "" } })

ENUM_NAMES(IGame::TessellationIds, {
	{ IGame::Square,  "Square",  "Regular tessellation of the plane by squares" },
	{ IGame::Hexagon, "Hexagon", "Regular tessellation of the plane by hexagons" } })

ENUM_NAMES(IPainter::PenAttributes, {
	{ IPainter::PenWidth, "Width", "" },
	{ IPainter::PenColor, "Color", "" } })

ENUM_NAMES(IPainter::BrushAttributes, {
	{ IPainter::BrushTransp, "Transparency", "" },
	{ IPainter::BrushColor,  "Color",        "" } })

ENUM_NAMES(IGame::States, {
	{ IGame::Created,  "created",  "" },
	{ IGame::Running,  "running",  "" },
	{ IGame::Paussed,  "paussed",  "" },
	{ IGame::Over,     "over",     "" },
	{ IGame::Finished, "finished", "" } })

ENUM_NAMES(IGame::SquareSize, {
	{ IGame::RowCount,    "Row count",    "" },
	{ IGame::ColumnCount, "Column count", "" } })

ENUM_NAMES(IGame::HexagonSize, {
	{ IGame::RadiusCount, "Radius count", "" } })

template<>
GameFactory<TessHexagon>::GameFactory() : _settings {
	{
		EnumValue{ IGame::TessellationSize },
		Value{ { EnumValue{ IGame::RadiusCount }, Value{ size_t{5} } } },
		Value{ { EnumValue{ IGame::RadiusCount }, Validator::intRange(2, 15) } }
	} }
{}

template<>
GameFactory<TessSquare>::GameFactory() : _settings {
	{
		EnumValue{ IGame::TessellationSize},
		Value{
			{ EnumValue{ IGame::RowCount },    Value{ size_t{8} } },
			{ EnumValue{ IGame::ColumnCount }, Value{ size_t{8} } } },
		Value{
			{ EnumValue{ IGame::RowCount },    Validator::intRange(4, 100) },
			{ EnumValue{ IGame::ColumnCount }, Validator::intRange(4, 100) } }
	} }
{}

Settings& IGameFactory::globalSettings()
{
	static Settings settings{
		{ EnumValue{ IGame::GridPen },            IPainter::makePen(2, 0xFF000000),      IPainter::validPen(1, 4) },

		{ EnumValue{ IGame::ClosedCellBrush },    IPainter::makeBrush(0xFF0000FF),       IPainter::validBrush() },
		{ EnumValue{ IGame::OpenedCellBrush },    IPainter::makeBrush(0xFFFFFFFF),       IPainter::validBrush() },
		{ EnumValue{ IGame::SelectedCellBrush },  IPainter::makeBrush(0xFFF0F8FF, 0x80), IPainter::validBrush() },
		{ EnumValue{ IGame::DetonatedCellBrush }, IPainter::makeBrush(0xFFFFFF00),       IPainter::validBrush() },

		{ EnumValue{ IGame::CellMarkRatio },      Value{ Coord(0.8) },                   Validator::doubleRange(0.4, 1.0, 1) },
		{ EnumValue{ IGame::CellMarkPen },        IPainter::makePen(2, 0xFF000000),      IPainter::validPen(1, 4) },
		{ EnumValue{ IGame::CellMarkBrush },      IPainter::makeBrush(0xFFFF0000, 0xFF), IPainter::validBrush() },

		{ EnumValue{ IGame::CellMineRatio },      Value{ Coord(0.8) },                   Validator::doubleRange(0.4, 1.0, 1) },
		{ EnumValue{ IGame::CellMinePen },        IPainter::makePen(2, 0xFF000000),      IPainter::validPen(1, 4) },
		{ EnumValue{ IGame::CellMineBrush },      IPainter::makeBrush(0xFFFF0000, 0xFF), IPainter::validBrush() },

		{ EnumValue{ IGame::CellTextPen },        IPainter::makePen(2, 0xFF000000),      IPainter::validPen(1, 4) },

		{ EnumValue{ IGame::MinePercent },        Value{ 20 },                           Validator::intRange(1, 96) },
		{ EnumValue{ IGame::MineDetonProb },      Value{ 1.0 },                          Validator::doubleRange(0.1, 1.0, 2) },

		{ EnumValue{ IGame::Tessellation },       Value{ EnumValue{ IGame::Hexagon } } } };

	return settings;
}

IGameFactory* IGameFactory::instance()
{
	return instance(globalSettings().param(IGame::Tessellation).value().castTo<Value::EnumValue>());
}

IGameFactory* IGameFactory::instance(const EnumValue& tessId)
{
	static auto factories([]() {
		std::map <EnumValue, std::unique_ptr<IGameFactory>> factories;
		factories[EnumValue{ IGame::Hexagon }] = std::unique_ptr<IGameFactory>(new GameFactory<TessHexagon>{});
		factories[EnumValue{ IGame::Square }]  = std::unique_ptr<IGameFactory>(new GameFactory<TessSquare>{});
		return factories; } ());

	return factories.find(tessId) == factories.end() ? nullptr : factories.find(tessId)->second.get();
}

IGameFactory* IGameFactory::instance(const IGame::Memento& memento)
{
	assert(memento.size());
	return instance(EnumValue{});
}
