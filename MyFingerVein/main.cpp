#include "mainwindow.h"
#include <QApplication>
#include "servicemanager.h"
#include "messagedialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ServiceManager *sm = ServiceManager::instance();

    if(!sm->serviceExists())
    {
        MessageDialog msgDialog(MessageDialog::Error,
                                QObject::tr("Fatal Error"),
                                QObject::tr("the biometric-authentication service was not started"));
        msgDialog.exec();
        exit(EXIT_FAILURE);
    }

    if(!sm->apiCompatible())
    {
        MessageDialog msgDialog(MessageDialog::Error,
                                QObject::tr("Fatal Error"),
                                QObject::tr("API version is not compatible"));
        msgDialog.exec();
        exit(EXIT_FAILURE);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
