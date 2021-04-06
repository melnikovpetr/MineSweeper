#include "ItemDelegate.h"

#include "DeclareMetaType.h"
#include "SubProxyModel.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

#include <QtGui/QPainter>
#include <QtGui/QStandardItemModel>
#include <QtGui/QShowEvent>

#include <QtCore/QEvent>
#include <QtCore/QDebug>

#include <memory>

class ItemDelegate::EnumDataModel : public QStandardItemModel
{
public:
	enum DataRoles : int
	{
		EnumValueRole = Qt::UserRole
	};

public:
	explicit EnumDataModel(const Enum<>& enum_, QObject* parent = nullptr)
		: QStandardItemModel(parent)
	{
		assert(enum_);
		setColumnCount(1);
		setRowCount(enum_.ids().size());
		
		for (size_t row = 0; row < enum_.ids().size(); ++row)
		{
			auto enumValue = enum_.value(enum_.ids()[row]);
			setItem(row, 0, new QStandardItem{ enumValue.field<EnumValue::Name>().data() });
			item(row, 0)->setData(QVariant::fromValue<EnumValue>(enumValue), EnumValueRole);
		}
	}
};

class ItemDelegate::ColorDataModel : public QStandardItemModel
{
public:
	enum DataRoles : int
	{
		ColorRole = Qt::UserRole
	};

public:
	static QString colorName(const QColor& color)
	{
		static const auto names = []() {
			QHash<QString, QString> names{};
			for (const auto& name : QColor::colorNames())
			{
				names[QColor(name).name(QColor::NameFormat::HexArgb)] = name;
			}
			return names;
		}();
		auto name = names.find(color.name(QColor::NameFormat::HexArgb));

		return (name == names.end()) ? color.name(QColor::NameFormat::HexArgb) : name.value();
	}

	static QIcon colorToIcon(const QColor& color, QStyle* style = nullptr, QStyleOption* options = nullptr)
	{
		QPixmap pixmap{ [](auto size) { return QSize(size, size); }
		((style ? style : QApplication::style())->pixelMetric(QStyle::PM_ListViewIconSize, options)) };
		pixmap.fill(color);
		return QIcon(pixmap);
	}

public:
	explicit ColorDataModel(QObject* parent = nullptr) : QStandardItemModel(parent)
	{
		auto colorNames{ QColor::colorNames() };
		setColumnCount(1);
		setRowCount(colorNames.size());

		for (decltype(colorNames.size()) row = 0; row < colorNames.size(); ++row)
		{
			QColor color{ colorNames[row] };
			setItem(row, 0, new QStandardItem{ colorNames[row] });
			item(row, 0)->setData(QVariant::fromValue<QColor>(color), ColorRole);
			item(row, 0)->setData(QVariant::fromValue<QIcon>(colorToIcon(color)), Qt::DecorationRole);
		}
	}
};

class ItemDelegate::ComboBox : public QComboBox
{
public:
	explicit ComboBox(QWidget* parent = nullptr) : QComboBox(parent) { setFrame(false); setMinimumSize(0, 0);  }

	void setCurrentData(QVariant currentData, int currentDataRole)
	{
		if (model())
		{
			auto indexes = model()->match(model()->index(0, 0), currentDataRole, currentData, 1, Qt::MatchExactly);
			if (indexes.size()) { setCurrentIndex(indexes.begin()->row()); }
		}
	}

protected:
	void enterEvent(QEvent* event) override
	{
		QComboBox::enterEvent(event);
		if (_firstEnter) { _firstEnter = false; showPopup(); }
	}

private:
	bool _firstEnter = true;
};

class ItemDelegate::TableView : public QTableView
{
public:
	explicit TableView(QWidget* parent = nullptr) : QTableView(parent)
	{
		setEditTriggers(QAbstractItemView::EditTrigger::AllEditTriggers);
		setFrameShape(QFrame::Shape::NoFrame);
		setItemDelegate(new ItemDelegate(this));
		setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
		setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
		setShowGrid(false);

		horizontalHeader()->setSectionsClickable(false);
		horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);

		verticalHeader()->hide();
	}

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

	void showEvent(QShowEvent* event) override
	{
		QTableView::showEvent(event);
		if (_firstShow)
		{
			_firstShow = false;
			QApplication::postEvent(viewport(), new QMouseEvent(
				QMouseEvent::Type::MouseButtonDblClick,
				viewport()->mapFromGlobal(QCursor::pos()),
				Qt::MouseButton::LeftButton,
				Qt::MouseButtons(Qt::MouseButton::NoButton),
				Qt::KeyboardModifiers(Qt::KeyboardModifier::NoModifier)));
			setCurrentIndex(QModelIndex());
		}
	}

private:
	bool _firstShow = true;
};

void ItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto dataType = index.data().userType();
	if ((dataType == qMetaTypeId<QVariantList>())
		|| (dataType == qMetaTypeId<EnumValue>())
		|| (dataType == qMetaTypeId<QColor>()))
	{
		auto editor = editorCache(option, index);
		setEditorData(editor, index);
		editor->setGeometry(option.rect);
		painter->save();
		painter->setTransform(QTransform{});
		editor->render(painter, option.rect.topLeft());
		painter->restore();
	}
	else
	{
		QStyledItemDelegate::paint(painter, option, index);
	}
}

