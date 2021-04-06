#include "MainWindow.h"

#include "DeclareMetaType.h"

#include "../MineSweeper/MineSweeper.h"

#include <QtWidgets/QApplication>

#include <QtCore/QTranslator>

int main(int argc, char *argv[])
{
	if (getenv("QTDIR")) { QApplication::addLibraryPath(QString() + getenv("QTDIR") + "/plugins"); }

	QApplication a(argc, argv);

	QMetaType::registerConverter<EnumValue, QString>([](const EnumValue& enumValue) {
		return QString(enumValue.field<EnumValue::Name>().data()); });

	QTranslator translator{};
	if (translator.load(QLocale(), "translation", "_"))
	{
		a.installTranslator(&translator);
		for (auto& enumer : std::vector<Enum<> >{
			Enum<IGame::ParamIds>(),
			Enum<IGame::SquareSize>(),
			Enum<IGame::HexagonSize>(),
			Enum<IGame::States>(),
			Enum<IGame::TessellationIds>(),
			Enum<IPainter::BrushAttributes>(),
			Enum<IPainter::PenAttributes>(),
			})
		{
			std::vector<EnumValue::Fields> fieldsArray{};
			fieldsArray.reserve(enumer.ids().size());
			for (const auto& id : enumer.ids())
			{
				fieldsArray.push_back(std::make_tuple(
					enumer.value(id).field<EnumValue::Id>()
					, a.translate(enumer.name().data(), enumer.value(id).field<EnumValue::Name>().data()).toUtf8().data()
					, a.translate(enumer.name().data(), enumer.value(id).field<EnumValue::Desc>().data()).toUtf8().data()));
			}
			enumer.update(enumer.name(), fieldsArray);
		}
	}

	MainWindow w;
	w.show();

	return a.exec();
}
