#ifndef MFILESYSTEMMODEL_H
#define MFILESYSTEMMODEL_H

#include <QFileSystemModel>

class MFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit MFileSystemModel(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MFILESYSTEMMODEL_H
