#include "Painter.h"

#include <QtGui/QStaticText>
#include <QtGui/QPainterPath>

QColor Painter::toQColor(const Color& color)
{
	return QColor::fromRgba(color.argb);
}

QPointF Painter::toQPointF(const Point2D& point)
{
	return { point(C2D::X), point(C2D::Y) };
}

Point2D Painter::fromQPointF(const QPointF& point)
{
	return {{ Coord(point.x()), Coord(point.y()) }};
}

QPolygonF Painter::toQPolygonF(const Polygon& polygon)
{
	QPolygonF qpolygon{};
	qpolygon.reserve(polygon.vertexCount());

	polygon.traverse([&qpolygon](Polygon::VertexCIt, Polygon::VertexCIt end, Polygon::VertexCIt it) {
		qpolygon.append(toQPointF(*it)); return (it + 1) == end;
	});

	return qpolygon;
}

QRectF Painter::toQRectF(const Rect& rect)
{
	QRectF qrect{};
	qrect.setBottomLeft(toQPointF(rect.leftBottom()));
	qrect.setTopRight(toQPointF(rect.rightTop()));
	return qrect;
}

QTransform Painter::toQTransform(const Transform2D& transform)
{
	return QTransform{
		transform(C2D::X,   C2D::X),   transform(C2D::X,   C2D::Y),   transform(C2D::Dim, C2D::X),
		transform(C2D::Y,   C2D::X),   transform(C2D::Y,   C2D::Y),   transform(C2D::Dim, C2D::Y),
		transform(C2D::X,   C2D::Dim), transform(C2D::Y,   C2D::Dim), transform(C2D::Dim, C2D::Dim) };
}

Painter::Painter()
{
	_fromSource.reserve(stackReseveSize);
	_toSource.reserve(stackReseveSize);
}

void Painter::resize(const QSize& canvas, int margin, const Rect& source)
{
	if (_painter.isActive()) _painter.end();
	_pixmap = QPixmap{ canvas };
	_pixmap.fill(Qt::transparent);
	_painter.begin(&_pixmap);
	_painter.setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);
	_source = source;
	_fromSource.clear();
	_fromSource.push_back({});
	_toSource.clear();
	_toSource.push_back({});
	setTransform(
		Transform2D::move(
				{{ (Coord)canvas.width() / 2, (Coord)canvas.height() / 2 }})
			* Transform2D::resize(
				{{ Coord(canvas.width() - margin) / _source.width(), -Coord(canvas.height() - margin) / source.height() }}),
		false);
} 

Point2D Painter::mapToSource(const QPointF& point) const
{
	return fromQPointF(_toSource.back().map(point));
}

QRectF Painter::mapFromSource(const Rect& rect) const
{
	return toQRectF(rect.applyToCopy(_fromSource.back()));
}

QPointF Painter::mapFromSource(const Point2D& point) const
{
	return toQPointF(_fromSource.back() * point);
}

QPolygonF Painter::mapFromSource(const Polygon& polygon) const
{
	return toQPolygonF(polygon.applyToCopy(_fromSource.back()));
}

void Painter::save()
{
	_fromSource.push_back(_fromSource.back());
	_toSource.push_back(_toSource.back());
	_painter.save();
}

void Painter::setPen(const Value& pen)
{
	unsigned width{};
	QPen qpen{};
	Color color{};
	parsePen(pen, &width, &color);
	qpen.setBrush(toQColor(color));
	qpen.setWidth(width);
	_painter.setPen(qpen);
}

void Painter::setBrush(const Value& brush)
{
	Color color{};
	parseBrush(brush, &color);
	_painter.setBrush(QBrush{ toQColor(color) });
}

void Painter::setTransform(const Transform2D& transform, bool combine)
{
	_fromSource.back() = combine ? _fromSource.back() * transform : transform;
	_toSource.back() = toQTransform(_fromSource.back()).inverted();
	
}

void Painter::restore()
{
	if (_fromSource.count() > 1)
	{
		_fromSource.pop_back();
		_toSource.pop_back();
	}
	_painter.restore();
}

void Painter::drawLine(const Point2D& begin, const Point2D& end)
{
	_painter.drawLine(mapFromSource(begin), mapFromSource(end));
}

void Painter::drawCircle(const Point2D& center, const Coord& radius)
{
	QSizeF size = {
		std::abs(_fromSource.back()(C2D::X, C2D::X)*radius),
		std::abs(_fromSource.back()(C2D::Y, C2D::Y)*radius) };
	auto leftTop = mapFromSource(center) - QPointF{ size.width(), size.height() };
	_painter.drawArc(QRectF{ leftTop, 2.0*size }, 0, 360 * 16 /* specified in 1/16th of a degree */);
}

void Painter::drawPolygon(const Polygon& polygon)
{
	polygon.traverse([this](Polygon::VertexCIt begin, Polygon::VertexCIt end, Polygon::VertexCIt curr) {
		auto next = curr + 1;
		_painter.drawLine(mapFromSource(*curr), mapFromSource(next == end ? *begin : *next));
		return (curr + 1) == end;
	});
}

void Painter::drawText(const std::string& text, const Rect& rect)
{
	QString qtext{ text.data() };
	QRectF qrect{ mapFromSource(rect) };
	auto textRect = _painter.fontMetrics().tightBoundingRect(qtext);
	auto kx = qrect.width()  / qreal(textRect.width());
	auto ky = qrect.height() / qreal(textRect.height());
	auto k = kx < ky ? kx : ky;
	auto font = _painter.font();

	font.setPointSizeF(font.pointSizeF() * k);
	_painter.setFont(font);
	_painter.drawText(qrect.bottomLeft(), qtext);
}

void Painter::fillCircle(const Point2D& center, const Coord& radius)
{
	QSizeF size{
		std::abs(_fromSource.back()(C2D::X, C2D::X) * radius),
		std::abs(_fromSource.back()(C2D::Y, C2D::Y) * radius) };
	_painter.save();
	_painter.setPen(Qt::NoPen);
	_painter.drawEllipse(mapFromSource(center), size.width(), size.height());
	_painter.restore();
}

void Painter::fillPolygon(const Polygon& polygon)
{
	QPainterPath qpath{};
	qpath.addPolygon(mapFromSource(polygon));
	qpath.closeSubpath();
	_painter.fillPath(qpath, _painter.brush());
}

