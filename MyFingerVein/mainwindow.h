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

    void getDeviceInfo();
    int bioTypeToIndex(int type);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    /* 用于和远端 DBus 对象交互的代理接口 */
    QDBusInterface *serviceInterface;
    int deviceCount;
    QMap<int, QList<DeviceInfo *>> deviceInfosMap;
};

#endif // MAINWINDOW_H
