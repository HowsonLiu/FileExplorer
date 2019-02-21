#include "fmainwindow.h"
#include "mfilesystemmodel.h"
#include "fhistorystack.h"
#include "rightclickmenu.h"
#include "freleaser.h"
#include <QPushButton>
#include <QLineEdit>
#include <QListView>
#include <QTreeView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <Windows.h>
#include <ShlObj_core.h>
#include <QDesktopServices>
#include <QUrl>
#include <vector>

const QString g_fileDirNotExistMsg = QObject::tr("Could not find this item.\n'%1'");
const QString g_enterDirNotExistMsg = QObject::tr("Windows can't find '%1'. Check the spelling and try again.");
const QString g_fileExistMsg = QObject::tr("The destination already has a file named '%1'");
const QString g_fileSameWithDirMsg = QObject::tr("There is already a folder with the same name as the file name you specified.\n'%1'\nSpecifiy a different name");
const QString g_dirSubDirMsg = QObject::tr("The destination folder is a subfolder of the source folder.\n'%1'");
const QString g_fileremoveMsg = QObject::tr("Could not remove this item.\n'%1'");

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

    //context menu
    connect(m_listView, &QWidget::customContextMenuRequested,
            this, &FMainWindow::onCustomContextMenuRequested);
    connect(m_treeView, &QWidget::customContextMenuRequested,
            this, &FMainWindow::onCustomContextMenuRequested);
    connect(BLANKMENUINSTANCE, &BlankMenu::sigSwitchViewType,
            this, &FMainWindow::onSwitchViewType);
    connect(BLANKMENUINSTANCE, &BlankMenu::sigRefresh,
            this, &FMainWindow::onRefresh);
    connect(BLANKMENUINSTANCE, &BlankMenu::sigPaste,
            this, &FMainWindow::onPaste);
    connect(ITEMMENUINSTANCE, &ItemMenu::sigRemove,
            this, &FMainWindow::onRemove);
    connect(ITEMMENUINSTANCE, &ItemMenu::sigMore,
            this, &FMainWindow::onMore);

    //drop
    connect(m_fileModel, &MFileSystemModel::sigDropEnd,
            this, &FMainWindow::onPaste);


    onSwitchViewType(Icon);
    jumpToMyComputer();
    m_historyStack->push(m_fileModel->myComputer().toString());
}

FMainWindow::~FMainWindow()
{

}

int FMainWindow::CopySingleFile(const QString &src, const QString &dst, bool move)
{
    QFileInfo srcFileinfo(src);
    if(!srcFileinfo.exists() || srcFileinfo.isDir()){
        //Windows这里有retry和cancel，我直接skip
        QMessageBox::critical(this, "Item Not Found", g_fileDirNotExistMsg.arg(srcFileinfo.absoluteFilePath()), "Skip");
        return COPY_SKIP;
    }
    if(src.compare(dst) == 0) return COPY_OK;   //这里window对同路径的复制做加后缀处理，我简单忽略
    QFileInfo dstFileInfo(dst);
    QFile dstFile(dst);
    if(dstFileInfo.exists()){
        if(dstFileInfo.isFile()){
            //Windows这里有Replace和Skip和Cancel,我做replace和skip处理
            if(QMessageBox::warning(this, "Replace or Skip Files", g_fileExistMsg.arg(dstFileInfo.absoluteFilePath()),
                                    "Replace", "Skip") == 0){
                if(!dstFile.remove()) {
                    return COPY_ERROR;
                }
            }
            else{
                return COPY_SKIP;
            }
        }
        else{
            //Windows这里有retry和cancel,我做skip跳过
            QMessageBox::critical(this, "Rename File", g_fileSameWithDirMsg.arg(dstFileInfo.absoluteFilePath()), "Skip");
            return COPY_SKIP;
        }
    }
    if(!QFile::copy(src, dst)) return COPY_ERROR;
    if(move) QFile::remove(src);
    return COPY_OK;
}

