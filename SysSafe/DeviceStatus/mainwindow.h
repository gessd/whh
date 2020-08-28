#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QLocalServer>
#include <QLocalSocket>
#include "dfcx_common.h"
#include "dfcx_drv.h"

namespace Ui {
class MainWindow;
}

#define _LocasServerName_ "syssafedevicecheck"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool startCheckDevice();
private slots:
    //定时连接设备
    void onTimeOut();
    //显示窗口
    void onShowWindow();
    //关闭窗口
    void onCloseWindow();
    void onNewConnection();
    void onClientUpdateStatus();
    void onClientReadMessage();
private:
    Ui::MainWindow *ui;
    bool m_bInit;
    QTimer* m_pTimerDevice;
    //系统通知使用
    QSystemTrayIcon *m_pTrayIcon;
    //自定义菜单
    QMenu *m_pTrayIconMenu;
    QAction *m_pQuitAction;
    QAction *m_pShowAction;
    QLocalServer* m_pLocalServer;
    QLocalSocket* m_pLocalClient;
};

#endif // MAINWINDOW_H
