#include "boardclient.h"
#include <QJsonDocument>
#include <QJsonObject>

BoardClient::BoardClient(QObject *parent)
    : QObject(parent),
      socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::readyRead,
            this, &BoardClient::onReadyRead);

    connect(socket, &QTcpSocket::connected,
            this, &BoardClient::onConnected);

    connect(socket, &QTcpSocket::disconnected,
            this, &BoardClient::onDisconnected);

    connect(socket,
            static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error),
            this,
            &BoardClient::onError);
}

void BoardClient::connectToBoard(const QString &host, int port)
{
    emit logMessage("开始连接板子：" + host + ":" + QString::number(port));
    socket->connectToHost(host, port);
}

void BoardClient::disconnectFromBoard()
{
    socket->disconnectFromHost();
}

void BoardClient::requestSensorData()
{
    sendJsonLine("{\"cmd\":\"read\"}");
}

void BoardClient::sendControlCommand(const QString &device, int value)
{
    QJsonObject obj;
    obj["cmd"] = "control";
    obj["device"] = device;
    obj["value"] = value;

    QJsonDocument doc(obj);
    sendJsonLine(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

bool BoardClient::isConnected() const
{
    return socket->state() == QAbstractSocket::ConnectedState;
}

void BoardClient::onReadyRead()
{
    buffer.append(socket->readAll());

    while (buffer.contains('\n')) {
        int index = buffer.indexOf('\n');
        QByteArray line = buffer.left(index).trimmed();
        buffer.remove(0, index + 1);

        if (line.isEmpty())
            continue;

        QString json = QString::fromUtf8(line);

        emit logMessage("收到板子数据：" + json);
        emit sensorDataReceived(json);
    }
}

void BoardClient::onConnected()
{
    emit logMessage("板子连接成功");
    emit connected();
}

void BoardClient::onDisconnected()
{
    emit logMessage("板子连接断开");
    emit disconnected();
}

void BoardClient::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit logMessage("板子连接错误：" + socket->errorString());
}

void BoardClient::sendJsonLine(const QString &json)
{
    if (!isConnected()) {
        emit logMessage("发送失败：板子未连接");
        return;
    }

    QByteArray data = json.toUtf8();
    data.append('\n');

    socket->write(data);
    emit logMessage("发送到板子：" + json);
}
