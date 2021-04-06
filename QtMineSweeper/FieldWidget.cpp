#include "FieldWidget.h"

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>

FieldWidget::FieldWidget(IGame* game, QWidget* parent)
	: QFrame(parent), _game{(assert(game), game)}
{
	resize(size());
	_game->setPainter(&_painter);
	setMouseTracking(true);
}

void FieldWidget::update(const Rect& rect)
{
	QFrame::update(_painter.mapFromSource(rect).toRect());
}

void FieldWidget::paintEvent(QPaintEvent* event)
{
	QFrame::paintEvent(event);
	QPainter painter{this};
	painter.drawPixmap(event->rect(), _painter.pixmap(), event->rect());
}

void FieldWidget::resizeEvent(QResizeEvent* event)
{
	QFrame::resizeEvent(event);
	resize(event->size());
}

void FieldWidget::mouseMoveEvent(QMouseEvent* event)
{
	QFrame::mouseMoveEvent(event);
	_game->selectCell(_painter.mapToSource(event->pos()));
}

void FieldWidget::mouseReleaseEvent(QMouseEvent* event)
{
	QFrame::mouseReleaseEvent(event);
	if (_game->selectCell(_painter.mapToSource(event->pos())))
	{
		if (event->button() == Qt::LeftButton)  { _game->openCell(); } else
		if (event->button() == Qt::RightButton)	{ _game->markCell(); }
	}
}

void FieldWidget::resize(const QSize& size)
{
	unsigned margin{};
	IPainter::parsePen(_game->paramValue(IGame::GridPen), &margin);
	_painter.resize(size, margin, _game->fieldRect());
	_game->drawField();
}
