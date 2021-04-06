#include "Rect.h"

Rect::Rect(const Point2D& leftBottom, const Point2D& rightTop)
	: _leftBottom(leftBottom), _rightTop(rightTop)
{}

Point2D Rect::center() const
{
	return 0.5f * (_leftBottom + _rightTop);
}

Coord Rect::width() const
{
	return _rightTop(C2D::X) - _leftBottom(C2D::X);
}

Coord Rect::height() const
{
	return _rightTop(C2D::Y) - _leftBottom(C2D::Y);
}

Rect Rect::applyToCopy(const Transform2D& transform) const
{
	return Rect(*this).apply(transform);
}

Rect& Rect::apply(const Transform2D& transform)
{
	_leftBottom.apply(transform);
	_rightTop.apply(transform);
	return *this;
}

bool Rect::operator ==(const Rect& other) const
{
	return (_leftBottom == other._leftBottom) && (_rightTop == other.rightTop());
}
