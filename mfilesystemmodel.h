#ifndef MFILESYSTEMMODEL_H
#define MFILESYSTEMMODEL_H

#include <QFileSystemModel>

class MFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit MFileSystemModel(QObject *parent = nullptr);
    Qt::DropActions supportedDropActions() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList& indexes) const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
    bool setData(const QModelIndex& index, const QVariant &value, int role) override;

signals:
    void sigDropEnd(const QStringList&, const QString&, bool);
};

#endif // MFILESYSTEMMODEL_H
