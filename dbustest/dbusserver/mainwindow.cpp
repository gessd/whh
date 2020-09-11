#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("server");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addDebugText(QString qstrText)
{
    ui->textBrowser->append(qstrText);
}

int MainWindow::maxValue()
{
    addDebugText("--function maxValue");
    int nMax = ui->lineEditMaxNum->text().trimmed().toInt();
    return nMax;
}

int MainWindow::minValue()
{
    addDebugText("--function minValue");
    int nMin = ui->lineEditMinNum->text().trimmed().toInt();
    return nMin;
}

QString MainWindow::setName(QString name)
{
    addDebugText("--function setName");
    ui->lineEditName->setText(name);
    for (int i = 1; i < 5; i++){
        QString qstrTime = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        emit signals_process_value(i,name,qstrTime);
        QThread::sleep(2);
    }
    return name;
}
