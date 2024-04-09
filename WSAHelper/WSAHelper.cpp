#include "WSAHelper.h"
#include <QDebug>
#include <QProcess>
#include <QLabel>
#include <QStandardPaths>
#include <QComboBox>
#include <QPushButton>
#include <QDir>
#include <QTreeView>
#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <QMenu>
#include <QTime>


WSAHelper::WSAHelper(QWidget *parent)
    : QWidget(parent)
{
    initWidget();
    connectWSA();
}

void WSAHelper::initWidget() 
{
    ui.setupUi(this);

    //comboBox
    pLocalComboBox = ui.comboBox;
    if (pLocalComboBox == nullptr) 
    {
        return;
    }
    QFileInfoList drives = QDir::drives();
    for (const QFileInfo& drive : drives) 
    {
        pLocalComboBox->addItem(drive.absoluteFilePath());
    }
    if (drives.isEmpty())
    {
        return;
    }
    pLocalComboBox->setCurrentIndex(0);

    //localTableView
    pLocalTableView = ui.localTableView;
    if (pLocalTableView == nullptr) 
    {
        return;
    }
    pLocalModal = new QFileSystemModel();
    pLocalModal->setFilter(QDir::NoFilter);
    pLocalTableView->setModel(pLocalModal);
    QObject::connect(pLocalComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int index) {
        QString selectedDir = pLocalComboBox->itemText(index);
        pLocalModal->setRootPath(selectedDir);
        pLocalTableView->setRootIndex(pLocalModal->index(selectedDir));
        });
    QObject::connect(pLocalTableView->selectionModel(), &QItemSelectionModel::selectionChanged, [&](const QItemSelection& selected, const QItemSelection& deselected) {
        if (!selected.isEmpty()) {
            int selectedRow = selected.indexes().first().row();
            pLocalTableView->selectRow(selectedRow);
        }
        });
    QObject::connect(pLocalTableView, &QTableView::clicked, [&](const QModelIndex& index) {
        pLocalTableView->selectRow(index.row());
        });
    QObject::connect(pLocalTableView, &QTableView::doubleClicked, [&](const QModelIndex& index) {
        pLocalTableView->selectRow(index.row());
        QFileInfo fileInfo = pLocalModal->fileInfo(index);
        if (fileInfo.isDir()) {
            QString fullPath = fileInfo.absoluteFilePath();
            if (pLocalComboBox->findText(fullPath) == -1) 
            {
                int lastIndex = pLocalComboBox->count() - 1;
                pLocalComboBox->addItem(fullPath);
                pLocalComboBox->setCurrentText(fullPath);
            }
        }
        });

    QString selectedDir = pLocalComboBox->itemText(0);
    pLocalModal->setRootPath(selectedDir);
    pLocalTableView->setRootIndex(pLocalModal->index(selectedDir));

    // rightMenu
    initRightMenu();

    // sysTableView
    pSysModal = new QStandardItemModel();
    pSysTableView = ui.tableView;
    pSysTableView->setModel(pSysModal);
    QHeaderView* headerView = pSysTableView->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);
}

void WSAHelper::initRightMenu()
{
    pLocalTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(pLocalTableView, &QTableView::customContextMenuRequested, this, &WSAHelper::slotShowRightMenu);
}

void WSAHelper::slotShowRightMenu(const QPoint& pos)
{
    QModelIndex qIndex = pLocalTableView->indexAt(pos);
    if (qIndex.isValid())
    {
        QString filePath = pLocalModal->filePath(qIndex);
        QMenu* pRMenu = new QMenu(this);
        QAction* transferAction = new QAction(QString::fromLocal8Bit("´«Êä"), this);
        connect(transferAction, &QAction::triggered, this, [&]() {

            transferFile(true, filePath,"/sdcard");
            });
        pRMenu->addAction(transferAction);
        pRMenu->exec(QCursor::pos());
    }
}

void WSAHelper::transferFile(bool bPush,const  QString strFrom, const QString strTo)
{
    if (bPush)
    {
        QStringList arguments;
        arguments << "-s"<< "127.0.0.1:58526"<<"push" << strFrom<< strTo;
        adbProcess.start("adb", arguments);

        QByteArray strOutput;
        if (adbProcess.waitForFinished(-1)) {
            strOutput = adbProcess.readAllStandardOutput();
        }
        else {
            strOutput = adbProcess.readAllStandardError();
        }
        QList<QStandardItem*> newRow;
        newRow.append(new QStandardItem(QString::fromUtf8(strOutput)));
        newRow.append(new QStandardItem(getNowTime()));
        pSysModal->appendRow(newRow);
        pSysTableView->scrollToBottom();
    }
}

QString WSAHelper::getNowTime()
{
    QTime currentTime = QTime::currentTime();

    int hour = currentTime.hour();
    int minute = currentTime.minute();
    int second = currentTime.second();
    QString strTime = QString("%1:%2:%3").arg(hour, 2, 10, QLatin1Char('0'))
        .arg(minute, 2, 10, QLatin1Char('0'))
        .arg(second, 2, 10, QLatin1Char('0'));
    return strTime;
}


void WSAHelper::connectWSA()
{
    QStringList arguments;
    arguments << "connect" << "127.0.0.1:58526";
    adbProcess.start("adb", arguments);

    QByteArray strOutput;
    if (adbProcess.waitForFinished(-1)) {
        strOutput = adbProcess.readAllStandardOutput();
    }
    else {
        strOutput = adbProcess.readAllStandardError();
    }
    QLabel* pLabel = ui.label;
    if (pLabel) {
        pLabel->setText(strOutput);
    }
}

WSAHelper::~WSAHelper()
{}
