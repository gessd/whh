#include "VerifyThread.h"
#include <QDebug>
#include "df100dbusservice.h"

VerifyThread::VerifyThread()
{
}

void VerifyThread::run()
{
	pDf100Service->verifyTask();
}
