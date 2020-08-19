#ifndef CREATEUSERDIALOG_H
#define CREATEUSERDIALOG_H

#include <QDialog>
#include <QEvent>

namespace Ui {
class CreateUserDialog;
}

class CreateUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateUserDialog(QWidget *parent = 0);
    ~CreateUserDialog();
    void setUserName(QString qstrUserName);
    QString getPassword();
protected:
    void showEvent(QShowEvent* event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void onBtnOkClicked();
    void onPassTextEdited(QString qstrPass);
private:
    Ui::CreateUserDialog *ui;
    QString m_qstrPass;
};

#endif // CREATEUSERDIALOG_H
