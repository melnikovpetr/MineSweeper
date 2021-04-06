#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtWidgets/QMainWindow>

#include <QtCore/QCoreApplication>

#include <memory>

class MainWindow : public QMainWindow
{
	Q_DECLARE_TR_FUNCTIONS(MainWindow)

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private:
	void setupUI();
	void setupUIData();
	void customEvent(QEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	QAction* _actnNewGame  = nullptr;
	QAction* _actnLoadGame = nullptr;
	QAction* _actnSaveGame = nullptr;
	QAction* _actnPause    = nullptr;
	QAction* _actnSettings = nullptr;
	QAction* _actnExit     = nullptr;
	QAction* _actnAbout    = nullptr;
	
	QMenu* _menuFile  = nullptr;
	QMenu* _menuHelp  = nullptr;

	class QMessageBox* _loseGameMBox = nullptr;
	class QMessageBox* _notImplMBox  = nullptr;

	QString _gameStateLabel;
	QString _gameTimeLabel;
	QString _gameMinesLabel;

	class QLabel* _gameState = nullptr;
	class QLabel* _gameTime = nullptr;
	class QLabel* _gameMines = nullptr ;

	std::unique_ptr<struct IGame>      _game;
	std::unique_ptr<class FieldWidget> _fieldWidget;
};

#endif // MAIN_WINDOW_H
