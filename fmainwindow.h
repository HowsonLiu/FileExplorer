#ifndef FMAINWINDOW_H
#define FMAINWINDOW_H

#define COPY_OK 0
#define COPY_SKIP -1 //用户跳过
#define COPY_ERROR -2 //api出错

#include <QMainWindow>

class QPushButton;
class QLineEdit;
class QListView;
class QTreeView;
class MFileSystemModel;
class QAbstractItemView;
class FHistoryStack;
class FMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ViewType{Icon, Detail};
    Q_ENUM(ViewType)

public:
    FMainWindow(QWidget *parent = nullptr);
    ~FMainWindow();
    //下面这些文件处理函数可以分离出一个类，太少就算了
    ///Foucs: dst is destination file path (include 1.txt)
    int CopySingleFile(const QString& src, const QString& dst, bool move);
    ///Focus: dst is destination dir path (targetDir/)
    int CopyDir(const QString& src, const QString& dst, bool move);
    ///Focus: support muti object which must has the same parent folder
    bool openShellContextMenu(const QStringList& pathList);

public slots:
    void onSwitchViewType(ViewType type);
    void onPaste(const QStringList& srcs, const QString& dst, bool move);

private:
    QPushButton* m_backButton;
    QPushButton* m_forwardButton;
    QPushButton* m_upButton;
    QLineEdit* m_urlEdit;
    QPushButton* m_enterButton;
    QListView* m_listView;
    QTreeView* m_treeView;  //QListView不支持header,无法提供详细信息。根据API：This view does not display horizontal or vertical headers;
                            //to display a list of items with a horizontal header, use QTreeView instead.

    MFileSystemModel* m_fileModel;
    FHistoryStack* m_historyStack;
    QString m_currPath;
    ViewType m_currType;

private:
    void initWidget();
    bool inMyComputer() const;
    void jumpToMyComputer();
    bool jumpTo(const QString& path);
    QAbstractItemView *getCurrentView();

private slots:
    void onLineEditEnter();
    void onUpButtonClick();
    void onBackButtonEnable(bool);
    void onForwardButtonEnable(bool);
    void onBackButtonClick();
    void onForwardButtonClick();
    void onItemDoubleClick(const QModelIndex& index);
    void onCustomContextMenuRequested(const QPoint&);
    void onRefresh();
    void onRemove(const QString&);
    void onMore(const QStringList&);
};

#endif // FMAINWINDOW_H
