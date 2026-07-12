#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>
#include "usermanager.h"

namespace Ui {
class RegisterWidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

signals:
    void backToLogin();

private slots:
    void on_btnRegister_clicked();
    void on_btnBackLogin_clicked();

private:
    void showMessage(const QString &message, bool success);

private:
    Ui::RegisterWidget *ui;
    UserManager m_userManager;
};

#endif // REGISTERWIDGET_H
