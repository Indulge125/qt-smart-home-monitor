#include "homewidget.h"
#include "ui_homewidget.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);

    setWindowTitle("智能家居系统 - 首页");

    ui->labelTitle->setText("智能家居首页");

    ui->lineEditTemp->setReadOnly(true);
    ui->lineEditHum->setReadOnly(true);
    ui->lineEditLight->setReadOnly(true);
    ui->lineEditMqtt->setReadOnly(true);
    ui->lineEditBoard->setReadOnly(true);
    ui->lineEditLed->setReadOnly(true);

    ui->lineEditTemp->setText("-- ℃");
    ui->lineEditHum->setText("-- %");
    ui->lineEditLight->setText("--");
    ui->lineEditMqtt->setText("未连接");
    ui->lineEditBoard->setText("未连接");
    ui->lineEditLed->setText("未知");

    ui->labelMessage->setText("首页用于显示系统概览和连接状态");
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::on_btnBack_clicked()
{
    emit backToMain();
}

void HomeWidget::showMessage(const QString &message)
{
    ui->labelMessage->setText(message);
}
void HomeWidget::updateMqttState(const QString &state)
{
    ui->lineEditMqtt->setText(state);
}

void HomeWidget::updateBoardState(const QString &state)
{
    ui->lineEditBoard->setText(state);
}

void HomeWidget::updateFromBoardJson(const QString &json)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        showMessage("板子JSON解析失败");
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("temperature"))
        ui->lineEditTemp->setText(QString::number(obj.value("temperature").toDouble()) + " ℃");

    if (obj.contains("humidity"))
        ui->lineEditHum->setText(QString::number(obj.value("humidity").toDouble()) + " %");

    if (obj.contains("light"))
        ui->lineEditLight->setText(QString::number(obj.value("light").toDouble()));

    if (obj.contains("led"))
        ui->lineEditLed->setText(boolState(obj.value("led").toBool()));

    showMessage("首页数据已更新");
}

void HomeWidget::updateBoardDeviceState(const QString &device, int value)
{
    if (device == "led") {
        ui->lineEditLed->setText(value == 1 ? "已打开" : "已关闭");
    }

    showMessage("板子设备状态已更新：" + device);
}

QString HomeWidget::boolState(bool value) const
{
    return value ? "已打开" : "已关闭";
}
