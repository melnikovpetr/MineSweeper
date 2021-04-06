#include "SettingsBox.h"

#include "ItemDelegate.h"
#include "DeclareMetaType.h"

#include "../Common/Settings.h"

#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>

#include <QtGui/QStandardItemModel>

#include <memory>

class TableView : public QTableView
{
public:
	explicit TableView(QWidget* parent = nullptr) : QTableView(parent) {}

	QSize sizeHint() const override
	{
		QSize size{ frameWidth() * 2, frameWidth() * 2 };
		size.setWidth(size.width() + verticalHeader()->width());
		for (decltype(horizontalHeader()->count()) i = 0; i < horizontalHeader()->count(); ++i)
		{
			size.setWidth(size.width() + columnWidth(i));
		}
		size.setHeight(size.height() + horizontalHeader()->height());
		for (decltype(verticalHeader()->count()) i = 0; i < verticalHeader()->count(); ++i)
		{
			size.setHeight(size.height() + rowHeight(i));
		}
		if (showGrid())
		{
			size.setWidth(size.width() + horizontalHeader()->count() + 1);
			size.setHeight(size.height() + verticalHeader()->count() + 1);
		}
		return size;
	}

protected:
	void resizeEvent(QResizeEvent* event) override
	{
		QTableView::resizeEvent(event);
		if (horizontalHeader()->count())
		{
			auto colTotalWidth = 
				viewport()->width() - verticalHeader()->width() - showGrid()*(horizontalHeader()->count() + 1);
			auto colWidth = (colTotalWidth / horizontalHeader()->count());
			for (decltype(horizontalHeader()->count()) i = 0; i < (horizontalHeader()->count() - 1); ++i)
			{
				setColumnWidth(i, colWidth);
			}
			setColumnWidth(horizontalHeader()->count() - 1, colTotalWidth - colWidth*(horizontalHeader()->count() - 1));
		}
	}
};

void SettingsBox::setParamValue(QStandardItem* item, const Value& value, const Value& validator)
{
	assert(item);
	QVariant data{};
	QVariant validatorData{};

	if (value.typeId() == Value::MapEnum)
	{
		QStandardItem* childItem = nullptr;
		const auto& castedValue = value.castTo<Value::MapEnum>();
		QVariantList itemHeader{};
		for (const auto& childItemPair : castedValue)
		{
			item->setChild(0, item->columnCount(), childItem = new QStandardItem());
			itemHeader.push_back(QVariant::fromValue(childItemPair.first));
			setParamValue(childItem, childItemPair.second, validator.empty() ? validator : validator[childItemPair.first]);
		}
		data = itemHeader;
	} else
	if (value.typeId() != Value::Unknown)
	{
		switch (value.typeId()) 
		{
			CASE_TYPE_ID(Value::Bool,      data = QVariant::fromValue(value.castTo<TYPE_ID>()))
			CASE_TYPE_ID(Value::UInt,      data = QVariant::fromValue(value.castTo<TYPE_ID>()))
			CASE_TYPE_ID(Value::Int,       data = QVariant::fromValue(value.castTo<TYPE_ID>()))
			CASE_TYPE_ID(Value::ULongLong, data = QVariant::fromValue((qulonglong)value.castTo<TYPE_ID>()))
			CASE_TYPE_ID(Value::LongLong,  data = QVariant::fromValue((qlonglong)value.castTo<TYPE_ID>()))
			CASE_TYPE_ID(Value::Float,     data = QVariant::fromValue(value.castTo<TYPE_ID>()))
			CASE_TYPE_ID(Value::Double,    data = QVariant::fromValue(value.castTo<TYPE_ID>()))
			CASE_TYPE_ID(Value::EnumValue, data = QVariant::fromValue(value.castTo<TYPE_ID>()))
			CASE_TYPE_ID(Value::Color,     data = QVariant::fromValue(QColor(value.castTo<TYPE_ID>().toStringHex().data())))
			CASE_TYPE_ID(Value::String,    data = QVariant::fromValue(QString(value.castTo<TYPE_ID>().data())))
			default: assert(false);
		}

		if (!validator.empty())
		{
			if (validator[Validator::Type] == EnumValue{ Validator::IntRange })
			{
				validatorData = QVariantList{
					// QVariant(long unsigned int) is absent
					QVariant{ ItemDelegate::IntRange },
					QVariantList{
						QVariant{ validator[Validator::Constraint][Validator::IntRangeMin].castTo<Value::Int>() },
						QVariant{ validator[Validator::Constraint][Validator::IntRangeMax].castTo<Value::Int>() } } };
			} else
			if (validator[Validator::Type] == EnumValue{ Validator::DoubleRange })
			{
				validatorData = QVariantList{
					// QVariant(long unsigned int) is absent
					QVariant { (qulonglong)Validator::DoubleRange },
					QVariantList{
						QVariant{ validator[Validator::Constraint][Validator::DoubleRangeMin].castTo<Value::Double>() },
						QVariant{ validator[Validator::Constraint][Validator::DoubleRangeMax].castTo<Value::Double>() },
						QVariant{ validator[Validator::Constraint][Validator::DoubleRangeDecimals].castTo<Value::Int>() }} };
			} else
			if (validator[Validator::Type] == EnumValue{ Validator::RegExp })
			{
				validatorData = QVariantList{
					// QVariant(long unsigned int) is absent
					QVariant{ (qulonglong)Validator::RegExp },
					QVariant{ validator[Validator::Constraint].castTo<Value::String>().data() } };
			}
			else
				assert(false);
		}
	}
	item->setData(data, Qt::DisplayRole);
	item->setData(validatorData, ItemDelegate::ValidatorRole);
}

