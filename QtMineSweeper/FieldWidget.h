#ifndef FIELD_WIDGET_H
#define FIELD_WIDGET_H

#include "Painter.h"

#include <QtWidgets/QFrame>

class FieldWidget : public QFrame
{
public:
	explicit FieldWidget(IGame* game, QWidget* parent = nullptr);

	void update(const Rect& rect);

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	void resize(const QSize& size);

private:
	IGame* _game;
	Painter _painter;
};

#endif // FIELD_WIDGET_H
