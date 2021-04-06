#ifndef SUB_PROXY_MODEL_H
#define SUB_PROXY_MODEL_H

#include <QtCore/QIdentityProxyModel>

class SubProxyModel : public QIdentityProxyModel
{
public:
	SubProxyModel(QAbstractItemModel* model, QObject* parent = nullptr);

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
	QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
	void setSourceModel(QAbstractItemModel* newSourceModel) override;
	void setRoot(const QModelIndex& sourceIndex);

private:
	QModelIndex _root;
};

#endif // SUB_PROXY_MODEL_H