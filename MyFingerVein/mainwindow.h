#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "customtype.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addDebugText(QString qstrText);
    void getDeviceInfo();
    int bioTypeToIndex(int type);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
private slots:
    void onUSBDeviceHotPlug(int, int, int);
    void enrollCallBack(const QDBusMessage &reply);
    void verifyCallBack(const QDBusMessage &reply);
    void searchCallBack(const QDBusMessage &reply);
    void StopOpsCallBack(const QDBusMessage &reply);
    void errorCallBack(const QDBusError &error);
private:
    QString handleErrorResult(int error);
private:
    Ui::MainWindow *ui;
    /* 用于和远端 DBus 对象交互的代理接口 */
    QDBusInterface *serviceInterface;
    int deviceCount;
    //设备类型与设备信息
    QMap<int, QList<DeviceInfo *>> deviceInfosMap;

    //测试用 指静脉设备信息
    DeviceInfo* m_pFingerVeinDeviceINfo;
};

#endif // MAINWINDOW_H
