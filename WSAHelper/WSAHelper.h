#pragma once

#include <QtWidgets/QWidget>
#include <QProcess>
#include "ui_WSAHelper.h"
#include <QFileSystemModel>
#include <QStandardItemModel>

class WSAHelper : public QWidget
{
    Q_OBJECT

public:
    WSAHelper(QWidget *parent = nullptr);
    ~WSAHelper();

    void initWidget();
    void initRightMenu();
    void connectWSA();
    void transferFile(bool bPush, const QString strFrom, const QString strTo);
    QString getNowTime();
public slots:
    void slotShowRightMenu(const QPoint& pos);

private:
    Ui::WSAHelperClass ui;
    QComboBox* pLocalComboBox;
    QTableView* pLocalTableView;
    QFileSystemModel* pLocalModal;
    QProcess adbProcess;
    QTableView* pSysTableView;
    QStandardItemModel* pSysModal;
};
