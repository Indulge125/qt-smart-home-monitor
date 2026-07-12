#include "widget.h"
#include "ui_widget.h"

#include <QLineEdit>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("智能家居系统 - 登录");

    ui->lineEditUsername->setPlaceholderText("请输入账号");
    ui->lineEditPassword->setPlaceholderText("请输入密码");
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);

    ui->labelMessage->setText("请先登录或注册账号");
    ui->labelMessage->setStyleSheet("color:#374151;");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btnLogin_clicked()
{
    QString username = ui->lineEditUsername->text();
    QString password = ui->lineEditPassword->text();

    if (m_userManager.validateUser(username, password)) {
        showMessage("登录成功", true);
        emit loginSuccess(username.trimmed());
        return;
    }

    showMessage("账号不存在或密码错误", false);
}

void Widget::on_btnGoRegister_clicked()
{
    emit goRegister();
}

void Widget::showMessage(const QString &message, bool success)
{
    ui->labelMessage->setText(message);

    if (success)
        ui->labelMessage->setStyleSheet("color:#15803d;");
    else
        ui->labelMessage->setStyleSheet("color:#dc2626;");
}
