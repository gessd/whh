#include "xmessagebox.h"
#include "ui_xmessagebox.h"
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QLocalSocket>

#define _MessageBoxStyle_ "QDialog{border-image:url(:/images/background.png);}\
                            #widgetTitle{border-bottom:1px solid #FF0000;}\
                            QToolButton{border-image:url(:/images/button.png);}"

XMessageBox::XMessageBox(QWidget * parent, const QString & title, const QString & text, const QPixmap& pixmap, bool bButtonYes) :
    QDialog(parent),
    ui(new Ui::XMessageBox)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
	if (parent) {
		QRect rect = parent->geometry();
		move((rect.width() - width()) / 2, (rect.height() - height()) / 2);
	}
	else {
		QDesktopWidget *pDesk = QApplication::desktop();
		move((pDesk->width() - width()) / 2, (pDesk->height() - height()) / 2);
	}
	ui->btnYes->setVisible(bButtonYes);
	ui->labelTitle->installEventFilter(this);
	ui->labelTitle->setText(title);
	ui->labelText->setText(text);
	ui->labelIcon->setPixmap(pixmap);

	connect(ui->btnYes, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(reject()));
}

XMessageBox::~XMessageBox()
{
    delete ui;
}

XMessageBox::StandardButton XMessageBox::information(QWidget * parent, const QString & title, const QString & text)
{
    XMessageBox box(parent, title, text, QMessageBox::standardIcon(QMessageBox::Information));
    XMessageBox::StandardButton stand = XMessageBox::No;
    box.setStyleSheet(_MessageBoxStyle_);
    if (QDialog::Accepted == box.exec()) stand = XMessageBox::Yes;
	return stand;
}

XMessageBox::StandardButton XMessageBox::question(QWidget * parent, const QString & title, const QString & text)
{
    XMessageBox box(parent, title, text, QMessageBox::standardIcon(QMessageBox::Question), true);
    XMessageBox::StandardButton stand = XMessageBox::No;
    box.setStyleSheet(_MessageBoxStyle_);
    if (QDialog::Accepted == box.exec()) stand = XMessageBox::Yes;
	return stand;
}

XMessageBox::StandardButton XMessageBox::warning(QWidget * parent, const QString & title, const QString & text)
{
    XMessageBox box(parent, title, text, QMessageBox::standardIcon(QMessageBox::Warning));
    XMessageBox::StandardButton stand = XMessageBox::No;
    box.setStyleSheet(_MessageBoxStyle_);
    if (QDialog::Accepted == box.exec()) stand = XMessageBox::Yes;
	return stand;
}

bool XMessageBox::eventFilter(QObject * watched, QEvent * event)
{
	if (ui->labelTitle != watched) return false;
	static bool    m_bMousePressed = false;
	static QPoint  m_pointMouse;
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = (QMouseEvent *)event;
		m_bMousePressed = true;
		m_pointMouse = mouseEvent->globalPos() - pos();
		return true;
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		m_bMousePressed = false;
		return true;
	}
	else if (event->type() == QEvent::MouseMove) {
		QMouseEvent *mouseEvent = (QMouseEvent *)event;
		if (m_bMousePressed) {
			move(mouseEvent->globalPos() - m_pointMouse);
		}
		return true;
	}
	return false;
}

int XMessageBox::exec()
{
	return QDialog::exec();
}
