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

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addDebugText(QString qstrText);
    void dbusfunc1();
    void dbusfunc2();
public slots:
    void onProcessValue(int index, QString name, QString qstrTime);
private slots:
    void on_btnConnect_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
