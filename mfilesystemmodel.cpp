#include "mfilesystemmodel.h"
#include <QMimeData>
#include <QUrl>

MFileSystemModel::MFileSystemModel(QObject *parent) : QFileSystemModel(parent)
{

}

Qt::DropActions MFileSystemModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction; // | Qt::LinkAction
}

Qt::DropActions MFileSystemModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::MoveAction; // | Qt::LinkAction
}

Qt::ItemFlags MFileSystemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QFileSystemModel::flags(index);
    if(index.isValid()){
        flag = flag | Qt::ItemIsEditable;
        if(fileInfo(index).isDir())
            flag = flag | Qt::ItemIsDropEnabled;
    }
    //qDebug() << flag << endl;
    return flag;
}

QStringList MFileSystemModel::mimeTypes() const
{
    return QFileSystemModel::mimeTypes();
}

QMimeData *MFileSystemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* ori = QFileSystemModel::mimeData(indexes);
    return ori;
}

bool MFileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if(action == Qt::IgnoreAction) return false;
    if(!data->hasUrls()) return false;
    QList<QUrl> urls = data->urls();
    QStringList pathList;
    for(QUrl url : urls){
        if(!url.isLocalFile()) continue;
        pathList << url.toLocalFile();
    }
    if(pathList.empty()) return false;
    emit sigDropEnd(pathList, fileInfo(parent).absoluteFilePath(), action == Qt::MoveAction);
    return true;
}

bool MFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return QFileSystemModel::setData(index, value, role);
}
