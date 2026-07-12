#include "registerwidget.h"
#include "ui_registerwidget.h"

#include <QLineEdit>

RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);

    setWindowTitle("智能家居系统 - 注册");

    ui->lineEditUsername->setPlaceholderText("请输入用户名");
    ui->lineEditPassword->setPlaceholderText("请输入密码");
    ui->lineEditConfirmPassword->setPlaceholderText("请再次输入密码");

    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
    ui->lineEditConfirmPassword->setEchoMode(QLineEdit::Password);

    showMessage("请输入注册信息", true);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::on_btnRegister_clicked()
{
    QString username = ui->lineEditUsername->text().trimmed();
    QString password = ui->lineEditPassword->text().trimmed();
    QString confirm = ui->lineEditConfirmPassword->text().trimmed();

    if (m_userManager.userExists(username)) {
        ui->labelMessage->setText("该账号已注册，请直接登录");
        return;
    }

    if (password != confirm) {
        ui->labelMessage->setText("两次输入的密码不一致");
        return;
    }

    QString errorMessage;
    if (m_userManager.registerUser(username, password, &errorMessage)) {
        ui->labelMessage->setText("注册成功，请返回登录");
    } else {
        ui->labelMessage->setText(errorMessage);
    }
}

void RegisterWidget::on_btnBackLogin_clicked()
{
    emit backToLogin();
}

void RegisterWidget::showMessage(const QString &message, bool success)
{
    ui->labelMessage->setText(message);

    if (success)
        ui->labelMessage->setStyleSheet("color:#15803d;");
    else
        ui->labelMessage->setStyleSheet("color:#dc2626;");
}
