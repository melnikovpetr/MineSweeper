#ifndef SETTINGS_BOX_H
#define SETTINGS_BOX_H

#include "../Common/Settings.h"

#include <QtWidgets/QGroupBox>

#include <QtCore/QCoreApplication>

#include <functional>
#include <set>

class SettingsBox : public QGroupBox
{
	Q_DECLARE_TR_FUNCTIONS(SettingsBox)

public:
	typedef std::function<void(size_t, const Value&)> ChangeHandler;
	enum TableColumns { ParamName, ParamValue, ParamDesc, _ParamAttrCount };

public:
	SettingsBox(Settings& settings, const ChangeHandler& onChange, QWidget* parent = nullptr);

	void applySettings() const;

private:
	void setupUI();
	void setupUIData(const ChangeHandler& onChange);
	void setParamValue(class QStandardItem* item, const Value& value, const Value& validator);
	Value getParamValue(class QStandardItem* item) const;

private:
	class QTableView* _tableSettings;
	Settings* _settings;
	std::set<size_t> _changedParamIndexes;
};

#endif // SETTINGS_BOX_H