QWidget* ItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto dataType = index.data().userType();
	if (dataType == qMetaTypeId<QVariantList>())
	{
		auto table = new TableView{ parent };
		table->setModel(new SubProxyModel(view(option)->model(), table));
		return table;
	} else
	if (dataType == qMetaTypeId<EnumValue>())
	{
		auto combo = new ComboBox{parent};
		combo->setModel(new EnumDataModel(index.data().value<EnumValue>().enumer(), combo));
		return combo;
	} else
	if (dataType == qMetaTypeId<QColor>())
	{
		auto combo = new ComboBox{parent};
		combo->setModel(new ColorDataModel(combo));
		return combo;
	} else
	if (dataType == qMetaTypeId<qulonglong>())
	{
		auto spin = new QSpinBox(parent);
		spin->setFrame(false);
		return spin;
	} else
	if (dataType == qMetaTypeId<float>())
	{
		auto spin = new QDoubleSpinBox(parent);
		spin->setFrame(false);
		return spin;
	}
	else
	{
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void ItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	auto data = index.data();
	auto dataType = data.userType();

	if (dataType == qMetaTypeId<QVariantList>())
	{
		auto table = static_cast<TableView*>(editor);
		static_cast<SubProxyModel*>(table->model())->setRoot(index);
		//table->resizeRowsToContents();
	} else
	if (dataType == qMetaTypeId<EnumValue>())
	{
		static_cast<ComboBox*>(editor)->setCurrentData(data, EnumDataModel::EnumValueRole);
	} else
	if (dataType == qMetaTypeId<QColor>())
	{
		static_cast<ComboBox*>(editor)->setCurrentData(data, ColorDataModel::ColorRole);
	}
	else
	{
		if (index.data(ValidatorRole).isValid())
		{
			auto validator = index.data(ValidatorRole).toList();
			if ((validator[Type].toInt() == IntRange)
				&& qobject_cast<QSpinBox*>(editor))
			{
				auto spinBox = qobject_cast<QSpinBox*>(editor);
				auto intValidator = validator[Constraint].toList();
				spinBox->setMinimum(intValidator[IntRangeMin].toInt());
				spinBox->setMaximum(intValidator[IntRangeMax].toInt());
			} else
			if ((validator[Type].toInt() == DoubleRange)
				&& qobject_cast<QDoubleSpinBox*>(editor))
			{
				auto spinBox = qobject_cast<QDoubleSpinBox*>(editor);
				auto dblValidator = validator[Constraint].toList();
				spinBox->setMinimum(dblValidator[DoubleRangeMin].toDouble());
				spinBox->setMaximum(dblValidator[DoubleRangeMax].toDouble());
				spinBox->setDecimals(dblValidator[DoubleRangeDecimals].toInt());
				spinBox->setSingleStep(1.0 / (spinBox->decimals() ? 10.0*spinBox->decimals() : 1));
			} else
			if ((validator[Type].toInt() == RegExp)
				&& qobject_cast<QLineEdit*>(editor))
			{
				qobject_cast<QLineEdit*>(editor)->setValidator(new QRegularExpressionValidator(
					QRegularExpression(validator[Constraint].toString()), editor));
			}
			else
			{
				assert(false);
			}
		}
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void ItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	auto dataType = index.data().userType();
	if (dataType == qMetaTypeId<QVariantList>())
	{
	} else
	if (dataType == qMetaTypeId<EnumValue>())
	{
		auto combo = static_cast<QComboBox*>(editor);
		model->setData(index, combo->itemData(combo->currentIndex(), EnumDataModel::EnumValueRole), Qt::DisplayRole);
	} else
	if (dataType == qMetaTypeId<QColor>())
	{
		auto combo = static_cast<QComboBox*>(editor);
		model->setData(index, combo->itemData(combo->currentIndex(), ColorDataModel::ColorRole), Qt::DisplayRole);
	} else
	if (dataType == qMetaTypeId<qulonglong>())
	{
		auto spinBox = static_cast<QSpinBox*>(editor);
		model->setData(index, (qulonglong)spinBox->value(), Qt::DisplayRole);
	} else
	if (dataType == qMetaTypeId<float>())
	{
		auto spinBox = static_cast<QDoubleSpinBox*>(editor);
		model->setData(index, (float)spinBox->value(), Qt::DisplayRole);
	}
	else
	{
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	std::unique_ptr<QWidget> editor{ createEditor(view(option)->viewport(), option, index) };
	setEditorData(editor.get(), index);
	return editor->sizeHint();
}

const QAbstractItemView* ItemDelegate::view(const QStyleOptionViewItem& option) const
{
	auto view = qobject_cast<const QAbstractItemView*>(option.widget);
	assert(view);
	return view;
}

QWidget* ItemDelegate::editorCache(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	auto dataType = index.data().userType();
	auto& editor = _editorCache[dataType];
	if (!editor)
	{
		editor = createEditor(view(option)->viewport(), option, index);
		editor->setAttribute(Qt::WA_DontShowOnScreen);
		editor->hide();
	}
	return editor;
}
