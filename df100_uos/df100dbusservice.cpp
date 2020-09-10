/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "df100dbusservice.h"
//#include "mainwindow.h"

#include <QDebug>
#include <QJsonDocument>
#include <QFile>
#include <dlfcn.h>
#include <unistd.h>
#include "userservice.h"


const QString FingerModePath = "/usr/share/deepin-authentication/modes/VFingerPrint/";

QStringList readFinger(QString id);
void addFinger(QString id, QString finger, QString temp, int null_id);
void deleteFinger(QString id, QString finger);

static void* GetFVAPI(void *pSoHandle, const char* sApiName)
{
	char* sError = NULL;
	void* pApi = NULL;

	if(pSoHandle == NULL || sApiName == NULL) return NULL;

	pApi = dlsym(pSoHandle, sApiName);
	sError = dlerror();
	if(sError != NULL) 
	{
		qDebug() << "API: " << sApiName << "ERROR:" <<  sError;
		return NULL;
	}
	return pApi;
}

Df100DbusService::Df100DbusService(QObject *parent)
    : QDBusAbstractAdaptor(parent)
//    , _mw(parent)
{
	m_devHd = 0;
	m_veinHd = 0;
	memset(m_captureVein, 0, sizeof(m_captureVein));

	setAutoRelaySignals(true);
	QDir dir(FingerModePath);
	if (!dir.exists()) {
		if (!dir.mkpath(FingerModePath)) {
			qDebug() << "mkdir error:" << dir;
		}
	}

	connect(this, SIGNAL(siSendMessage(int, int, QString)), this, SLOT(sSendMessage(int,int,QString)));

	char* sSoFile = "/usr/lib/libXGComApi.so";

	memset(&m_FVAPI, 0, sizeof(FVAPI_t));
	char* sError = NULL;
	void *pSoHandle = NULL;
	pSoHandle = dlopen(sSoFile, RTLD_LAZY);
	if(pSoHandle == NULL)
	{
		sError = dlerror();
		qDebug() << "dlopen error:" << sError;
		//Log TODO
	} else {
		m_FVAPI.FV_SetDebug = (long(*)(long))GetFVAPI(pSoHandle, "FV_SetDebug");
		m_FVAPI.FV_ConnectDev = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_ConnectDev");
		m_FVAPI.FV_CloseDev = (long(*)(long))GetFVAPI(pSoHandle, "FV_CloseDev");
		m_FVAPI.FV_SendCmdPacket = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SendCmdPacket");
		m_FVAPI.FV_RecvCmdPacket = (long(*)(long,LPCTSTR,long))GetFVAPI(pSoHandle, "FV_RecvCmdPacket");
		m_FVAPI.FV_RecvUartData = (long(*)(long,LPCTSTR,long,long))GetFVAPI(pSoHandle, "FV_RecvUartData");
		m_FVAPI.FV_GetDevParam = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetDevParam");
		m_FVAPI.FV_SetDevParam = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SetDevParam");
		m_FVAPI.FV_PlayDevSound = (long(*)(long,long))GetFVAPI(pSoHandle, "FV_PlayDevSound");
		m_FVAPI.FV_CheckFinger = (long(*)(long))GetFVAPI(pSoHandle, "FV_CheckFinger");
		m_FVAPI.FV_GetVeinChara = (long(*)(long,LPCTSTR,long))GetFVAPI(pSoHandle, "FV_GetVeinChara");
		m_FVAPI.FV_ReadDevTemp = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_ReadDevTemp");
		m_FVAPI.FV_WriteDevTemp = (long(*)(long,long,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_WriteDevTemp");
		m_FVAPI.FV_DeleteDevTemp = (long(*)(long,long))GetFVAPI(pSoHandle, "FV_DeleteDevTemp");
		m_FVAPI.FV_GetDevDebugInfo = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetDevDebugInfo");
		m_FVAPI.FV_Upgrade = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_Upgrade");
		m_FVAPI.FV_GetImgFormData = (long(*)(LPCTSTR,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetImgFormData");
		m_FVAPI.FV_GetImgFormDev = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetImgFormDev");
		m_FVAPI.FV_GetCharaFromImg = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetCharaFromImg");
		m_FVAPI.FV_ReadDevLog = (long(*)(long,LPCTSTR,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_ReadDevLog");
		m_FVAPI.FV_WriteDevUserInfo = (long(*)(long,long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_WriteDevUserInfo");
		m_FVAPI.FV_ReadDevUserInfo = (long(*)(long,long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_ReadDevUserInfo");

		m_FVAPI.FV_CreateVeinLib = (long(*)(long))GetFVAPI(pSoHandle, "FV_CreateVeinLib");
		m_FVAPI.FV_DestroyVeinLib = (long(*)(long))GetFVAPI(pSoHandle, "FV_DestroyVeinLib");
		m_FVAPI.FV_GetNullId = (long(*)(long))GetFVAPI(pSoHandle, "FV_GetNullID");
		m_FVAPI.FV_ImportVeinTemp = (long(*)(long,long,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_ImportVeinTemp");
		m_FVAPI.FV_ExportVeinTemp = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_ExportVeinTemp");
		m_FVAPI.FV_CleanVeinTemp = (long(*)(long,long))GetFVAPI(pSoHandle, "FV_CleanVeinTemp");
		m_FVAPI.FV_SetUserInfo = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SetUserInfo");
		m_FVAPI.FV_GetUserInfo = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetUserInfo");
		m_FVAPI.FV_GetTempUserInfo = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_GetTempUserInfo");
		m_FVAPI.FV_SearchUser = (long(*)(long,LPCTSTR,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SearchUser");
		m_FVAPI.FV_VerifyUser = (long(*)(LPCTSTR,LPCTSTR,long,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_VerifyUser");
		m_FVAPI.FV_CreateVeinTemp = (long(*)(LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_CreateVeinTemp");
		m_FVAPI.FV_CharaMatch = (long(*)(LPCTSTR,LPCTSTR,long))GetFVAPI(pSoHandle, "FV_CharaMatch");

		m_FVAPI.FV_NetPackParse = (long(*)(LPCTSTR,long,LPCTSTR,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_NetPackParse");
		m_FVAPI.FV_AsciiToHex = (long(*)(LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_AsciiToHex");
		m_FVAPI.FV_HexToAscii = (long(*)(LPCTSTR,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_HexToAscii");

		m_FVAPI.FV_SocketServerInit = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SocketServerInit");
		m_FVAPI.FV_SocketAccept = (long(*)(long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SocketAccept");
		m_FVAPI.FV_SocketRecvPack = (long(*)(long,LPCTSTR,LPCTSTR,LPCTSTR))GetFVAPI(pSoHandle, "FV_SocketRecvPack");
		m_FVAPI.FV_SocketSendPack = (long(*)(long,long,LPCTSTR))GetFVAPI(pSoHandle, "FV_SocketSendPack");
		m_FVAPI.FV_SocketClose = (long(*)(long))GetFVAPI(pSoHandle, "FV_SocketClose");

		m_veinHd = m_FVAPI.FV_CreateVeinLib(1000);
		if (m_veinHd <= 0) {
			qDebug() << "创建算法库失败";
		}

		qDebug() << "算法库创建完成";
		initUserService();
		//加载已经登记过的
		StFinger *pFingerList = queryUserList("", "", 0);

		if (pFingerList != NULL) {
			StFinger *pTmp = pFingerList;
			while (pTmp != NULL) {
				long nullId = m_FVAPI.FV_GetNullId(m_veinHd);

				if (nullId <= 0) {
					qDebug() << "获取算法库位置失败";
				}
				else {
					qDebug() << "获取算法库位置成功";
					long result = m_FVAPI.FV_ImportVeinTemp(m_veinHd, nullId, pTmp->temp, NULL);
					if (result == 0) {
						qDebug() << "导入算法库成功, user_id:" << pTmp->user_id << " finger:" << pTmp->finger;
					} else {
						qDebug() << "导入算法库失败, user_id:" << pTmp->user_id << " finger:" << pTmp->finger;
					}
				}
				pTmp = pTmp->pNext;
			}
			freeUserList(pFingerList);
		}
	}
}

Df100DbusService::~Df100DbusService()
{
	if (m_devHd > 0) {
		m_FVAPI.FV_CloseDev(m_devHd);
		m_devHd = 0;
	}
	if (m_veinHd > 0) {
		m_FVAPI.FV_DestroyVeinLib(m_veinHd);
		m_veinHd = 0;
	}

	uninitUserService();
}

//这里需要实现对设备的占用操作
void Df100DbusService::Claim(QString id, bool claimed) {
	qDebug() << "dbus method ==== Claim === be called" << QString("args, === id : %1, claimed : %2 ===").arg(id).arg(claimed);

	if (claimed) {
		//占用
		if (m_devHd > 0) {
			//占用失败, 已占用
			qDebug() << "占用失败,已占用";
		} else {
			m_devHd = m_FVAPI.FV_ConnectDev((char*)"VID=8455,PID=30008", (char*)"00000000");
			if (m_devHd > 0) {
				//占用成功
				claimed = DeviceStateClaimed;
				_id = id;
				qDebug() << "占用成功";
			} else {
				//占用失败
				claimed = DeviceStateNormal;
				_id = "";
				qDebug() << "占用失败";
			}
		}
	} else {
		//释放
		if (m_devHd <= 0) {
			//释放失败
		} else {
			m_FVAPI.FV_CloseDev(m_devHd);
			m_devHd = 0;
			claimed = DeviceStateNormal;
			_id = "";
		}
	}

	//    _mw->resetPros();
}

//这里需要实现开始录制的操作
void Df100DbusService::Enroll(QString id, QString finger) {
	qDebug() << "dbus method ==== Enroll === be called" << QString("args, === id:%1 finger : %2 ===").arg(id).arg(finger);
	_id = id;
	_enrollName = finger;

	int timeout, CharaNum, ret;
	char ssChara[3][20*1024] = { 0 };
	int TempLen = 0;
	char sTmp[20*1024] = {0};
	//连接设备
	if (m_devHd <= 0) {
		qDebug() << "df100设备未连接\n";
		SendMessage(MT_Enroll, 1, "");
		return;
	}

	CharaNum = 0;

	SendMessage(MT_Enroll, 2, "");
	qDebug() << "sendMessage MT_Enroll 2";
	while(1)
	{
		int CharaLen = 0;

		timeout = 0;
		while(1)
		{
			int finger = m_FVAPI.FV_CheckFinger(m_devHd);
			if(finger > 0) break;
			if(timeout++ > 100) //10秒内没有检测到手指放入则超时返回
			{
				SendMessage(MT_Enroll, 1, "");
				qDebug() << "sendMessage MT_Enroll 1";
				qDebug() << "26-请正确放置手指/n/n";
				qDebug() << "检测手指超时\n\n";
				return;
			}
			if (!m_bEnroll) {
				//手动退出
				return;
			}
			usleep(100*1000);
		}

		CharaLen = m_FVAPI.FV_GetVeinChara(m_devHd, ssChara[CharaNum], 10*1000);
		if(CharaLen > 0)
		{
			CharaNum++;
		} else if(CharaLen == -16) {
			qDebug() << "此设备不支持特征采集\n";
			SendMessage(MT_Enroll, 1, "");
			return;
		}
		qDebug() << "获取特征" << CharaNum << CharaLen;

#if 1 //如果采集模板的时候不需要检查3次采集的是不是同一根手指则不需要这段代码
		if(CharaNum == 2)
		{
			//检查第一次和第二次采集的是不是同一根手指
			ret = m_FVAPI.FV_CharaMatch(ssChara[0], ssChara[1], 60);
			if( ret != 0)
			{
				qDebug() << "不是同一根手指\n";
				//TODO
				//bio_set_notify_abs_mid(dev, NOTIFY_ENROLL_VEIN_NOT_MATCH);
			}
		}
		if(CharaNum == 3)
		{
			//检查1,2,3次采集的是不是同一根手指
			int ret1 = m_FVAPI.FV_CharaMatch(ssChara[0], ssChara[1], 60);
			int ret2 = m_FVAPI.FV_CharaMatch(ssChara[0], ssChara[2], 60);
			int ret3 = m_FVAPI.FV_CharaMatch(ssChara[1], ssChara[2], 60);
			if( ret1 != 0 || ret2 != 0 || ret3 != 0)
			{
				qDebug() << "不是同一根手指\n";
				//TODO
				//bio_set_notify_abs_mid(dev, NOTIFY_ENROLL_VEIN_NOT_MATCH);
			}
		}
#endif

		if(CharaNum >= 3) break; //采集3次就够了

		SendMessage(MT_Enroll, 2, "");
		qDebug() << "sendMessage MT_Enroll 2";
		//TODO
		//bio_set_notify_abs_mid(dev, NOTIFY_ENROLL_CAPTRUE_AGAIN);
		//bio_print_notice("23-请再放一次\n", 1);

		//等待手指拿开再采集下一次
		while(1)
		{
			int finger = m_FVAPI.FV_CheckFinger(m_devHd);
			if(finger <= 0) break; //检测手指是否拿开，拿开后再采集下一次
			usleep(100*1000);
		}
	}

	//采集完了就可以融合成模板了
	TempLen = m_FVAPI.FV_CreateVeinTemp(ssChara[0], ssChara[1], ssChara[2], sTmp, NULL);
	if(TempLen > 0)
	{
		//保存至算法库
		long nullId = m_FVAPI.FV_GetNullId(m_veinHd);
		if (nullId <= 0) {
			qDebug() << "FV_GetNullId 失败";
			//TODO
			return;
		//	dev->ops_result = OPS_ENROLL_FAIL;
		//	return -1;
		}
		long result = m_FVAPI.FV_ImportVeinTemp(m_veinHd, nullId, sTmp, NULL);

		if (result < 0) {
			return;
			//qDebug() << "03-导入失败" ;
			//dev->ops_result = OPS_ENROLL_FAIL;
			//return -1;
		}

		_temp = QString(sTmp);
		_null_id = nullId;

		qDebug() << "00-登记成功\n";
		SendMessage(MT_Enroll, 0, "");
		qDebug() << "sendMessage MT_Enroll 2";
#if 0
		//保存至数据库
		StUser stUser;

		stUser.uid = uid;
		stUser.idx = idx;
		strcpy(stUser.bioIdxName, bio_idx_name);
		stUser.nullId = nullId;
		strcpy(stUser.temp, sTmp);

		initService();
		saveUser(&stUser);
		unInitService();
#endif	
	} else {
		qDebug() << "02-登记失败\n";
	}
}

//这里需要实现结束录制的操作
void Df100DbusService::StopEnroll() {
    qDebug() << "dbus method ==== StopEnroll === be called";
    _id = "";
    _enrollName = "";
//    _mw->resetPros();
}

//这里需要实现开始认证的操作
void Df100DbusService::Verify(QString finger) {
    qDebug() << "dbus method ==== Verify === be called" << QString("args, === finger : %1 ===").arg(finger);

	if (_id == "00000000-0000-0000-0000-000000000000") {
		SendMessage(MT_Verify, 0, "");
		return;
	}
    //创建定时器 
    //m_pTimer = new QTimer(this);
    //connect(m_pTimer, SIGNAL(timeout()), this, SLOT(verifyTask()));
    m_bVerify = true;

    QTimer::singleShot(100, this, &Df100DbusService::verifyTask);

    //m_pTimer->start(3000);

    SendMessage(MT_Verify, 3, "");
    /*
    int i = 0;

    m_verifyThread.pDf100Service = this;
    m_verifyThread.m_finger = finger;
    m_verifyThread.start();

	*/
}

void Df100DbusService::sSendMessage(int type, int code, QString msg)
{
	SendMessage(type, code, msg);
}

//这里需要实现结束认证的操作
void Df100DbusService::StopVerify() {
	qDebug() << "dbus method ==== StopVerify === be called";

	m_bVerify = false;
	//m_pTimer->stop();
	//delete m_pTimer;
	//m_pTimer = NULL;
	//m_verifyThread.quit();
	//m_verifyThread.wait();
}

//这里需要实现删除指纹的操作
void Df100DbusService::DeleteFinger(QString id, QString finger) {
    qDebug() << "dbus method ==== DeleteFinger === be called";
    qDebug() << QString("args, === id : %1, finger : %2 ===").arg(id).arg(finger);

    deleteFinger(id, finger);
//    if (_fingers.contains(finger)) {
//        _fingers.removeOne(finger);
//    }
}

//这里需要实现删除所有指纹的操作
void Df100DbusService::DeleteAllFingers(QString id) {
    qDebug() << "dbus method ==== DeleteAllFingers === be called";
    qDebug() << QString("args, === id : %1 ===").arg(id);

    deleteFinger(id, "");
//    _fingers.clear();
}

//这里需要实现返回现有指纹的操作
QStringList Df100DbusService::ListFingers(QString id) {
    qDebug() << "dbus method ==== ListFingers === be called" << QString("args, === id : %1 ===").arg(id);
//    _fingers =
    return readFinger(id);
}

void Df100DbusService::verifyTask()
{
	QString finger;
	//获取指静脉信息
	char ssChara[20*1024] = { 0 };
	int CharaLen = 0;
	int authRsp = -1;

	int timeout = 0;

	qDebug() << "开始检测";

	while (m_bVerify) {
		while (m_bVerify) {
			int finger = m_FVAPI.FV_CheckFinger(m_devHd);
			if(finger > 0) break;
			if (timeout++ > 100) {
				//20毫秒超时
				qDebug() << "20秒 超时";
				timeout = 0;
				continue;
			}

			usleep(200*1000);
		}

		//获取指静脉信息
		CharaLen = m_FVAPI.FV_GetVeinChara(m_devHd, ssChara, 10*1000);

		if (CharaLen > 0) {
			qDebug() << "获取指静脉信息成功" << ssChara;
			break;
		} else {
			return;
		}
	}

	if (finger == "any") {
		finger = "";
	}

	StFinger *pFingerList = queryUserList("", finger, 0);
	if (pFingerList) {

		StFinger *pTmp = pFingerList;

		while (pTmp != NULL) {
			int TempLen = 0;
			char sStudyTemp[20*1024] = { 0 };
			char sUserInfo[1000] = { 0 };

			qDebug() << "验证:" << ssChara << "模板:" << pTmp->temp;

			if (m_FVAPI.FV_VerifyUser((char*)ssChara, (char*)pTmp->temp, 78, sStudyTemp, sUserInfo) == 0) {
				authRsp = 0;
				_id = pTmp->user_id;
				break;
			}
			authRsp = 0;
			pTmp = pTmp->pNext;
		}

		freeUserList(pFingerList);
		if (authRsp == -1) {
			qDebug() << "验证失败";
			//验证失败
			SendMessage(MT_Verify, 1, "");
			goto finish;
		} else {
			qDebug() << "验证成功";
			SendMessage(MT_Verify, 0, ""); 
			goto finish;
		}
	} else {
		SendMessage(MT_Verify, 1, "");
		qDebug() << "未找到用户指静脉信息";
		goto finish;
	}

finish:
	return;
}

void Df100DbusService::SendMessage(int type, int code, QString msg)
{
    switch (type) {
    case MT_Enroll: {
        if (code == 0) {
            if (_id.isEmpty() || _enrollName.isEmpty() || _temp.isEmpty()) {
                return;
            }
            addFinger(_id, _enrollName, _temp, _null_id);
        }
        Q_EMIT EnrollStatus(_id, code, msg);
        break;
    }
    case MT_Verify: {
        Q_EMIT VerifyStatus(_id, code, msg);
        break;
    }
    }
}

QStringList readFinger(QString id) {

	StFinger *pFingerList = queryUserList(id, "", 0);

	if (pFingerList) {
		QStringList qStringList;
		StFinger *pTmp = pFingerList;

		while (pTmp != NULL) {
			qStringList.append(pTmp->finger);
			pTmp = pTmp->pNext;
		}
		freeUserList(pFingerList);
		return qStringList;
	} else {
        	return QStringList();
	}
}

/**
 * 保存指静脉
 */
void addFinger(QString id, QString finger, QString temp, int null_id) {
    if (finger.isEmpty() || id.isEmpty() || temp.isEmpty())
        return;

    qDebug() << QString("add Finger id : %1, finger: %2, temp :%3").arg(id).arg(finger).arg(temp);

    StFinger stFinger;

    memset(&stFinger, 0, sizeof(stFinger));
    strcpy(stFinger.user_id, id.toLatin1().data());
    strcpy(stFinger.finger, finger.toLocal8Bit().data());
    strcpy(stFinger.temp, temp.toLatin1().data());
    stFinger.null_id = null_id;

    if (addUser(stFinger) < 0) {
	    qDebug() << "保存指静脉信息失败";
    } else {
	    qDebug() << "保存指静脉信息成功";
    }

    return;
    if (finger.contains(' ')) {
        qWarning("Add Finger Error: finger name contains space!");
    }

    auto path = FingerModePath + "/" + id;
    QFile file(path);
    if (!file.exists()) {
        file.open(QFile::WriteOnly | QFile::Truncate);
    } else {
        file.open(QFile::WriteOnly | QFile::Append);
        file.write(" ", 1);
    }
    file.write(finger.toUtf8());
    file.flush();
}

void deleteFinger(QString id, QString finger) {
    if (id.isEmpty())
        return;

    qDebug() << QString("delete Finger id : %1, finger: %2").arg(id).arg(finger);

    StFinger stFinger;

    memset(&stFinger, 0, sizeof(stFinger));
    strcpy(stFinger.user_id, id.toLatin1().data());
    strcpy(stFinger.finger, finger.toLocal8Bit().data());

    if (deleteUser(stFinger) < 0) {
	    qDebug() << "删除指静脉信息失败";
    } else {
	    qDebug() << "删除指静脉信息成功";
    }

    return;
    auto path = FingerModePath + "/" + id;
    QFile file(path);
    if (!file.exists()) {
        qWarning("Delete Finger Error: finger file does not exists!");
        return;
    }

    if (finger.isEmpty()) {
        file.remove();
        return;
    }

    file.open(QFile::ReadOnly);
    QString fingerData(file.readAll());
    auto fd = fingerData.split(' ');
    fd.removeAll(finger);
    file.close();
    file.open(QFile::WriteOnly | QFile::Truncate);
    file.write(fd.join(' ').toUtf8());
    file.flush();
}

