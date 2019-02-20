#include "rightclickmenu.h"
#include <QMimeData>
#include <QUrl>
#include <QApplication>
#include <QClipboard>

RightClickMenu::RightClickMenu(QWidget *parent) : QMenu(parent)
{

}

ItemMenu *ItemMenu::instance()
{
    static ItemMenu* ins = new ItemMenu();
    return ins;
}

ItemMenu::ItemMenu(QWidget *parent) : RightClickMenu (parent)
{
    initWidget();

    connect(m_cutAct, &QAction::triggered,
            this, &ItemMenu::onCutAct);
    connect(m_copyAct, &QAction::triggered,
            this, &ItemMenu::onCopyAct);
    connect(m_removeAct, &QAction::triggered,
            this, &ItemMenu::onRemoveAct);
    connect(m_moreAct, &QAction::triggered,
            this, &ItemMenu::onMoreAct);
}

void ItemMenu::setItemList(const QStringList &list)
{
    m_selectedItemList = list;
}

void ItemMenu::onCutAct()
{
    QMimeData* mimeData = new QMimeData;
    QList<QUrl> urls;
    for(QString item : m_selectedItemList){
        urls.append(QUrl::fromLocalFile(item));
    }
    mimeData->setUrls(urls);
    int dropEffect = 2; // 2 for cut and 5 for copy
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << dropEffect;
    mimeData->setData("Preferred DropEffect", data);
    QApplication::clipboard()->setMimeData(mimeData);
}

void ItemMenu::onCopyAct()
{
    QMimeData* mimeData = new QMimeData;
    QList<QUrl> urls;
    for(QString item : m_selectedItemList){
        urls.append(QUrl::fromLocalFile(item));
    }
    mimeData->setUrls(urls);
    int dropEffect = 5; // 2 for cut and 5 for copy
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << dropEffect;
    mimeData->setData("Preferred DropEffect", data);
    QApplication::clipboard()->setMimeData(mimeData);
}

void ItemMenu::onRemoveAct()
{
    for(QString item : m_selectedItemList)
        emit sigRemove(item);
}

void ItemMenu::onMoreAct()
{
    emit sigMore(m_selectedItemList);
}

void ItemMenu::initWidget()
{
    m_cutAct = new QAction(this);
    m_copyAct = new QAction(this);
    m_removeAct = new QAction(this);
    m_moreAct = new QAction(this);

    m_cutAct->setText("Cut");
    m_copyAct->setText("Copy");
    m_removeAct->setText("Delete");
    m_moreAct->setText("More");

    addAction(m_cutAct);
    addAction(m_copyAct);
    addAction(m_removeAct);
    addAction(m_moreAct);
}


BlankMenu* BlankMenu::instance()
{
    static BlankMenu* ins = new BlankMenu();
    return ins;
}

BlankMenu::BlankMenu(QWidget *parent): RightClickMenu (parent)
{
    initWidget();

    connect(m_viewIconAct, &QAction::triggered,
            this, &BlankMenu::onViewIconAct);
    connect(m_viewDetailAct, &QAction::triggered,
            this, &BlankMenu::onViewDetailAct);
    connect(m_refreshAct, &QAction::triggered,
            this, &BlankMenu::sigRefresh);
    connect(m_pasteAct, &QAction::triggered,
            this, &BlankMenu::onPasteAct);
}

void BlankMenu::SetPath(const QString &info)
{
    m_curPath = info;
}

void BlankMenu::onViewIconAct()
{
    emit sigSwitchViewType(FMainWindow::Icon);
}

void BlankMenu::onViewDetailAct()
{
    emit sigSwitchViewType(FMainWindow::Detail);
}

void BlankMenu::onPasteAct()
{
    const QMimeData* mimeData = QApplication::clipboard()->mimeData();
    if(!mimeData->hasUrls()) return;
    if(!mimeData->hasFormat("Preferred DropEffect")) return;
    int dropEffect = 0;
    QByteArray data = mimeData->data("Preferred DropEffect");
    QDataStream stream(&data, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> dropEffect;
    QList<QUrl> urls = mimeData->urls();
    if(dropEffect != 2 && dropEffect != 5) return;
    QStringList srcs;
    for(QUrl url : urls){
        if(!url.isLocalFile()) continue;
        srcs << url.toLocalFile();
    }
    emit sigPaste(srcs, m_curPath, dropEffect == 2);
    if(dropEffect == 2) QApplication::clipboard()->clear();
}

void BlankMenu::initWidget()
{
    m_viewMenu = new QMenu(this);
    m_viewIconAct = new QAction(this);
    m_viewDetailAct = new QAction(this);
    m_pasteAct = new QAction(this);
    m_refreshAct = new QAction(this);
    m_viewMenu->setTitle("View");
    m_viewIconAct->setText("Icon");
    m_viewDetailAct->setText("Detail");
    m_pasteAct->setText("Paste");
    m_refreshAct->setText("Refresh");
    m_viewMenu->addAction(m_viewIconAct);
    m_viewMenu->addAction(m_viewDetailAct);
    addMenu(m_viewMenu);
    addAction(m_refreshAct);
    addAction(m_pasteAct);
}
