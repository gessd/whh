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

#ifndef TESTDBUSSERVICE_H
#define TESTDBUSSERVICE_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#include <QDBusAbstractAdaptor>
#include "VerifyThread.h"

enum {
    DeviceStateNormal = 1 << 0,            // 设备正常可用
    DeviceStateClaimed = 1 << 1,           // 设备被独占
};

enum {
    DeviceTypeUnknown = 0,    // 未知类型
    DeviceTypeScanning,       // 扫描式指纹设备
    DeviceTypeTouch,          // 触摸式指纹设备
};

//class MainWindow;
//
typedef char *LPCTSTR;

typedef struct _API_FV
{  
	long (*FV_SetDebug) (long on);
	long (*FV_ConnectDev) (LPCTSTR sDev, LPCTSTR sPasswrod); 
	long (*FV_CloseDev) (long lDevHandle);
	long (*FV_SendCmdPacket) (long lDevHandle, long lCmd, LPCTSTR sData);
	long (*FV_RecvCmdPacket) (long lDevHandle, LPCTSTR sData, long lTimeout);
	long (*FV_RecvUartData) (long lDevHandle, LPCTSTR sData, long lSize, long lTimeout);
	long (*FV_GetDevParam) (long lDevHandle, LPCTSTR sParam);
	long (*FV_SetDevParam) (long lDevHandle, LPCTSTR sParam);
	long (*FV_PlayDevSound) (long lDevHandle, long lSound);
	long (*FV_CheckFinger) (long lDevHandle);
	long (*FV_GetVeinChara) (long lDevHandle, LPCTSTR sChara, long lTimeout);
	long (*FV_ReadDevTemp) (long lDevHandle, long lUserId, LPCTSTR sTemp);
	long (*FV_WriteDevTemp) (long lDevHandle, long lUserId, LPCTSTR sTemp, LPCTSTR sUserInfo);
	long (*FV_DeleteDevTemp) (long lDevHandle, long lUserId);
	long (*FV_GetDevDebugInfo) (long lDevHandle, LPCTSTR sFileName);
	long (*FV_Upgrade) (long lDevHandle, LPCTSTR sFileName);
	long (*FV_GetImgFormData) (LPCTSTR sData, long lDataLen, LPCTSTR sImg);
	long (*FV_GetImgFormDev) (long lDevHandle, LPCTSTR sImg);
	long (*FV_GetCharaFromImg) (LPCTSTR sImg, LPCTSTR sChara);
	long (*FV_ReadDevLog) (long lDevHandle, LPCTSTR sStartTime, LPCTSTR sEndTime, LPCTSTR sLog);
	long (*FV_WriteDevUserInfo) (long lDevHandle, long lStartID, long lNum, LPCTSTR sUserInfo);
	long (*FV_ReadDevUserInfo) (long lDevHandle, long lStartID, long lNum, LPCTSTR sUserInfo);

	long (*FV_CreateVeinLib) (long lUserNum);
	long (*FV_DestroyVeinLib) (long lLibHandle);
	long (*FV_GetNullId)(long lLibHandle);
	long (*FV_ImportVeinTemp) (long lLibHandle, long lUserId, LPCTSTR sTemp, LPCTSTR sUserInfo);
	long (*FV_ExportVeinTemp) (long lLibHandle, long lUserId, LPCTSTR sTemp);
	long (*FV_CleanVeinTemp) (long lLibHandle, long lUserId);
	long (*FV_SetUserInfo) (long lLibHandle, long lUserId, LPCTSTR sUserInfo);
	long (*FV_GetUserInfo) (long lLibHandle, long lUserId, LPCTSTR sUserInfo);
	long (*FV_GetTempUserInfo) (LPCTSTR sTemp, LPCTSTR sUserInfo);
	long (*FV_SearchUser) (long lLibHandle, LPCTSTR sChara, long lTh, LPCTSTR sUserInfo);
	long (*FV_VerifyUser) (LPCTSTR sTemp, LPCTSTR sChara, long lScore, LPCTSTR sStudyTemp, LPCTSTR sUserInfo);
	long (*FV_CreateVeinTemp) (LPCTSTR sChara1, LPCTSTR sChara2, LPCTSTR sChara3, LPCTSTR sTemp, LPCTSTR sUserInfo);
	long (*FV_CharaMatch) (LPCTSTR sChara1, LPCTSTR sChara2, long Score);

	long (*FV_NetPackParse) (LPCTSTR bNetBuf, long lBufSize, LPCTSTR sCmd, LPCTSTR sSN, LPCTSTR sData);
	long (*FV_AsciiToHex) (LPCTSTR sAscii, LPCTSTR bHex);
	long (*FV_HexToAscii) (LPCTSTR bHex, long lByte, LPCTSTR sAscii);

	long (*FV_SocketServerInit) (long lPort, LPCTSTR sParam);
	long (*FV_SocketAccept) (long lSocket, LPCTSTR sInfo);
	long (*FV_SocketRecvPack) (long lSocket, LPCTSTR sCMD, LPCTSTR sSN, LPCTSTR sData);
	long (*FV_SocketSendPack) (long lSocket, long lCmd, LPCTSTR sData);
	long (*FV_SocketClose) (long lSocket);

} FVAPI_t, *pFVAPI_t;


class Df100DbusService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.dfcx.df100.VFingerprint")

public:
    explicit Df100DbusService(QObject *parent);
    virtual ~Df100DbusService();

public: // PROPERTIES
    //需要提供的属性
    Q_PROPERTY(QString Name READ name)
    Q_PROPERTY(int State READ state)
    Q_PROPERTY(int Type READ type)
    Q_PROPERTY(int Capability READ capability)

    QString name() { return "vdevice"; }
    int state() { return m_state; }
    int type() { return DeviceTypeScanning; }
    int capability() { return 1; }


    void enrollTask();
public Q_SLOTS:
    //需要实现的接口函数
    void Claim(QString id, bool claimed);
    void Enroll(QString id, QString finger);
    void StopEnroll();
    void Verify(QString finger);
    void StopVerify();
    void DeleteFinger(QString id, QString finger);
    void DeleteAllFingers(QString id);
    QStringList ListFingers(QString id);

    void sSendMessage(int type, int code, QString msg);
    void verifyTask();

Q_SIGNALS:
    //发出录入过程中的状态消息
    void EnrollStatus(QString id, int code, QString msg);
    //发出认证过程中的状态消息
    void VerifyStatus(QString id, int code, QString msg);
    //发出手指按下的状态消息
    void Touch(QString id, bool pressed);

    void siSendMessage(int type, int code, QString msg);

public:
    void varify(bool v);

private:
    //QTimer *m_pTimer;
    QStringList _fingers;
    int m_state{DeviceStateNormal};
    QTimer *m_signalTimer{nullptr};

    QString _enrollName{""};
    QString _id{""};
    QString _temp{""};
    int _null_id{0};

    ////////////////////////////////////////////
    /// 用于测试指纹框架的部分,指纹适配***无需关注***
public:
    enum MsgType {
        MT_Enroll = 0,
        MT_Verify
    };
public Q_SLOTS:
    void SendMessage(int type, int code, QString msg);
private:
	FVAPI_t m_FVAPI;
	long m_veinHd;
	long m_devHd;
	char m_captureVein[20*1024];

	VerifyThread m_verifyThread;
	volatile bool m_bVerify;
	QString m_finger;

	bool m_bEnroll;
};

#endif // TESTDBUSSERVICE_H
