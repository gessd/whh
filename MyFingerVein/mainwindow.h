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
    //用户
    void on_pushButton_clicked();
    //设备
    void on_pushButton_2_clicked();
    //录入
    void on_pushButton_3_clicked();
private slots:
    void onUSBDeviceHotPlug(int int1, int int2, int int3);
    void enrollCallBack(const QDBusMessage &reply);
    void verifyCallBack(const QDBusMessage &reply);
    void searchCallBack(const QDBusMessage &reply);
    void StopOpsCallBack(const QDBusMessage &reply);
    void errorCallBack(const QDBusError &error);
    void errorCallback(QDBusError error);
    void showFeaturesCallback(QDBusMessage callbackReply);
    //验证
    void on_pushButton_4_clicked();
    //删除
    void on_pushButton_5_clicked();
    //查找
    void on_pushButton_6_clicked();
    //列表
    void on_pushButton_7_clicked();

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
