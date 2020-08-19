#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMainWindow>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QListWidgetItem>
#include <QtGui/QMovie>
#include "createuserdialog.h"
//#include "dfcx_common.h"
//#include "dfcx_drv.h"
#include "usermanage.h"

namespace Ui {
class MainWindow;
}

struct StFingerData
{
    QToolButton* pBtton;
    StVein stData;
    StFingerData(){
        pBtton = NULL;
        stData.fg_id = 0;
        stData.user_id = 0;
        stData.vein_id = 0;
        stData.null_id = 0;
#ifdef _USER_DRV_API_
        memset(stData.temp, 0, sizeof(stData.temp));
#endif
    }
};

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
    void showUserName();
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
private:
    //初始主页手指按钮
    void initFingerData(QToolButton* button, int index);
    void uninitFingerData();
    //绑定手指指静脉信息
    QToolButton* bindButtonFinger(StVein vein);
    //显示录入指静脉信息界面
    void showVeinAddWidget(QToolButton* button);
    //显示验证指静脉信息界面
    void showVeinCompareWidget(QToolButton* button);
private:
    Ui::MainWindow *ui;
    UserManage m_userManage;
    //CreateUserDialog* m_pUserDialog;
    //系统通知使用
    QSystemTrayIcon *m_pTrayIcon;
    //自定义菜单
    QMenu *m_pTrayIconMenu;
    QAction *m_pQuitAction;
    QAction *m_pShowAction;
    QTimer* m_pTimer;

    QMap<QToolButton*, StVein*> m_mapButtonFinger; //手指按钮与指静脉信息对应关系
    QToolButton* m_pCurrenFingerButton; //当前手指按钮,指静脉信息管理使用
    QButtonGroup* m_pGroupAction;
    QButtonGroup* m_pGroupLanguage;
    //用户信息
    StUser m_stUserData;
    QMovie* m_pMovieFinger;
    QTimer* m_pTimerOperation;
};

#endif // MAINWINDOW_H
