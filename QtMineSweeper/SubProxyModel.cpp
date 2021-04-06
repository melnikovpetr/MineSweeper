#include "SubProxyModel.h"

#include <cassert>

SubProxyModel::SubProxyModel(QAbstractItemModel* model, QObject* parent) : QIdentityProxyModel(parent), _root()
{
	assert(model);
	setSourceModel(model);
}

QVariant SubProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (_root.isValid() && (orientation == Qt::Orientation::Horizontal))
	{
		if (_root.data(role).userType() == qMetaTypeId<QVariantList>())
		{
			auto data = _root.data(role).toList();
			return section < data.count() ? data[section] : QVariant();
		}
		else
		{
			return _root.data(role);
		}
	}
	else
	{
		return QIdentityProxyModel::headerData(section, orientation, role);
	}
}

QModelIndex SubProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
	assert(sourceIndex.isValid() ? sourceIndex.model() == sourceModel() : true);

	if (!sourceIndex.isValid() || (sourceIndex == _root))
	{
		return QModelIndex();
	}
	else
	{
		auto idx = sourceIndex.parent();
		while (idx.isValid() && (_root != idx)) idx = idx.parent();

		if (_root == idx) return QIdentityProxyModel::mapFromSource(sourceIndex);
		else return QModelIndex();
	}
}

QModelIndex SubProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
	assert(proxyIndex.isValid() ? proxyIndex.model() == this : true);

	if (!proxyIndex.isValid()) return _root;
	else return QIdentityProxyModel::mapToSource(proxyIndex);
}

void SubProxyModel::setSourceModel(QAbstractItemModel* newSourceModel)
{
	_root = QModelIndex();
	QIdentityProxyModel::setSourceModel(newSourceModel);
}

void SubProxyModel::setRoot(const QModelIndex& sourceIndex)
{
	assert(sourceIndex.isValid() ? sourceIndex.model() == sourceModel() : true);

	beginResetModel();
	_root = sourceIndex;
	endResetModel();
}
