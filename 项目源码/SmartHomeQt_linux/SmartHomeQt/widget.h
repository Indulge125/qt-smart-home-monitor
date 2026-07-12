#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "usermanager.h"

class QTimer;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void loginSuccess(const QString &username);
    void goRegister();

private slots:
    void on_btnLogin_clicked();
    void on_btnGoRegister_clicked();

private:
    void showMessage(const QString &message, bool success);
    void startLoginLock();

private:
    Ui::Widget *ui;
    UserManager m_userManager;

    int m_failedCount = 0;
    int m_lockSeconds = 0;
    QTimer *m_lockTimer = nullptr;
};

#endif // WIDGET_H
