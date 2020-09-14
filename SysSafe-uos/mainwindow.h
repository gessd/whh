#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMainWindow>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QToolButton>
#include <QtGui/QMovie>

#include "customtype.h"

namespace Ui {
class MainWindow;
}

//重启软件
static const int RETCODE_RESTART = 2020;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int sysInit();
    void sysUnInit();
    void showFingerInfo();
protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    //显示窗口
    void onShowWindow();
    //退出系统
    void onCloseWindow();
    //系统消息处理
    void onIconActivated(QSystemTrayIcon::ActivationReason reason);
    //定时弹出系统消息 测试
    void onTimeOut();
    //设置按钮点击
    void onBtnTitleSet();
    //帮助按钮点击
    void onBtnTitleHelp();
    //关于按钮点击
    void onBtnTitleAbout();
    //返回主页按钮点击
    void onBtnBackMainClicked();
    //设置密码
    void onBtnSetUserPass();
    //页面切换
    void onMainWidgetCurrentChanged(int index);
    //登录验证按钮点击
    void onBtnLogonClicked();
    //设置切换
    void onItemClicked(QListWidgetItem *item);
    //主页手指点击
    void onBtnFingerClicked();
    //录入按钮点击
    void onBtnAddVeinClicked();
    //删除指静脉按钮点击
    void onBtnFingerRemoveClicked();
    //验证指静脉按钮点击
    void onBtnFingerChecked();

    //设置无动作操作
    void onSetActionClicked(int buttonId);
    //设置语言
    void onSetLanguageClicked(int buttonId);
    //定时器监控无操作处理
    void onTimeOutOperation();
    //设备长时间离线
    void onTimeOutDeviceOffline();
	//声音大小改变
	void onVoiceValueChanged(int value);

    //接受单实例消息
    void onMessageReceived(QString qstrMessage);

    //麒麟系统
    QToolButton* getFingerButton(int index);
    //USB
    void onUSBDeviceHotPlug(int drvid, int action, int devNumNow);
    //设备
    void onStatusChanged(int drvId, int statusType);
    //录入回调
    void enrollCallBack(const QDBusMessage &reply);
    //验证回调
    void verifyCallBack(const QDBusMessage &reply);
    //搜索回调
    void searchCallBack(const QDBusMessage &reply);
    void StopOpsCallBack(const QDBusMessage &reply);
    //错误信息
    void errorCallBack(const QDBusError &error);
    //获取列表时错误信息
    void errorCallback(QDBusError error);
    //指静脉信息列表
    void showFeaturesCallback(QDBusMessage callbackReply);
private:
    //初始主页手指按钮
    void initFingerData(QToolButton* button, int index);
    void uninitFingerData();
    //绑定手指指静脉信息
    void bindButtonFinger(QToolButton* pBtnFinger, int index, QToolButton *pBtnBlood);
    //显示录入指静脉信息界面
    void showVeinAddWidget(QToolButton* button);
    //显示验证指静脉信息界面
    void showVeinCompareWidget(QToolButton* button);
    //获取错误信息
    QString handleErrorResult(int error);
    //设置按钮是否有指静脉信息
    void setButtonFingerInfo(QToolButton* pButton, bool bInfo);
private:
    Ui::MainWindow *ui;
    //系统通知使用
    QSystemTrayIcon *m_pTrayIcon;
    //自定义菜单
    QMenu *m_pTrayIconMenu;
    QAction *m_pQuitAction;
    QAction *m_pShowAction;
    QToolButton* m_pCurrenFingerButton; //当前手指按钮,指静脉信息管理使用
    QButtonGroup* m_pGroupLanguage;
    QMovie* m_pMovieFinger;

    /* 用于和远端 DBus 对象交互的代理接口 */
    QDBusInterface *serviceInterface;
    int deviceCount;
    //指静脉设备信息
    DeviceInfo* m_pFingerVeinDeviceInfo;
    int m_nDeviceId;
    unsigned int m_nDBUSProgress;
    enum OPS{IDLE, ENROLL, VERIFY, SEARCH} ops;
};

#endif // MAINWINDOW_H
