#include "setconfig.h"
#include "styledefine.h"
#include <QtWidgets/QApplication>
#include <QtCore/QFile>

QString SetConfig::getSetValue(QString set, QVariant defaultValue)
{
    QString qstrInfFile = QApplication::applicationDirPath() + +"/"+_SetFile;
    if(false == QFile::exists(qstrInfFile)){
        QFile file(qstrInfFile);
        if(file.open(QIODevice::ReadWrite | QIODevice::Text |QIODevice::Append)) file.close();
    }
    QSettings setting(qstrInfFile, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    if(false == setting.contains(set)){
        setting.setValue(set, defaultValue);
    }
    return setting.value(set, defaultValue).toString();
}

void SetConfig::setSetValue(QString set, QVariant value)
{
    QString qstrInfFile = QApplication::applicationDirPath() + +"/"+_SetFile;
    if(false == QFile::exists(qstrInfFile)){
        QFile file(qstrInfFile);
        if(file.open(QIODevice::ReadWrite | QIODevice::Text |QIODevice::Append)) file.close();
    }
    QSettings setting(qstrInfFile, QSettings::IniFormat);
    setting.setIniCodec("UTF-8");
    setting.setValue(set, value);
}

SetConfig::SetConfig()
{

}
