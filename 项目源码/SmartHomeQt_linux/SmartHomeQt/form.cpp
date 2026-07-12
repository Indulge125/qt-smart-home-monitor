#include "form.h"
#include "ui_form.h"

#include <QDebug>
#include <QCoreApplication>
#include <QList>
#include <QPushButton>
#include <QSettings>
#include <QStyle>
#include <QStringList>
#include <QTimer>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);

    setWindowTitle("智能家居系统 - 控制台");
    setMinimumSize(980, 680);

    ui->labelTitle->setText("智能家居系统");
    ui->labelUser->setText("当前用户：--");

    while (ui->stackedWidget->count() > 0) {
        QWidget *placeholder = ui->stackedWidget->widget(0);
        ui->stackedWidget->removeWidget(placeholder);
        delete placeholder;
    }

    homeWidget = new HomeWidget(ui->stackedWidget);
    monitorWidget = new MonitorWidget(ui->stackedWidget);
    networkWidget = new NetworkWidget(ui->stackedWidget);
    deviceWidget = new DeviceWidget(ui->stackedWidget);
    settingsWidget = new SettingsWidget(ui->stackedWidget);

    ui->stackedWidget->addWidget(homeWidget);
    ui->stackedWidget->addWidget(monitorWidget);
    ui->stackedWidget->addWidget(networkWidget);
    ui->stackedWidget->addWidget(deviceWidget);
    ui->stackedWidget->addWidget(settingsWidget);
    switchPage(homeWidget, ui->btnHome);

    boardClient = new BoardClient(this);

    connect(homeWidget, &HomeWidget::backToMain, this, [=]() {
        switchPage(homeWidget, ui->btnHome);
    });

    connect(networkWidget, &NetworkWidget::backToMain, this, [=]() {
        switchPage(homeWidget, ui->btnHome);
    });

    connect(monitorWidget, &MonitorWidget::backToMain, this, [=]() {
        switchPage(homeWidget, ui->btnHome);
    });

    connect(deviceWidget, &DeviceWidget::backToMain, this, [=]() {
        switchPage(homeWidget, ui->btnHome);
    });

    connect(settingsWidget, &SettingsWidget::backToMain, this, [=]() {
        switchPage(homeWidget, ui->btnHome);
    });

    connect(settingsWidget, &SettingsWidget::themeChanged,
            this, &Form::applyTheme);

    connect(deviceWidget, &DeviceWidget::sendMqttCommand,
            networkWidget, &NetworkWidget::publishMessage);
    connect(deviceWidget, &DeviceWidget::sendBoardCommand,
            boardClient, &BoardClient::sendControlCommand);

    connect(monitorWidget, &MonitorWidget::requestBoardData,
            boardClient, &BoardClient::requestSensorData);

    connect(boardClient, &BoardClient::sensorDataReceived,
            monitorWidget, &MonitorWidget::updateFromBoardJson);
    connect(boardClient, &BoardClient::logMessage, this, [](const QString &message) {
        qDebug() << message;
    });

    connect(networkWidget, &NetworkWidget::mqttStateChanged,
            homeWidget, &HomeWidget::updateMqttState);

    connect(boardClient, &BoardClient::connected, this, [=]() {
        homeWidget->updateBoardState("已连接");
    });

    connect(boardClient, &BoardClient::disconnected, this, [=]() {
        homeWidget->updateBoardState("未连接");
    });

    connect(boardClient, &BoardClient::sensorDataReceived,
            homeWidget, &HomeWidget::updateFromBoardJson);

    connect(deviceWidget, &DeviceWidget::sendBoardCommand,
            homeWidget, &HomeWidget::updateBoardDeviceState);

    QTimer::singleShot(0, this, [this]() {
        const QString configPath = QCoreApplication::applicationDirPath()
                + "/config/app.ini";
        QSettings settings(configPath, QSettings::IniFormat);
        const QString host = settings.value("tcp/host", "127.0.0.1").toString();
        const int port = settings.value("tcp/port", 8080).toInt();
        boardClient->connectToBoard(host, port);
    });
}

Form::~Form()
{
    delete ui;
}

void Form::setCurrentUser(const QString &username)
{
    ui->labelUser->setText("当前用户：" + username);
}

void Form::on_btnLogout_clicked()
{
    emit logoutRequested();
}

void Form::on_btnHome_clicked()
{
    switchPage(homeWidget, ui->btnHome);
}

void Form::on_btnNetwork_clicked()
{
    switchPage(networkWidget, ui->btnNetwork);
}

void Form::on_btnMonitor_clicked()
{
    switchPage(monitorWidget, ui->btnMonitor);
}

void Form::on_btnDevice_clicked()
{
    switchPage(deviceWidget, ui->btnDevice);
}

void Form::on_btnSettings_clicked()
{
    switchPage(settingsWidget, ui->btnSettings);
}

void Form::switchPage(QWidget *page, QPushButton *button)
{
    ui->stackedWidget->setCurrentWidget(page);
    setActiveButton(button);
}

void Form::setActiveButton(QPushButton *button)
{
    const QList<QPushButton *> navButtons = {
        ui->btnHome,
        ui->btnMonitor,
        ui->btnNetwork,
        ui->btnDevice,
        ui->btnSettings
    };

    for (QPushButton *navButton : navButtons) {
        navButton->setProperty("active", navButton == button);
        navButton->style()->unpolish(navButton);
        navButton->style()->polish(navButton);
        navButton->update();
    }
}

void Form::applyTheme(const QString &themeName, const QString &backgroundColor)
{
    const QList<QWidget *> themedWidgets = {
        this,
        homeWidget,
        monitorWidget,
        networkWidget,
        deviceWidget,
        settingsWidget
    };

    for (QWidget *widget : themedWidgets) {
        widget->setStyleSheet(recolorStyleSheet(widget->styleSheet(), backgroundColor));
    }

    ui->sidebar->setStyleSheet(
                "QFrame#sidebar {\n"
                "    background-color: " + sidebarColorForTheme(themeName) + ";\n"
                "    border-right: 1px solid #e5eaf1;\n"
                "}");
}

QString Form::sidebarColorForTheme(const QString &themeName) const
{
    if (themeName == "清爽蓝")
        return "#f7fbff";

    if (themeName == "护眼绿")
        return "#f3fbf6";

    if (themeName == "暖阳米")
        return "#fffaf2";

    if (themeName == "淡雅紫")
        return "#faf7ff";

    if (themeName == "薄荷青")
        return "#f4fffb";

    return "#ffffff";
}

QString Form::recolorStyleSheet(const QString &styleSheet, const QString &backgroundColor) const
{
    QString updatedStyle = styleSheet;
    const QStringList knownBackgrounds = {
        "#f6f8fb",
        "#eef6ff",
        "#f4fbf7",
        "#fff8ed",
        "#f7f2ff",
        "#eefcf8"
    };

    for (const QString &color : knownBackgrounds) {
        updatedStyle.replace("background-color: " + color,
                             "background-color: " + backgroundColor);
        updatedStyle.replace("background-color:" + color,
                             "background-color:" + backgroundColor);
    }

    if (!updatedStyle.contains("background-color"))
        updatedStyle.prepend("QWidget { background-color: " + backgroundColor + "; }\n");

    return updatedStyle;
}
