#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "../Common/EnumValue.h"

#include <QtWidgets/QDialog>

#include <QtCore/QCoreApplication>

#include <map>

class SettingsDialog : public QDialog
{
	Q_DECLARE_TR_FUNCTIONS(SettingsDialog)

public:
	SettingsDialog(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);

private:
	void setupUI();
	void setupUIData();

private:
	class SettingsBox* _settingsBoxGlobal;
	std::map<EnumValue, class SettingsBox*> _settingsBoxTess;
	class QDialogButtonBox* _bttnBoxOkCancel;
};

#endif // SETTINGS_DIALOG_H