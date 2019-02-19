#ifndef RIGHTCLICKMENU_H
#define RIGHTCLICKMENU_H

#include <QMenu>
#include "fmainwindow.h"

class RightClickMenu : public QMenu
{
    Q_OBJECT
public:
    explicit RightClickMenu(QWidget *parent = nullptr);
};



class ItemMenu : public RightClickMenu
{
    Q_OBJECT
private:
    explicit ItemMenu(QWidget *parent = nullptr);

public:
    static ItemMenu* instance();
    //must be called before exec
    void setItemList(const QStringList& list);

signals:
    void sigRemove(const QString&);

private:
    QAction* m_cutAct;
    QAction* m_copyAct;
    QAction* m_removeAct;
    QAction* m_moreAct;

    QStringList m_selectedItemList;

private slots:
    void onCutAct();
    void onCopyAct();
    void onRemoveAct();
    void onMoreAct();

private:
    void initWidget();
};
#define ITEMMENUINSTANCE ItemMenu::instance()


class BlankMenu : public RightClickMenu
{
    Q_OBJECT
private:
    explicit BlankMenu(QWidget *parent = nullptr);

signals:
    void sigSwitchViewType(FMainWindow::ViewType);
    void sigRefresh();
    void sigPaste(const QStringList&, const QString&, bool);

public:
    static BlankMenu* instance();
    //must be called before exec
    void SetPath(const QString& info);

private:
    QMenu* m_viewMenu;
    QAction* m_viewIconAct;
    QAction* m_viewDetailAct;
    QAction* m_pasteAct;
    QAction* m_refreshAct;

    QString m_curPath;

private slots:
    void onViewIconAct();
    void onViewDetailAct();
    void onPasteAct();

private:
    void initWidget();
};
#define BLANKMENUINSTANCE BlankMenu::instance()

#endif // RIGHTCLICKMENU_H
