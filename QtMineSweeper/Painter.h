#ifndef PAINTER_H
#define PAINTER_H

#include "../MineSweeper/MineSweeper.h"

#include <QtGui/QPainter>

class Painter :	public IPainter
{
public:
	static QColor toQColor(const Color& color);
	static QPointF toQPointF(const Point2D& point);
	static Point2D fromQPointF(const QPointF& point);
	static QPolygonF toQPolygonF(const Polygon& polygon);
	static QRectF toQRectF(const Rect& rect);
	static QTransform toQTransform(const Transform2D &transform);

public:
	Painter();

	void resize(const QSize & canvas, int maxPenWidth, const Rect & source);

	Point2D mapToSource(const QPointF& point) const;
	QRectF mapFromSource(const Rect& rect) const;
	QPointF mapFromSource(const Point2D& point) const;
	QPolygonF mapFromSource(const Polygon& polygon) const;
	const QPixmap& pixmap() const { return _pixmap; }

	void save() override;

	void setPen(const Value& pen) override;
	void setBrush(const Value& brush)override;
	void setTransform(const Transform2D& transform, bool combine) override;

	void restore() override;

	void drawLine(const Point2D& begin, const Point2D& end) override;
	void drawCircle(const Point2D& center, const Coord& radius) override;
	void drawPolygon(const Polygon& polygon) override;
	void drawText(const std::string & text, const Rect& rect) override;

	void fillCircle(const Point2D& center, const Coord& radius) override;
	void fillPolygon(const Polygon& polygon) override;

private:
	static constexpr int stackReseveSize = 10;

	QPixmap  _pixmap;
	QPainter _painter;
	Rect _source;
	QList<Transform2D> _fromSource;
	QList<QTransform> _toSource;
};

#endif // PAINTER_H
