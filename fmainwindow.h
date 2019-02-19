#ifndef FMAINWINDOW_H
#define FMAINWINDOW_H

#include <QMainWindow>

class QPushButton;
class QLineEdit;
class QListView;
class QTreeView;
class MFileSystemModel;
class QAbstractItemView;
class FMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ViewType{Icon, Detail};
    Q_ENUM(ViewType)

public:
    FMainWindow(QWidget *parent = nullptr);
    ~FMainWindow();

public slots:
    void onSwitchViewType(ViewType type);

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
    void onItemDoubleClick(const QModelIndex& index);
};

#endif // FMAINWINDOW_H