int FMainWindow::CopyDir(const QString &src, const QString &dst, bool move)
{
    QFileInfo srcFileinfo(src);
    if(!srcFileinfo.exists() || srcFileinfo.isFile()){
        QMessageBox::critical(this, "Item Not Found", g_fileDirNotExistMsg.arg(srcFileinfo.absoluteFilePath()), "Skip");
        return COPY_SKIP;
    }
    if(src.compare(dst) == 0) return COPY_OK;
    QDir srcDir(src), dstDir(dst);
    while(dstDir.cdUp()){
        if(dstDir == srcDir){
            //Windows这里有cancel和Skip
            QMessageBox::critical(this, "Subfolder", g_dirSubDirMsg.arg(dstDir.absolutePath()), "Skip");
            return COPY_SKIP;
        }
    }
    dstDir.setPath(dst);
    QFileInfo srcFileInfo(src);
    QFileInfo dstFileInfo(dst);
    if(srcFileInfo.isFile()) return COPY_ERROR;
    if(dstFileInfo.exists() && dstFileInfo.isFile()){
        QMessageBox::critical(this, "Rename File", g_fileSameWithDirMsg.arg(dstFileInfo.absoluteFilePath()), "Skip");
        return COPY_SKIP;
    }
    if(!dstDir.exists()){
        if(!dstDir.mkdir(dstDir.absolutePath()))
            return COPY_ERROR;
    }
    QFileInfoList srcFileInfoList = srcDir.entryInfoList();
    for(QFileInfo srcFileInfo : srcFileInfoList){
        if(srcFileInfo.fileName() == "." || srcFileInfo.fileName() == "..")
            continue;
        if(srcFileInfo.isDir()){
            if(CopyDir(srcFileInfo.filePath(), dstDir.filePath(srcFileInfo.fileName()), move) == COPY_ERROR)
                return COPY_ERROR;
        }
        else{
            QFileInfo dstFileInfo(dstDir, srcFileInfo.fileName());
            if(CopySingleFile(srcFileInfo.absoluteFilePath(), dstFileInfo.absoluteFilePath(), move) == COPY_ERROR)
                return COPY_ERROR;
        }
    }
    if(move && srcDir.isEmpty()) srcDir.rmpath(srcDir.absolutePath());
    return COPY_OK;
}

bool FMainWindow::openShellContextMenu(const QStringList& pathList)
{
    if(pathList.empty()) return false;
    std::vector<ITEMIDLIST*> idvec;
    std::vector<LPCITEMIDLIST> idChildvec;
    IShellFolder* ifolder = nullptr;
    for(QString path : pathList){
        std::wstring windowsPath = path.toStdWString();
        std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');
        ITEMIDLIST* id = nullptr;
        HRESULT res = SHParseDisplayName(windowsPath.c_str(), nullptr, &id, 0, nullptr);   //路径转PIDL
        if(!SUCCEEDED(res) || !id) continue;
        idvec.push_back(id);
        idChildvec.push_back(nullptr);
        res = SHBindToParent(id, IID_IShellFolder, (void**)&ifolder, &idChildvec.back());   //获取ishellfolder
        if(!SUCCEEDED(res) || !idChildvec.back())
            idChildvec.pop_back();
        else if(path.compare(pathList.back()) != 0 && ifolder){
            ifolder->Release();
            ifolder = nullptr;
        }
    }
    FItemIdListVectorReleaser vecReleaser(idvec);
    FComInterfaceReleaser ifolderReleaser (ifolder);
    if(ifolder == nullptr || idChildvec.empty()) return false;
    IContextMenu* iMenu = nullptr;
    HRESULT res = ifolder->GetUIObjectOf((HWND)this->winId(), (UINT)idChildvec.size(),
                                         (const ITEMIDLIST**)idChildvec.data(),     //获取右键UI按钮
                                         IID_IContextMenu, nullptr, (void**)&iMenu);//放到imenu中
    if(!SUCCEEDED(res) || !ifolder) return false;
    FComInterfaceReleaser menuReleaser(iMenu);
    HMENU hMenu = CreatePopupMenu();
    if(!hMenu) return false;
    if(SUCCEEDED(iMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))){    //默认选项
        int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD, QCursor::pos().x(),
                                    QCursor::pos().y(), (HWND)this->winId(), nullptr);
        if(iCmd > 0){   //执行菜单命令
            CMINVOKECOMMANDINFOEX info = { 0 };
            info.cbSize = sizeof(info);
            info.fMask = CMIC_MASK_UNICODE;
            info.hwnd = (HWND)this->winId();
            info.lpVerb = MAKEINTRESOURCEA(iCmd - 1);
            info.lpVerbW = MAKEINTRESOURCEW(iCmd - 1);
            info.nShow = SW_SHOWNORMAL;
            iMenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
        }
    }
    DestroyMenu(hMenu);
    return true;
}

void FMainWindow::onSwitchViewType(FMainWindow::ViewType type)
{
    if(m_currType != type){
        switch (type) {
        case Icon:
            m_treeView->hide();
            m_listView->setRootIndex(m_fileModel->index(m_currPath));
            m_listView->show();
            break;
        case Detail:
            m_listView->hide();
            m_treeView->setRootIndex(m_fileModel->index(m_currPath));
            m_treeView->show();
            break;
        }
        m_currType = type;
    }
}

