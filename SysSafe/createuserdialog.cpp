#include "createuserdialog.h"
#include "ui_createuserdialog.h"
#include <QDesktopWidget>
#include <QMouseEvent>
#include "setconfig.h"
#include "styledefine.h"

CreateUserDialog::CreateUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateUserDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle(tr("设置密码"));
    connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(onBtnOkClicked()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->lineEditPass, SIGNAL(textEdited(QString)), this, SLOT(onPassTextEdited(QString)));
    ui->widgetTitle->installEventFilter(this);
}

CreateUserDialog::~CreateUserDialog()
{
    delete ui;
}

void CreateUserDialog::setUserName(QString qstrUserName)
{
    ui->lineEditUser->setText(qstrUserName);
}

QString CreateUserDialog::getPassword()
{
    return m_qstrPass;
}

void CreateUserDialog::showEvent(QShowEvent *event)
{
    ui->lineEditPass->clear();
    ui->lineEditPassword->clear();
    QString qstrPassMsg = tr("密码需包含");
    if(1 == SetConfig::getSetValue(_PassUpper_, 0).toUInt()){
        qstrPassMsg.append(tr(" 大写字母"));
    }
    if(1 == SetConfig::getSetValue(_PassLower_, 0).toUInt()){
        qstrPassMsg.append(tr(" 小写字母"));
    }
    if(1 == SetConfig::getSetValue(_PassNumber_, 0).toUInt()){
        qstrPassMsg.append(tr(" 数字"));
    }
    if(1 == SetConfig::getSetValue(_PassSpecial_, 0).toUInt()){
        qstrPassMsg.append(tr(" 特殊符号"));
    }
    unsigned int nLen = SetConfig::getSetValue(_PassLength_, 0).toUInt();
    if(nLen > 0){
        qstrPassMsg.append(tr("密码需大于:"));
        qstrPassMsg.append(QString::number(nLen)+tr("字符"));
    }
    ui->labelErrorMsg->setText(qstrPassMsg);
    QObject* pObj = parent();
    if(pObj){
        QWidget* pWidget = dynamic_cast<QWidget*>(pObj);
        if(pWidget){
            QRect rect = pWidget->geometry();
            move((rect.width() - width()) / 2, (rect.height() - height()) / 2);
        }else {
            QDesktopWidget *pDesk = QApplication::desktop();
            move((pDesk->width() - width()) / 2, (pDesk->height() - height()) / 2);
        }
    }
}

void CreateUserDialog::closeEvent(QCloseEvent *event)
{

}

bool CreateUserDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (ui->widgetTitle != watched) return false;
    static bool    bMousePressed = false;
    static QPoint  pointMouse;
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        bMousePressed = true;
        pointMouse = mouseEvent->globalPos() - pos();
        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        bMousePressed = false;
        return true;
    }
    else if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        if (bMousePressed) {
            move(mouseEvent->globalPos() - pointMouse);
        }
        return true;
    }
    return false;
}

void CreateUserDialog::onBtnOkClicked()
{
    if(ui->lineEditUser->text().trimmed().isEmpty()){
        ui->labelErrorMsg->setText(tr("用户名不能为空"));
        return;
    }
    if(ui->lineEditPass->text().trimmed().isEmpty()){
        ui->labelErrorMsg->setText(tr("密码不能为空"));
        return;
    }
    QString qstrPass1 = ui->lineEditPass->text().trimmed();
    QString qstrPass2 = ui->lineEditPassword->text().trimmed();
    if(qstrPass1 != qstrPass2){
        ui->labelErrorMsg->setText(tr("两次密码输入不一致"));
        return;
    }
    //判断密码是否符合规则
    if(1 == SetConfig::getSetValue(_PassUpper_, 0).toUInt()){
        if(false == qstrPass1.contains(QRegExp("[A-Z]"))){
            ui->labelErrorMsg->setText(tr("未包含大写字母"));
            return;
        }
    }
    if(1 == SetConfig::getSetValue(_PassLower_, 0).toUInt()){
        if(false == qstrPass1.contains(QRegExp("[a-z]"))){
            ui->labelErrorMsg->setText(tr("未包含小写字母"));
            return;
        }
    }
    if(1 == SetConfig::getSetValue(_PassNumber_, 0).toUInt()){
        if(false == qstrPass1.contains(QRegExp("[0-9]"))){
            ui->labelErrorMsg->setText(tr("未包含数字"));
            return;
        }
    }
    if(1 == SetConfig::getSetValue(_PassSpecial_, 0).toUInt()){
        if (false == qstrPass1.contains(QRegExp(QString::fromUtf8("[`~!@#$%^&*()_+={}|:;<>,./?·~！@#￥%……&*（）——+-=，。、；‘：“”’【】\\-\\[\\]\\\"\\\\]")))) {
            ui->labelErrorMsg->setText(tr("未包含特殊字符"));
            return;
        }
    }
    unsigned int nLen = SetConfig::getSetValue(_PassLength_, 0).toUInt();
    if(nLen > 0){
        if(nLen > qstrPass1.length()){
            ui->labelErrorMsg->setText(tr("长度不够")+QString::number(nLen));
            return;
        }
    }

    m_qstrPass = qstrPass1;
    accept();
}

void CreateUserDialog::onPassTextEdited(QString qstrPass)
{

}
