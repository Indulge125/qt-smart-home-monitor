#include "networkwidget.h"
#include "ui_networkwidget.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QLineEdit>
#include <QSettings>

NetworkWidget::NetworkWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkWidget),
    mqttClient(new QMqttClient(this))
{
    ui->setupUi(this);
    setWindowTitle("智能家居系统 - 网络连接");

    ui->labelTitle->setText("MQTT网络连接配置");

    const QString configPath = QCoreApplication::applicationDirPath()
            + "/config/app.ini";
    QSettings settings(configPath, QSettings::IniFormat);

    ui->lineEditHost->setText(settings.value("mqtt/host", "127.0.0.1").toString());
    ui->lineEditPort->setText(settings.value("mqtt/port", 1883).toString());
    ui->lineEditClientId->setText(settings.value("mqtt/client_id", "SmartHomeQtClient").toString());
    ui->lineEditUsername->setText(settings.value("mqtt/username").toString());
    ui->lineEditPassword->setText(settings.value("mqtt/password").toString());
    ui->lineEditSubTopic->setText(settings.value("mqtt/subscribe_topic").toString());
    ui->lineEditPubTopic->setText(settings.value("mqtt/publish_topic").toString());

    ui->lineEditPassword->setEchoMode(QLineEdit::Password);

    ui->lineEditState->setReadOnly(true);
    ui->lineEditState->setText("未连接");

    ui->textEditReceive->setReadOnly(true);
    ui->textEditLog->setReadOnly(true);

    ui->textEditPubMessage->setPlainText("{\"lamp\":true,\"id\":0}");

    connect(mqttClient, &QMqttClient::connected,
            this, &NetworkWidget::onMqttConnected);

    connect(mqttClient, &QMqttClient::disconnected,
            this, &NetworkWidget::onMqttDisconnected);

    connect(mqttClient, &QMqttClient::messageReceived,
            this, &NetworkWidget::onMqttMessageReceived);

    appendLog("MQTT网络连接界面初始化完成");
}

NetworkWidget::~NetworkWidget()
{
    delete ui;
}

void NetworkWidget::on_btnBack_clicked()
{
    emit backToMain();
}

void NetworkWidget::on_btnConnect_clicked()
{
    QString host = ui->lineEditHost->text();
    int port = ui->lineEditPort->text().toInt();
    QString clientId = ui->lineEditClientId->text();

    if (host.isEmpty()) {
        appendLog("连接失败：服务器地址不能为空");
        return;
    }

    if (port <= 0) {
        appendLog("连接失败：端口不正确");
        return;
    }

    if (clientId.isEmpty()) {
        clientId = "SmartHomeQtClient";
    }

    mqttClient->setHostname(host);
    mqttClient->setPort(port);
    mqttClient->setClientId(clientId);

    if (!ui->lineEditUsername->text().isEmpty())
        mqttClient->setUsername(ui->lineEditUsername->text());

    if (!ui->lineEditPassword->text().isEmpty())
        mqttClient->setPassword(ui->lineEditPassword->text());

    ui->lineEditState->setText("连接中...");
    appendLog("开始连接 MQTT：" + host + ":" + QString::number(port));

        emit mqttStateChanged("连接中...");
        mqttClient->connectToHost();
}

void NetworkWidget::on_btnDisconnect_clicked()
{
    mqttClient->disconnectFromHost();
    appendLog("正在断开 MQTT 连接");
}

void NetworkWidget::on_btnSubscribe_clicked()
{
    QString topic = ui->lineEditSubTopic->text();

    if (topic.isEmpty()) {
        appendLog("订阅失败：Topic不能为空");
        return;
    }

    QMqttSubscription *sub = mqttClient->subscribe(topic);

    if (sub)
        appendLog("订阅成功：" + topic);
    else
        appendLog("订阅失败，请确认MQTT已连接：" + topic);
}

void NetworkWidget::on_btnPublish_clicked()
{
    QString topic = ui->lineEditPubTopic->text();
    QString message = ui->textEditPubMessage->toPlainText();

    if (topic.isEmpty()) {
        appendLog("发布失败：Topic不能为空");
        return;
    }

    if (message.isEmpty()) {
        appendLog("发布失败：发布内容不能为空");
        return;
    }

    qint32 result = mqttClient->publish(topic, message.toUtf8());

    if (result == -1) {
        appendLog("发布失败，请确认MQTT已连接");
        return;
    }

    appendLog("发布成功：" + topic);
    appendLog(message);
}

void NetworkWidget::onMqttConnected()
{
    ui->lineEditState->setText("已连接");
        appendLog("MQTT连接成功");
        emit mqttStateChanged("已连接");
}

void NetworkWidget::onMqttDisconnected()
{
    ui->lineEditState->setText("未连接");
    appendLog("MQTT连接已断开");
    emit mqttStateChanged("未连接");
}

void NetworkWidget::onMqttMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    QString text = QString::fromUtf8(message);

    ui->textEditReceive->append("Topic: " + topic.name());
    ui->textEditReceive->append(text);
    ui->textEditReceive->append("");

    appendLog("收到消息：" + topic.name());

    emit mqttMessageReceived(topic.name(), text);
}

void NetworkWidget::appendLog(const QString &text)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->textEditLog->append("[" + time + "] " + text);
}

void NetworkWidget::publishMessage(const QString &message)
{
    QString topic = ui->lineEditPubTopic->text();

    if (topic.isEmpty()) {
        appendLog("外部发布失败：发布Topic不能为空");
        return;
    }

    if (message.isEmpty()) {
        appendLog("外部发布失败：发布内容不能为空");
        return;
    }

    qint32 result = mqttClient->publish(topic, message.toUtf8());

    if (result == -1) {
        appendLog("外部发布失败：请确认MQTT已连接");
        return;
    }

    appendLog("外部发布成功：" + topic);
    appendLog(message);
}