void FMainWindow::onPaste(const QStringList &srcs, const QString &dst, bool move)
{
    for(QString src : srcs){
        QFileInfo srcFileInfo(src);
        QFileInfo dstFileInfo(QDir(dst), srcFileInfo.fileName());
        if(srcFileInfo.isFile()){
            CopySingleFile(src, dstFileInfo.absoluteFilePath(), move);
        }
        else{
            CopyDir(src, dstFileInfo.absoluteFilePath(), move);
        }
    }
    if(inNetwork()) onRefresh();    //由于性能的原因，网络文件的增删不会被Model感知，需要手动刷新
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
    m_fileModel = new MFileSystemModel();
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
    m_treeView->setSortingEnabled(true);    //自带排序
    m_treeView->sortByColumn(0, Qt::AscendingOrder);    //原本默认是名字降序，改为升序

    //UI
    m_backButton->setStyleSheet("QPushButton{border-image: url(:/icon/Resources/back-enable.png)}"
                            "QPushButton:disabled{border-image: url(:/icon/Resources/back-disable.png)}");
    m_forwardButton->setStyleSheet("QPushButton{border-image: url(:/icon/Resources/forward-enable.png)}"
                                   "QPushButton:disabled{border-image: url(:/icon/Resources/forward-disable.png)}");
    m_upButton->setStyleSheet("QPushButton{border-image: url(:/icon/Resources/up-enable.png)}"
                                   "QPushButton:disabled{border-image: url(:/icon/Resources/up-disable.png)}");
    m_enterButton->setStyleSheet("QPushButton{border-image: url(:/icon/Resources/enter.png)}");
    m_backButton->setFixedSize(QSize(23,23));
    m_forwardButton->setFixedSize(QSize(23,23));
    m_upButton->setFixedSize(QSize(23,23));
    m_enterButton->setFixedSize(QSize(23,23));
    resize(800, 500);
}

bool FMainWindow::inMyComputer() const
{
    return m_currPath.compare(m_fileModel->myComputer().toString()) == 0;
}

bool FMainWindow::inNetwork() const
{
    return m_currPath.startsWith("//") | m_currPath.startsWith("\\\\"); //简单的判断，日后更改
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
        QMessageBox::critical(this, "File Explorer", g_enterDirNotExistMsg.arg(dir.absolutePath()), QMessageBox::Ok);
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
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
}

void FMainWindow::onCustomContextMenuRequested(const QPoint& point)
{
    if(inMyComputer()) return;
    if(getCurrentView()->indexAt(point).isValid()){ //在非空白处右击
        //选中的全传过去，点击的排第一
        QStringList list;
        list.append(m_fileModel->filePath(getCurrentView()->indexAt(point)));
        QModelIndexList selectList = getCurrentView()->selectionModel()->selectedIndexes();
        for(auto index : selectList){
            if(!list.contains(m_fileModel->filePath(index)))
                list.append(m_fileModel->filePath(index));
        }
        ITEMMENUINSTANCE->setItemList(list);
        ITEMMENUINSTANCE->exec(QCursor::pos());
    }
    else{ //空白
        //传当前文件夹路径
        BLANKMENUINSTANCE->SetPath(m_currPath);
        BLANKMENUINSTANCE->exec(QCursor::pos());
    }
}

// QTBUG-46684
// QFileSystemModel只监控文件的新增与删除，对文件大小的变化不监控（操作系统所致）
// 在Qt-5.9.4中提供QT_FILESYSTEMMODEL_WATCH_FILES宏监控
// 在之前的版本中可以通过QFileSystemWatcher自行监控，但这种方法复杂，监控的数量有限制
// 因此最被大多数人接受的方法是替换QFileSystemModel以达到强制更新
void FMainWindow::onRefresh()
{
    delete m_fileModel;
    m_fileModel = new MFileSystemModel();
    m_listView->setModel(m_fileModel);
    m_treeView->setModel(m_fileModel);
    QModelIndex index = m_fileModel->setRootPath(m_currPath);
    m_listView->setRootIndex(index);
    m_treeView->setRootIndex(index);
}

void FMainWindow::onRemove(const QString& path)
{
    if(!m_fileModel->remove(m_fileModel->index(path)))
        QMessageBox::critical(this, "Remove Error",
                              g_fileremoveMsg.arg(path), QMessageBox::Ok);
    if(inNetwork()) onRefresh();//由于性能的原因，网络文件的增删不会被Model感知，需要手动刷新
}

void FMainWindow::onMore(const QStringList& pathList)
{
    openShellContextMenu(pathList);
}
