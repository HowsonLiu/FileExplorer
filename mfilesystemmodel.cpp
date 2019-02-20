#include "mfilesystemmodel.h"
#include <QMimeData>
#include <QUrl>
#include <QDebug>

MFileSystemModel::MFileSystemModel(QObject *parent) : QFileSystemModel(parent)
{

}

Qt::DropActions MFileSystemModel::supportedDropActions() const
{
    Qt::DropActions ori = QFileSystemModel::supportedDropActions();
    //qDebug() << ori << endl;
    return ori;
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
    QStringList ori = QFileSystemModel::mimeTypes();
    //qDebug() << ori << endl;
    return ori;
}

QMimeData *MFileSystemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData* ori = QFileSystemModel::mimeData(indexes);
    qDebug() << ori << endl;
    return ori;
}

bool MFileSystemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    qDebug() << data->urls() << action << row << column << fileInfo(parent).absoluteFilePath() << endl;
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
    qDebug() << index << value << role << endl;
    return QFileSystemModel::setData(index, value, role);
}
