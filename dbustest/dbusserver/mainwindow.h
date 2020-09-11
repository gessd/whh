#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbusdefine.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    //定义Interface名称为
    Q_CLASSINFO("D-Bus Interface", _DBUS_Interface_)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addDebugText(QString qstrText);

public slots:
    int maxValue();
    int minValue();
    QString setName(QString name);
signals:
    void signals_process_value(int,QString,QString);
private:

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
