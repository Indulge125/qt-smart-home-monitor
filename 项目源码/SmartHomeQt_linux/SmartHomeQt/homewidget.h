#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>

namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

    void updateMqttState(const QString &state);
    void updateBoardState(const QString &state);
    void updateFromBoardJson(const QString &json);
    void updateBoardDeviceState(const QString &device, int value);

signals:
    void backToMain();

private slots:
    void on_btnBack_clicked();

private:
    void showMessage(const QString &message);
    QString boolState(bool value) const;

private:
    Ui::HomeWidget *ui;
};

#endif // HOMEWIDGET_H
