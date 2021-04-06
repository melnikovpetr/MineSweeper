#include "SettingsDialog.h"

#include "SettingsBox.h"

#include "../MineSweeper/MineSweeper.h"

#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDesktopWidget>

SettingsDialog::SettingsDialog(QWidget* parent, Qt::WindowFlags flags) : QDialog(parent, flags)
{
	setupUI();
	setupUIData();
}

void SettingsDialog::setupUI()
{
	setWindowFlags((windowFlags() | Qt::WindowMaximizeButtonHint) & ~Qt::WindowContextHelpButtonHint);
	setLayout(new QVBoxLayout{ this });
	_settingsBoxGlobal = new SettingsBox {
		IGameFactory::globalSettings(),
		[this](size_t paramId, const Value& value) {
			if (paramId == IGame::Tessellation)
			{
				auto& tessEnumValue = value.castTo<Value::EnumValue>();
				for (const auto& pair : _settingsBoxTess)
				{
					pair.second->setVisible(pair.first == tessEnumValue);
				}
			}
		},
		this };

	layout()->addWidget(_settingsBoxGlobal);

	for (const auto& tessId : Enum<IGame::TessellationIds>().ids())
	{
		auto tessEnumValue = Enum<IGame::TessellationIds>().value(tessId);
		_settingsBoxTess[tessEnumValue] = new SettingsBox {
			IGameFactory::instance(tessEnumValue)->settings(),
			[](size_t, const Value&) {},
			this };

		_settingsBoxTess[tessEnumValue]->setVisible(
				IGameFactory::globalSettings().param(IGame::Tessellation).value() == tessEnumValue);

		layout()->addWidget(_settingsBoxTess[tessEnumValue]);
	}

	_bttnBoxOkCancel = new QDialogButtonBox {
		QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel,
		Qt::Horizontal,
		this };

	connect(_bttnBoxOkCancel, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
	connect(_bttnBoxOkCancel, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);

	layout()->addWidget(_bttnBoxOkCancel);

	resize(QDesktopWidget().availableGeometry(this).size() * 0.5);

	connect(this, &QDialog::accepted, [this]() {
		_settingsBoxGlobal->applySettings();
		for (const auto& tessId : Enum<IGame::TessellationIds>().ids())
		{
			auto tessEnumValue = Enum<IGame::TessellationIds>().value(tessId);
			_settingsBoxTess[tessEnumValue]->applySettings();
		}
	});
}

void SettingsDialog::setupUIData()
{
	setWindowTitle(tr("Settings"));
	_settingsBoxGlobal->setTitle(tr("Global"));
	for (const auto& tessId : Enum<IGame::TessellationIds>().ids())
	{
		auto tessEnumValue = Enum<IGame::TessellationIds>().value(tessId);
		_settingsBoxTess[tessEnumValue]->setTitle(tr(tessEnumValue.field<EnumValue::Name>().data()));
	}
	_bttnBoxOkCancel->button(QDialogButtonBox::StandardButton::Ok)->setText(tr("Ok"));
	_bttnBoxOkCancel->button(QDialogButtonBox::StandardButton::Cancel)->setText(tr("Cancel"));
}
