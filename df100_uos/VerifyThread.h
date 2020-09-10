#ifndef _VERIFY_THREAD_H_7AHD7_
#define _VERIFY_THREAD_H_7AHD7_

#include <QThread>

class Df100DbusService;

class VerifyThread : public QThread
{
public:
	VerifyThread();

	void run();

	QString m_finger;

	Df100DbusService *pDf100Service;
};

#endif

