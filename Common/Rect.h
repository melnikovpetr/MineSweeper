#ifndef RECT_H
#define RECT_H

#include "Point.h"

class Rect
{
public:
	Rect(const Point2D& leftBottom = {}, const Point2D& rightTop = {});

	const Point2D& leftBottom() const { return _leftBottom; }
	const Point2D& rightTop() const { return _rightTop; }

	const Coord& left() const { return _leftBottom(C2D::X); }
	void setLeft(const Coord& left) { _leftBottom(C2D::X) = left; }

	const Coord& bottom() const { return _leftBottom(C2D::Y); }
	void setBottom(const Coord& bottom) { _leftBottom(C2D::Y) = bottom; }

	const Coord& right() const { return _rightTop(C2D::X); }
	void setRight(const Coord& right) { _rightTop(C2D::X) = right; }

	const Coord& top() const { return _rightTop(C2D::Y); }
	void setTop(const Coord& top) { _rightTop(C2D::Y) = top; }

	Point2D center() const;
	Coord width() const;
	Coord height() const;

	Rect applyToCopy(const Transform2D& transform) const;
	Rect& apply(const Transform2D& transform);

	bool operator == (const Rect& other) const;

private:
	Point2D _leftBottom;
	Point2D _rightTop;
};

#endif // RECT_H
