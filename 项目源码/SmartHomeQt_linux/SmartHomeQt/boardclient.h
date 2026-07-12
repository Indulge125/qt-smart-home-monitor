#ifndef BOARDCLIENT_H
#define BOARDCLIENT_H

#include <QObject>
#include <QTcpSocket>

class BoardClient : public QObject
{
    Q_OBJECT

public:
    explicit BoardClient(QObject *parent = nullptr);

    void connectToBoard(const QString &host, int port);
    void disconnectFromBoard();
    void requestSensorData();
    void sendControlCommand(const QString &device, int value);

    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void sensorDataReceived(const QString &json);
    void logMessage(const QString &message);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

private:
    void sendJsonLine(const QString &json);

private:
    QTcpSocket *socket;
    QByteArray buffer;
};

#endif // BOARDCLIENT_H
