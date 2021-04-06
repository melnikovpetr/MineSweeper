#include "MainWindow.h"

#include "FieldWidget.h"
#include "SettingsDialog.h"

#include "../MineSweeper/MineSweeper.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QAbstractButton>

#include <QtGui/QCloseEvent>

#include <QtCore/QTime>

class GameEventWrapper : public QEvent
{
public:
	enum { GameEventId = QEvent::User };

public:
	GameEventWrapper(GameEvent&& event) : QEvent((Type)GameEventId), _event{ std::move(event) } {}
	const GameEvent& event() const { return _event; }

private:
	GameEvent _event;
};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	setupUI();
	setupUIData();
	_actnNewGame->trigger();
	//_actnSettings->trigger();
}

MainWindow::~MainWindow() { }

void MainWindow::setupUI()
{
	auto windowsSize = QDesktopWidget().availableGeometry(this).size() / 2;
	auto menuBar = new QMenuBar(this);
	auto statusBar = new QStatusBar(this);

	_menuFile = new QMenu(menuBar);
	_menuFile->addAction(_actnNewGame   = new QAction(this));
	_menuFile->addAction(_actnLoadGame  = new QAction(this));
	_menuFile->addAction(_actnSaveGame  = new QAction(this));
	_menuFile->addAction(_actnPause     = new QAction(this));
	_menuFile->addAction(_actnSettings  = new QAction(this));
	_menuFile->addSeparator();
	_menuFile->addAction(_actnExit      = new QAction(this));
	menuBar->addAction(_menuFile->menuAction());

	_menuHelp = new QMenu(menuBar);
	_menuHelp->addAction(_actnAbout = new QAction(this));
	menuBar->addAction(_menuHelp->menuAction());

	setMenuBar(menuBar);

	statusBar->addPermanentWidget(_gameState = new QLabel(this));
	statusBar->addPermanentWidget(_gameTime  = new QLabel(this));
	statusBar->addPermanentWidget(_gameMines = new QLabel(this));
	setStatusBar(statusBar);

	_loseGameMBox = new QMessageBox(QMessageBox::Question, {}, {}, QMessageBox::No | QMessageBox::Yes, this);
	_notImplMBox  = new QMessageBox(QMessageBox::Warning,  {}, {}, QMessageBox::Ok,                    this);

	resize(windowsSize.width() > windowsSize.height() 
		? QSize{ windowsSize.height(), windowsSize.height() }
		: QSize{ windowsSize.width(),  windowsSize.width() });

	setMinimumSize(size() / 2);

	connect(_actnSettings, &QAction::triggered, [this]() {
		SettingsDialog dialog(this);
		while(true)
		{
			auto  result = (dialog.exec() == QDialog::Accepted);
			if (IGameFactory::globalSettings().param(EnumValue(IGame::Tessellation)).value()
				!= EnumValue(IGame::Hexagon))
			{
				_notImplMBox->exec();
				continue;
			}
			if (result) { _actnNewGame->trigger(); };
			break;
		}
	});

	connect(_actnNewGame, &QAction::triggered, [this]() {
		if (_game && _game->isRunning() && (_loseGameMBox->exec() == QMessageBox::No)) return;
		_game = IGameFactory::instance()->game([this](GameEvent&& event) {
			QApplication::postEvent(this, new GameEventWrapper{ std::move(event) }); });
		_fieldWidget.reset(new FieldWidget{ _game.get(), this });
		setCentralWidget(_fieldWidget.get());
		_gameState->setText(_gameStateLabel + " " + EnumValue(_game->state()).field<EnumValue::Name>().data());
		_gameTime->setText(_gameTimeLabel   + " " + QTime::fromMSecsSinceStartOfDay(0).toString("mm:ss"));
		_gameMines->setText(_gameMinesLabel + " " + QString::number(0) + "/" + QString::number(_game->mineCount()));
	});

	connect(_actnLoadGame, &QAction::triggered, [this]() {
		if (_game && _game->isRunning() && (_loseGameMBox->exec() == QMessageBox::No)) return;
		_notImplMBox->exec();
	});

	connect(_actnSaveGame, &QAction::triggered, [this]() {
		_notImplMBox->exec();
	});

	connect(_actnPause, &QAction::triggered, [this]() {
		if (_game) _game->setPause(_game->state() != IGame::Paussed);
	});

	connect(_actnExit, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::setupUIData()
{
	_menuFile->setTitle(tr("&File"));
	_actnNewGame->setText(tr("&New Game"));
	_actnLoadGame->setText(tr("&Load Game"));
	_actnSaveGame->setText(tr("&Save Game"));
	_actnPause->setText(tr("&Pause/Resume"));
	_actnSettings->setText(tr("Se&ttings"));
	_actnExit->setText(tr("&Exit"));

	_menuHelp->setTitle(tr("&Help"));
	_actnAbout->setText(tr("&About"));

	_gameStateLabel = tr("Game state:");
	_gameTimeLabel = tr("Elapsed time:");
	_gameMinesLabel = tr("Mines left:");

	_loseGameMBox->setWindowTitle(tr("Question"));
	_loseGameMBox->setText(tr("The current game will be lost! Continue?"));
	_loseGameMBox->button(QMessageBox::Yes)->setText(tr("Yes"));
	_loseGameMBox->button(QMessageBox::No)->setText(tr("No"));

	_notImplMBox->setWindowTitle(tr("Warning"));
	_notImplMBox->setText(tr("This functional is not implemented!"));
}

void MainWindow::customEvent(QEvent* event)
{
	assert(event);
	if (event->type() == (QEvent::Type)GameEventWrapper::GameEventId)
	{
		auto& castedEvent = static_cast<GameEventWrapper*>(event)->event();

		if (castedEvent.type() == GameEvent::State)
		{
			_gameState->setText(_gameStateLabel + " " + castedEvent.info().toString().data());
		} else
		if (castedEvent.type() == GameEvent::TickTack)
		{
			_gameTime->setText(
				_gameTimeLabel + " " +
				QTime::fromMSecsSinceStartOfDay(
					castedEvent.info().castTo<Value::typeIds<std::chrono::milliseconds::rep>>()).toString("mm:ss"));
		} else
		if (castedEvent.type() == GameEvent::MineMarked)
		{
			_gameMines->setText(
				_gameMinesLabel + " " +
				QString::number(castedEvent.info().castTo<Value::typeIds<size_t>>()) +
				"/" +
				QString::number(_game->mineCount()));
		} else
		if (castedEvent.type() == GameEvent::Paint)
		{
			std::vector<Value> rects{};
			if (castedEvent.info().typeId() == Value::Rect)   { rects.push_back(castedEvent.info()); } else
			if (castedEvent.info().typeId() == Value::Vector) { rects = castedEvent.info().castTo<Value::Vector>(); }
			else assert(false);

			for (auto& rect : rects) _fieldWidget->update(rect.castTo<Value::Rect>());
		}
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (_game && _game->isRunning() && (_loseGameMBox->exec() == QMessageBox::No)) event->ignore();
	else QMainWindow::closeEvent(event);
}
