#include "monitorwidget.h"
#include "ui_monitorwidget.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

MonitorWidget::MonitorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonitorWidget)
{
    ui->setupUi(this);

    setWindowTitle("智能家居系统 - 板子监视");

    initUi();
    showSimulatedBoardData();
}

MonitorWidget::~MonitorWidget()
{
    delete ui;
}

void MonitorWidget::initUi()
{
    ui->labelTitle->setText("板子实时数据监视");

    ui->lineEditTemp->setReadOnly(true);
    ui->lineEditHum->setReadOnly(true);
    ui->lineEditLight->setReadOnly(true);
    ui->lineEditBeeper->setReadOnly(true);
    ui->lineEditFan->setReadOnly(true);
    ui->lineEditLed->setReadOnly(true);
    ui->lineEditUpdateTime->setReadOnly(true);
    ui->textEditJson->setReadOnly(true);

    ui->lineEditTemp->setText("-- ℃");
    ui->lineEditHum->setText("-- %");
    ui->lineEditLight->setText("--");
    ui->lineEditBeeper->setText("未知");
    ui->lineEditFan->setText("未知");
    ui->lineEditLed->setText("未知");
    ui->lineEditUpdateTime->setText("--");
}

void MonitorWidget::on_btnBack_clicked()
{
    emit backToMain();
}

void MonitorWidget::on_btnRefresh_clicked()
{
   emit requestBoardData();
}

void MonitorWidget::on_btnClear_clicked()
{
    ui->lineEditTemp->clear();
    ui->lineEditHum->clear();
    ui->lineEditLight->clear();
    ui->lineEditBeeper->clear();
    ui->lineEditFan->clear();
    ui->lineEditLed->clear();
    ui->lineEditUpdateTime->clear();
    ui->textEditJson->clear();
}

void MonitorWidget::updateFromBoardJson(const QString &message)
{
    showBoardJson(message);
}

void MonitorWidget::showSimulatedBoardData()
{
    QString json =
            "{"
            "\"temperature\":26.5,"
            "\"humidity\":61.2,"
            "\"light\":386,"
            "\"beeper\":false,"
            "\"fan\":true,"
            "\"led\":true"
            "}";

    showBoardJson(json);
}

void MonitorWidget::showBoardJson(const QString &message)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        ui->textEditJson->setPlainText(message);
        ui->lineEditUpdateTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("temperature"))
        ui->lineEditTemp->setText(QString::number(obj.value("temperature").toDouble()) + " ℃");

    if (obj.contains("temp"))
        ui->lineEditTemp->setText(QString::number(obj.value("temp").toDouble()) + " ℃");

    if (obj.contains("humidity"))
        ui->lineEditHum->setText(QString::number(obj.value("humidity").toDouble()) + " %");

    if (obj.contains("hum"))
        ui->lineEditHum->setText(QString::number(obj.value("hum").toDouble()) + " %");

    if (obj.contains("light"))
        ui->lineEditLight->setText(QString::number(obj.value("light").toDouble()));

    if (obj.contains("beeper"))
        ui->lineEditBeeper->setText(boolState(obj.value("beeper").toBool()));

    if (obj.contains("fan"))
        ui->lineEditFan->setText(boolState(obj.value("fan").toBool()));

    if (obj.contains("led"))
        ui->lineEditLed->setText(boolState(obj.value("led").toBool()));

    ui->lineEditUpdateTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    QJsonDocument prettyDoc(obj);
    ui->textEditJson->setPlainText(QString::fromUtf8(prettyDoc.toJson(QJsonDocument::Indented)));
}

QString MonitorWidget::boolState(bool value) const
{
    if (value)
        return "已打开";

    return "已关闭";
}
