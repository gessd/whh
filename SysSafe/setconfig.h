#ifndef SETCONFIG_H
#define SETCONFIG_H
#include <QtCore/QSettings>

class SetConfig
{
public:
    //获取设置信息
    static QString getSetValue(QString set, QVariant defaultValue);
    //设置信息
    static void setSetValue(QString set, QVariant value);
private:
    SetConfig();
};

#endif // SETCONFIG_H
