#include "devicewidget.h"
#include "ui_devicewidget.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPushButton>

DeviceWidget::DeviceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devicewidget)
{
    ui->setupUi(this);


    setWindowTitle("智能家居系统 - 设备控制");

    initUi();
}

DeviceWidget::~DeviceWidget()
{
    delete ui;}

void DeviceWidget::initUi()
{
    ui->labelTitle->setText("设备控制界面");

    ui->lineEditBoardLed->setReadOnly(true);
    ui->lineEditBoardFan->setReadOnly(true);
    ui->lineEditBoardBeeper->setReadOnly(true);
    ui->lineEditMetaLed->setReadOnly(true);
    ui->lineEditMetaFan->setReadOnly(true);
    ui->lineEditMetaAlarm->setReadOnly(true);

    ui->textEditCommand->setReadOnly(true);
    ui->textEditLog->setReadOnly(true);

    ui->lineEditBoardLed->setText("未知");
    ui->lineEditBoardFan->setText("未知");
    ui->lineEditBoardBeeper->setText("未知");
    ui->lineEditMetaLed->setText("未知");
    ui->lineEditMetaFan->setText("未知");
    ui->lineEditMetaAlarm->setText("未知");

    ui->textEditCommand->setPlainText("{\"id\":0,\"lamp\":true}");

    updateToggleButton("board", "led", 0);
    updateToggleButton("board", "fan", 0);
    updateToggleButton("board", "beeper", 0);
    updateToggleButton("metaverse", "lamp", 0);
    updateToggleButton("metaverse", "fan", 0);
    updateToggleButton("metaverse", "alarm", 0);

    appendLog("设备控制界面初始化完成");
}

void DeviceWidget::on_btnBack_clicked()
{
    emit backToMain();
}

void DeviceWidget::on_btnBoardLedToggle_clicked()
{
    sendCommand("board", "led", nextValueForState(ui->lineEditBoardLed->text()));
}

void DeviceWidget::on_btnBoardFanToggle_clicked()
{
    sendCommand("board", "fan", nextValueForState(ui->lineEditBoardFan->text()));
}

void DeviceWidget::on_btnBoardBeeperToggle_clicked()
{
    sendCommand("board", "beeper", nextValueForState(ui->lineEditBoardBeeper->text()));
}

void DeviceWidget::on_btnMetaLedToggle_clicked()
{
    sendCommand("metaverse", "lamp", nextValueForState(ui->lineEditMetaLed->text()));
}

void DeviceWidget::on_btnMetaFanToggle_clicked()
{
    sendCommand("metaverse", "fan", nextValueForState(ui->lineEditMetaFan->text()));
}

void DeviceWidget::on_btnMetaAlarmToggle_clicked()
{
    sendCommand("metaverse", "alarm", nextValueForState(ui->lineEditMetaAlarm->text()));
}

void DeviceWidget::sendCommand(const QString &target, const QString &device, int value)
{
    QString json = buildCommandJson(target, device, value);

    ui->textEditCommand->setPlainText(json);

    updateDeviceState(target, device, value);

    appendLog("生成控制命令");
    appendLog(json);

    if (target == "metaverse") {
        emit sendMqttCommand(json);
    } else if (target == "board") {
        emit sendBoardCommand(device, value);
    }
}

void DeviceWidget::updateDeviceState(const QString &target, const QString &device, int value)
{
    QString state;

    if (value == 1)
        state = "已打开";
    else
        state = "已关闭";

    if (target == "board" && device == "led") {
        ui->lineEditBoardLed->setText(state);
    } else if (target == "board" && device == "fan") {
        ui->lineEditBoardFan->setText(state);
    } else if (target == "board" && device == "beeper") {
        ui->lineEditBoardBeeper->setText(state);
    } else if (target == "metaverse" && device == "lamp") {
        ui->lineEditMetaLed->setText(state);
    } else if (target == "metaverse" && device == "fan") {
        ui->lineEditMetaFan->setText(state);
    } else if (target == "metaverse" && device == "alarm") {
        ui->lineEditMetaAlarm->setText(state);
    }

    updateToggleButton(target, device, value);
}

void DeviceWidget::updateToggleButton(const QString &target, const QString &device, int value)
{
    QPushButton *button = nullptr;

    if (target == "board" && device == "led") {
        button = ui->btnBoardLedToggle;
    } else if (target == "board" && device == "fan") {
        button = ui->btnBoardFanToggle;
    } else if (target == "board" && device == "beeper") {
        button = ui->btnBoardBeeperToggle;
    } else if (target == "metaverse" && device == "lamp") {
        button = ui->btnMetaLedToggle;
    } else if (target == "metaverse" && device == "fan") {
        button = ui->btnMetaFanToggle;
    } else if (target == "metaverse" && device == "alarm") {
        button = ui->btnMetaAlarmToggle;
    }

    if (!button)
        return;

    const QString action = (value == 1) ? "关闭" : "打开";
    button->setText(action + deviceDisplayName(target, device));
}

int DeviceWidget::nextValueForState(const QString &state) const
{
    if (state == "已打开")
        return 0;

    return 1;
}

QString DeviceWidget::deviceDisplayName(const QString &target, const QString &device) const
{
    if (target == "board" && device == "led")
        return "板子LED";

    if (target == "board" && device == "fan")
        return "板子风扇";

    if (target == "board" && device == "beeper")
        return "板子蜂鸣器";

    if (target == "metaverse" && device == "lamp")
        return "元宇宙LED";

    if (target == "metaverse" && device == "fan")
        return "元宇宙风扇";

    if (target == "metaverse" && device == "alarm")
        return "元宇宙报警器";

    return device;
}

QString DeviceWidget::buildCommandJson(const QString &target, const QString &device, int value) const
{
    QJsonObject obj;

    if (target == "metaverse") {
        obj["id"] = 0;

        if (device == "lamp") {
            obj["lamp"] = (value == 1);
        } else if (device == "fan") {
            obj["fan"] = (value == 1);
        } else if (device == "alarm") {
            obj["alarm"] = (value == 1);
        }
    } else {
        obj["target"] = target;
        obj["device"] = device;
        obj["value"] = value;
    }

    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

void DeviceWidget::appendLog(const QString &text)
{
    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->textEditLog->append("[" + time + "] " + text);
}
