#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QWidget>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttsubscription.h>

namespace Ui {
class NetworkWidget;
}

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkWidget(QWidget *parent = nullptr);
    ~NetworkWidget();

    void publishMessage(const QString &message);
signals:
    void backToMain();
    void mqttMessageReceived(const QString &topic, const QString &message);
    void mqttStateChanged(const QString &state);
private slots:
    void on_btnBack_clicked();
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnSubscribe_clicked();
    void on_btnPublish_clicked();

    void onMqttConnected();
    void onMqttDisconnected();
    void onMqttMessageReceived(const QByteArray &message, const QMqttTopicName &topic);

private:
    void appendLog(const QString &text);

private:
    Ui::NetworkWidget *ui;
    QMqttClient *mqttClient;
};

#endif // NETWORKWIDGET_H
