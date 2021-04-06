#ifndef ITEM_DELEGATE_H
#define ITEM_DELEGATE_H

#include <QtWidgets/QStyledItemDelegate>

class ItemDelegate : public QStyledItemDelegate
{
public:
	class EnumDataModel;
	class ColorDataModel;

	class ComboBox;
	class TableView;

	enum DataRoles : int
	{
		ValidatorRole = Qt::UserRole,
	};

	enum ValidatorAttrs : int
	{
		Type,
		Constraint,
	};

	enum ValidatorTypes : int
	{
		IntRange,
		DoubleRange,
		RegExp,
	};

	enum IntRangeAttrs : int
	{
		IntRangeMin,
		IntRangeMax,
	};

	enum DoubleRangeAttrs : int
	{
		DoubleRangeMin,
		DoubleRangeMax,
		DoubleRangeDecimals,
	};

public:
	ItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

protected:
	const QAbstractItemView* view(const QStyleOptionViewItem& option) const;
	QWidget* editorCache(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
	mutable QHash<int, QWidget*> _editorCache;
};

#endif // ITEM_DELEGATE_H