Value SettingsBox::getParamValue(QStandardItem* item) const
{
	assert(item);
	auto data = item->data(Qt::DisplayRole);

	if (data.userType() == qMetaTypeId<QVariantList>())
	{
		std::map<EnumValue, Value> castedValue{};
		auto itemHeader = data.value<QVariantList>();

		for (decltype(item->columnCount()) column = 0; column < item->columnCount(); column++)
		{
			castedValue[itemHeader[column].value<EnumValue>()] = getParamValue(item->child(0, column));
		}
		return Value(std::move(castedValue));
	} else
	if (data.userType() == qMetaTypeId<EnumValue>())
	{
		return Value(data.value<EnumValue>());
	}
	else
	{
		switch (data.userType())
		{
			case QMetaType::UnknownType: { return Value{}; } break;
			case QMetaType::Bool:        { return Value{ data.value<Value::Type<Value::Bool>>() }; } break;
			case QMetaType::UInt:        { return Value{ data.value<Value::Type<Value::UInt>>() }; } break;
			case QMetaType::Int:         { return Value{ data.value<Value::Type<Value::Int>>() }; } break;
			case QMetaType::ULongLong:   { return Value{ data.value<Value::Type<Value::ULongLong>>() }; } break;
			case QMetaType::LongLong:    { return Value{ data.value<Value::Type<Value::LongLong>>() }; } break;
			case QMetaType::Float:       { return Value{ data.value<Value::Type<Value::Float>>() }; } break;
			case QMetaType::Double:      { return Value{ data.value<Value::Type<Value::Double>>() }; } break;
			case QMetaType::QColor:      { return Value{ Color(data.value<QColor>().name(QColor::HexArgb).toUtf8().data()) }; } break;
			case QMetaType::QString:     { return Value{ std::string(data.value<QString>().toUtf8().data()) }; } break;
			default: assert(false);
		}
		return Value();
	}
}

SettingsBox::SettingsBox(Settings& settings, const ChangeHandler& onChange, QWidget* parent)
	: QGroupBox(parent), _tableSettings{ new TableView(this) }, _settings(&settings)
{
	setupUI();
	setupUIData(onChange);
}

void SettingsBox::applySettings() const
{
	auto lock = _settings->makeLock();
	auto model = qobject_cast<QStandardItemModel*>(_tableSettings->model());
	assert(model);
	for (auto paramIndex : _changedParamIndexes)
	{
		auto& param = _settings->param(_settings->paramIds()[paramIndex]);
		param.setValue(getParamValue(model->item(paramIndex, ParamValue)));
	}
}

void SettingsBox::setupUI()
{
	setLayout(new QVBoxLayout{ this });

	layout()->setSpacing(0);
	layout()->setContentsMargins(0, -1, 0, -1);

	setFlat(true);
	setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);

	_tableSettings->setFrameShape(QFrame::Shape::Box);
	_tableSettings->setFrameShadow(QFrame::Plain);

	_tableSettings->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked | QAbstractItemView::EditTrigger::AnyKeyPressed);
	_tableSettings->setItemDelegate(new ItemDelegate{ _tableSettings });
	_tableSettings->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	_tableSettings->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);

	_tableSettings->verticalHeader()->hide();
	_tableSettings->horizontalHeader()->setStretchLastSection(true);
	_tableSettings->horizontalHeader()->setSectionsClickable(false);

	layout()->addWidget(_tableSettings);
}

void SettingsBox::setupUIData(const ChangeHandler& onChange)
{
	assert(_settings);
	auto lock = _settings->makeLock();

	auto model = std::make_unique<QStandardItemModel>(0, _ParamAttrCount, _tableSettings);
	assert(model);

	model->setHeaderData(ParamName,  Qt::Horizontal, tr("Parameter name", "SettingsBox"), Qt::DisplayRole);
	model->setHeaderData(ParamValue, Qt::Horizontal, tr("Value"),          Qt::DisplayRole);
	model->setHeaderData(ParamDesc,  Qt::Horizontal, tr("Description"),    Qt::DisplayRole);

	for (const auto& paramId : _settings->paramIds())
	{
		const auto& param = _settings->param(paramId);
		auto items = {
			new QStandardItem(QString(param.name().data())),
			new QStandardItem(),
			new QStandardItem(QString(param.desc().data())) };

		model->appendRow(items);
		
		for (auto colIndex : { ParamName, ParamDesc })
		{
			auto item = model->item(model->rowCount() - 1, colIndex);
			item->setFlags((item->flags() & ~Qt::ItemFlag::ItemIsEditable)/* & ~Qt::ItemFlag::ItemIsSelectable*/);
		}

		setParamValue(model->item(model->rowCount() - 1, ParamValue), param.value(), param.validator());
	}

	connect(model.get(), &QStandardItemModel::itemChanged, [this, onChange](QStandardItem* item) {
		assert(item);
		auto rootItem = item;
		auto model = item->model();
		assert(model);
		while (rootItem->parent() && (rootItem->parent() != model->invisibleRootItem())) rootItem = rootItem->parent();
		
		if (rootItem->column() == ParamValue)
		{
			auto paramId = _settings->paramIds()[rootItem->row()];
			auto value = getParamValue(rootItem);
			
			_changedParamIndexes.insert(rootItem->row());
			if (onChange) onChange(paramId, value);
		}
	});
	
	_tableSettings->setModel(model.release());
	_tableSettings->resizeRowsToContents();
	_tableSettings->setMinimumHeight(
		2*_tableSettings->frameWidth()
		+ _tableSettings->horizontalHeader()->height()
		+ (_tableSettings->verticalHeader()->count()
			? _tableSettings->rowHeight(0)
			: _tableSettings->verticalHeader()->defaultSectionSize()));
	setMinimumHeight(frameSize().height() + _tableSettings->minimumHeight());
}
