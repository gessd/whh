#ifndef _STYLEDEFINE_H_
#define _STYLEDEFINE_H_

//背景图片
#define _BackgroundStyleButton "#centralWidget{image:url(:/images/background.png);}"

//有指静脉信息按钮样式
#define _FingerStyleButton "QToolButton{border:none;image:url(:/images/fingerbutton.png);}"

//无指静脉信息按钮样式
#define _ButtonFingerNoDataStytle "QToolButton{border:1px solid #000000;background-color:#FFFFFF;}"

#define _ButtonPropertyName "btnBlood"
#define _BindFingerBloodButton(finger,blood) finger->setProperty(_ButtonPropertyName, QVariant::fromValue((QToolButton*)blood));blood->setVisible(false);finger->setStyleSheet(_ButtonFingerNoDataStytle);
#define _MaxProgressBar 100
#define _FingerProgress(num) ui->progressBarFinger->setValue(num);

////--------------------------------------------////
//配置信息
//配置文件
#define _SetFile "set.ini"
//无操作时动作 [0,1,2,3]和界面对应
#define _ActionSet "Setting/Action"
//设备最长离线时间 秒
#define _MaxTimeOutDeviceOffline "Setting/MaxTimeOutDeviceOffline"
//语言 [0,1,2]和界面对应
#define _LanguageSet "Setting/Language"
#define _VoiceSet "Setting/Voice"
//密码大写字母 1包含 0可以不包含
#define _PassUpper_  "Password/Upper"
//密码小写字母 1包含 0可以不包含
#define _PassLower_  "Password/Lower"
//密码数字 1包含 0可以不包含
#define _PassNumber_  "Password/Number"
//密码特殊字符 1包含 0可以不包含
#define _PassSpecial_  "Password/Special"
//密码长度
#define _PassLength_  "Password/Length"
//错误信息颜色
#define _MessageErrorColor "MessageColor/Error"
////============================================////

//无动作时间 秒
#define _NoActionMaxTime 60*60

//设备离线时动作ID
enum _ActionDeviceOffline
{
    ActionNone=0,
    ActionLockScreen,
    ActionLogout,
    ActionReboot,
    ActionHalt
};


//tcp通信使用
#define _MainProgramName_ "SysSafe.exe"
#define _CheckDevideProgramName_ "DeviceStatus.exe"
#define _LocasServerName_ "syssafedevicecheck"
#define _ShowWindow_ "show"
#define _CloseWindow_ "close"

//注册表
//1:1验证
#define _RegeditPathOneCheck_ "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Authentication\Credential Providers\{42001976-0E5D-4F2E-BCC6-8BF6AD3CEE5C}"
#define _RegetditKeyOneCheck_ "DfcxCredentialProvider"
//1:N验证
#define _RegeditPathNCheck_   "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Authentication\Credential Providers\{7A11D275-8634-47A0-84CF-94773519E865}"
#define _RegetditKeyNCheck_   "Dfcx1NCredentialProvider"
#endif // _STYLEDEFINE_H_
