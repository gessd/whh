#ifndef XMESSAGEBOX_H
#define XMESSAGEBOX_H

#include <QDialog>
#include <QMessageBox>
#include <QEvent>
#include <QPoint>

namespace Ui {
class XMessageBox;
}

class XMessageBox : public QDialog
{
    Q_OBJECT
public:
	enum StandardButton {
		// keep this in sync with QDialogButtonBox::StandardButton and QPlatformDialogHelper::StandardButton
		Yes = 0x00004000,
		No = 0x00010000
	};
	//XMessageBox::Yes || XMessageBox::No
	static XMessageBox::StandardButton information(QWidget *parent, const QString &title, const QString &text);
	
	static XMessageBox::StandardButton question(QWidget *parent, const QString &title, const QString &text);
	
	static XMessageBox::StandardButton warning(QWidget *parent, const QString &title, const QString &text);

    static XMessageBox::StandardButton message(QWidget *parent, const QString &title, const QString &text);
protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
private:
	explicit XMessageBox(QWidget * parent, const QString & title, const QString & text, const QPixmap& pixmap, bool bButtonYes=false);
	~XMessageBox();
	int exec();
private:
    Ui::XMessageBox *ui;
};

#endif // XMESSAGEBOX_H
