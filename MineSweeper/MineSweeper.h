#ifndef MINE_SWEEPER_H
#define MINE_SWEEPER_H

#include "../Common/Settings.h"
#include "../Common/Polygon.h"

#include <memory>

struct IPainter
{
	enum PenAttributes : size_t
	{
		PenWidth,
		PenColor,
	};

	enum BrushAttributes : size_t
	{
		BrushTransp,
		BrushColor,
	};

	static Value makePen(unsigned width, const Color& color)
	{
		return Value{
			{ EnumValue{IPainter::PenWidth}, Value{width} },
			{ EnumValue{IPainter::PenColor}, Value{color} } };
	}

	static void parsePen(const Value& pen, unsigned* width, Color* color = nullptr)
	{
		if (width) *width = pen[EnumValue{IPainter::PenWidth}].castTo<Value::UInt>();
		if (color) *color = pen[EnumValue{IPainter::PenColor}].castTo<Value::Color>();
	}

	static Value validPen(unsigned minWidth, unsigned maxWidth)
	{
		return Value {
			{ EnumValue{IPainter::PenWidth}, Validator::intRange(minWidth, maxWidth) },
			{ EnumValue{IPainter::PenColor}, Value{} } };
	}

	static Value makeBrush(const Color& color, Color::Argb tranparency = Color::maxChannelValue)
	{
		return Value{
			{ EnumValue{IPainter::BrushColor},  Value{color} },
			{ EnumValue{IPainter::BrushTransp}, Value{tranparency} } };
	}

	static void parseBrush(const Value& brush, Color* color, Color::Argb* tranparency = nullptr)
	{
		if (color) *color = brush[EnumValue{IPainter::BrushColor}].castTo<Value::Color>();
		if (tranparency) *tranparency = brush[EnumValue{IPainter::BrushTransp}].castTo<Value::typeIds<Color::Argb>>();
	}

	static Value validBrush(Color::Argb minTransp = 0, Color::Argb maxTransp = Color::maxChannelValue)
	{
		return Value{
			{ EnumValue{IPainter::BrushColor},  Value{} },
			{ EnumValue{IPainter::BrushTransp}, Validator::intRange(minTransp, maxTransp) } };
	}

	virtual void save() = 0;

	virtual void setPen(const Value& pen) = 0;
	virtual void setBrush(const Value& brush) = 0;
	virtual void setTransform(const Transform2D& transform, bool combine) = 0;

	virtual void restore() = 0;

	virtual void drawLine(const Point2D& begin, const Point2D& end) = 0;
	virtual void drawCircle(const Point2D& center, const Coord& radius) = 0;
	virtual void drawPolygon(const Polygon& polygon) = 0;
	virtual void drawText(const std::string & text, const Rect& rect) = 0;

	virtual void fillCircle(const Point2D& center, const Coord& radius) = 0;
	virtual void fillPolygon(const Polygon& polygon) = 0;
};

class GameEvent
{
public:
	enum Types : size_t
	{
		State,
		Paint,
		MineMarked,
		TickTack,
	};

	typedef std::function<void(GameEvent&& event)> Post;

public:
	GameEvent(Types type, const Value& info) : _type(type), _info(info) { }

	Types type() const { return _type; }
	const Value& info() const { return _info; }

private:
	Types _type;
	Value _info;
};

struct IGame
{
	enum ParamIds : size_t
	{
		GridPen,

		ClosedCellBrush,
		OpenedCellBrush,
		SelectedCellBrush,
		DetonatedCellBrush,

		CellMarkRatio,
		CellMarkSize,
		CellMarkPen,
		CellMarkBrush,

		CellMineRatio,
		CellMineSize,
		CellMinePen,
		CellMineBrush,

		CellTextPen,

		MinePercent,
		MineDetonProb,

		Tessellation,
		TessellationSize,
	};

	enum TessellationIds : size_t {	Square,	Hexagon, };

	enum HexagonSize : size_t { RadiusCount, };
	enum SquareSize : size_t { RowCount, ColumnCount, };

	enum States : size_t
	{
		Created,
		Running,
		Paussed,
		Over,
		Finished,
	};

	typedef std::vector<uint8_t> Memento;

	virtual ~IGame() {}

	virtual States state() const = 0;
	virtual bool setPause(bool pause) = 0;
	// Running or Paussed
	virtual bool isRunning() const = 0;
	virtual bool hasParam(ParamIds paramId) const = 0;
	virtual const Value& paramValue(ParamIds paramId) const = 0;

	virtual Memento memento() const = 0;

	virtual size_t cellCount() const = 0;
	virtual size_t mineCount() const = 0;
	virtual size_t selectedIndex() const = 0;

	virtual bool selectCell(const Point2D& point) = 0;
	virtual bool selectCell(size_t index) = 0;
	virtual std::vector<size_t> neighbors() const = 0;
	virtual std::vector<size_t> openCell() = 0;
	virtual void markCell() = 0;
	virtual void drawCell() const = 0;
	
	virtual void setPainter(IPainter* painter) = 0;
	virtual const Rect& fieldRect() const = 0;
	virtual void drawField() const = 0;
	virtual void drawCells(const std::vector<size_t>& indexes) const = 0;
};

struct IGameFactory
{
	static Settings& globalSettings();

	static IGameFactory* instance();
	static IGameFactory* instance(const EnumValue& tessId);
	static IGameFactory* instance(const IGame::Memento& memento);

	virtual Settings& settings() = 0;
	virtual std::unique_ptr<IGame> game(const GameEvent::Post& postEvent) = 0;
	virtual std::unique_ptr<IGame> game(const IGame::Memento& memento, const GameEvent::Post& postEvent) = 0;
};

#endif // MINE_SWEEPER_H
