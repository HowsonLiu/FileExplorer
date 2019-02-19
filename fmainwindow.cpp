#include "fmainwindow.h"
#include "mfilesystemmodel.h"
#include "fhistorystack.h"
#include <QPushButton>
#include <QLineEdit>
#include <QListView>
#include <QTreeView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

const QString g_dirNotExistMsg = QObject::tr("Windows can't find '%1'. Check the spelling and try again.");

FMainWindow::FMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initWidget();

    connect(m_enterButton, &QPushButton::clicked,
            this, &FMainWindow::onLineEditEnter);
    connect(m_urlEdit, &QLineEdit::returnPressed,
            this, &FMainWindow::onLineEditEnter);
    connect(m_listView, &QListView::doubleClicked,
            this, &FMainWindow::onItemDoubleClick);
    connect(m_treeView, &QTreeView::doubleClicked,
            this, &FMainWindow::onItemDoubleClick);
    connect(m_upButton, &QPushButton::clicked,
            this, &FMainWindow::onUpButtonClick);
    connect(m_historyStack, &FHistoryStack::sigBackEnable,
            this, &FMainWindow::onBackButtonEnable);
    connect(m_historyStack, &FHistoryStack::sigForwardEnable,
            this, &FMainWindow::onForwardButtonEnable);
    connect(m_backButton, &QPushButton::clicked,
            this, &FMainWindow::onBackButtonClick);
    connect(m_forwardButton, &QPushButton::clicked,
            this, &FMainWindow::onForwardButtonClick);

    onSwitchViewType(Icon);
    jumpToMyComputer();
    m_historyStack->push(m_fileModel->myComputer().toString());
}

FMainWindow::~FMainWindow()
{

}

void FMainWindow::onSwitchViewType(FMainWindow::ViewType type)
{
    switch (type) {
    case Icon:
        m_treeView->hide();
        m_listView->show();
        break;
    case Detail:
        m_listView->hide();
        m_treeView->show();
        break;
    }
    m_currType = type;
}

void FMainWindow::initWidget()
{
    //layout
    m_backButton = new QPushButton(this);
    m_forwardButton = new QPushButton(this);
    m_upButton = new QPushButton(this);
    m_urlEdit = new QLineEdit(this);
    m_enterButton = new QPushButton(this);
    m_listView = new QListView(this);
    m_treeView = new QTreeView(this);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addWidget(m_backButton);
    topLayout->addWidget(m_forwardButton);
    topLayout->addWidget(m_upButton);
    topLayout->addWidget(m_urlEdit);
    topLayout->addWidget(m_enterButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(topLayout);
    layout->addWidget(m_listView);
    layout->addWidget(m_treeView);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    //property
    m_fileModel = new MFileSystemModel(this);
    m_historyStack = new FHistoryStack(this);

    //history
    m_backButton->setEnabled(false);
    m_forwardButton->setEnabled(false);

    //view
    m_listView->setModel(m_fileModel);
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setIconSize(QSize(80,80));
    m_listView->setGridSize(QSize(100,100));
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setMovement(QListView::Snap);//网格
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);//自定义右键菜单
    m_listView->setAcceptDrops(true);
    m_listView->setDragEnabled(true);
    m_listView->setDropIndicatorShown(true);
    m_listView->setDragDropMode(QAbstractItemView::DragDrop);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection); //多选
    m_listView->setEditTriggers(QAbstractItemView::SelectedClicked);    //单击改名

    m_treeView->setModel(m_fileModel);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setRootIsDecorated(false);  //隐藏折叠三角形
    m_treeView->setItemsExpandable(false);  //不可展开
    m_treeView->setAcceptDrops(true);
    m_treeView->setDragEnabled(true);
    m_treeView->setDropIndicatorShown(true);
    m_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setEditTriggers(QAbstractItemView::SelectedClicked);
}

bool FMainWindow::inMyComputer() const
{
    return m_currPath.compare(m_fileModel->myComputer().toString()) == 0;
}

void FMainWindow::jumpToMyComputer()
{
    m_currPath = m_fileModel->myComputer().toString();//“此电脑”并不是实际的路径，是windows独有的。
    m_urlEdit->setText(m_currPath);
    QModelIndex rootIndex = m_fileModel->setRootPath(m_currPath);   //奇怪的是，通过这种方法取出来的modelindex是无效的
    getCurrentView()->setRootIndex(rootIndex);                      //然而无效的modelindex能让view跳转到“此电脑”的页面
}

bool FMainWindow::jumpTo(const QString &path)
{
    if(path.compare(m_fileModel->myComputer().toString()) == 0){
        jumpToMyComputer();
        return true;
    }
    QDir dir(path);
    if(!dir.exists()){
        QMessageBox::critical(this, "File Explorer", g_dirNotExistMsg.arg(dir.absolutePath()), QMessageBox::Ok);
        m_urlEdit->setText(m_currPath);
        return false;
    }
    m_currPath = dir.absolutePath();
    m_urlEdit->setText(m_currPath);
    QModelIndex index = m_fileModel->setRootPath(m_currPath);
    getCurrentView()->setRootIndex(index);
    return true;
}

QAbstractItemView *FMainWindow::getCurrentView()
{
    switch (m_currType) {
    case Icon:
        return m_listView;
    case Detail:
        return m_treeView;
    }
}

void FMainWindow::onLineEditEnter()
{
    QString oldPath = m_currPath;
    if(jumpTo(m_urlEdit->text()) && m_currPath.compare(oldPath) != 0)
        m_historyStack->push(m_currPath);   //不是刷新才记录到历史中
}

void FMainWindow::onUpButtonClick()
{
    if(inMyComputer()) return;
    QDir dir(m_currPath);
    if(dir.cdUp()){
        jumpTo(dir.absolutePath());
        m_historyStack->push(dir.absolutePath());
    }
    else{
        jumpToMyComputer();
        m_historyStack->push(m_fileModel->myComputer().toString());
    }
}

void FMainWindow::onBackButtonEnable(bool b)
{
    m_backButton->setEnabled(b);
}

void FMainWindow::onForwardButtonEnable(bool b)
{
    m_forwardButton->setEnabled(b);
}

void FMainWindow::onBackButtonClick()
{
    QString backPath = m_historyStack->backPath();
    if(backPath.isEmpty()) return;
    if(jumpTo(backPath)) m_historyStack->back();
}

void FMainWindow::onForwardButtonClick()
{
    QString forwardPath = m_historyStack->forwardPath();
    if(forwardPath.isEmpty()) return;
    if(jumpTo(forwardPath)) m_historyStack->forward();
}

void FMainWindow::onItemDoubleClick(const QModelIndex& index)
{
    if(!index.isValid()) return;
    QFileInfo fileInfo = m_fileModel->fileInfo(index);
    if(fileInfo.isDir()){
        jumpTo(fileInfo.absoluteFilePath());
        m_historyStack->push(fileInfo.absoluteFilePath());
    }
    else{
        //open file
    }
}
