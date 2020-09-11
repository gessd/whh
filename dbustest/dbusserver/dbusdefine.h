#ifndef DBUSDEFINE_H
#define DBUSDEFINE_H

#include <QDBusConnection>
#include <QDBusError>
#include <QThread>
#include <QDateTime>
#include <QDebug>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>

#define _DBUS_ "D-Bus Interface"
#define _DBUS_Service_   "com.scorpio.test"
#define _DBUS_Paht_      "/com/test/window"
#define _DBUS_Interface_ "com.scorpio.test.value"

#define _Interface_Name_    "setName"
#endif // DBUSDEFINE_H
