#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include <QWidget>

namespace Ui {
class devicewidget;
}

class DeviceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceWidget(QWidget *parent = nullptr);
    ~DeviceWidget();

signals:
    void backToMain();
    void sendMqttCommand(const QString &json);
    void sendBoardCommand(const QString &device, int value);
private slots:
    void on_btnBack_clicked();

    void on_btnBoardLedToggle_clicked();
    void on_btnBoardFanToggle_clicked();
    void on_btnBoardBeeperToggle_clicked();
    void on_btnMetaLedToggle_clicked();
    void on_btnMetaFanToggle_clicked();
    void on_btnMetaAlarmToggle_clicked();

private:
    void initUi();
    void sendCommand(const QString &target, const QString &device, int value);
    void updateDeviceState(const QString &target, const QString &device, int value);
    void updateToggleButton(const QString &target, const QString &device, int value);
    int nextValueForState(const QString &state) const;
    QString deviceDisplayName(const QString &target, const QString &device) const;
    QString buildCommandJson(const QString &target, const QString &device, int value) const;
    void appendLog(const QString &text);

private:
    Ui::devicewidget *ui;
};

#endif // DEVICEWIDGET_H
