#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include <QWidget>

namespace Ui {
class MonitorWidget;
}

class MonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorWidget(QWidget *parent = nullptr);
    ~MonitorWidget();

    void updateFromBoardJson(const QString &message);

signals:
    void backToMain();
    void requestBoardData();
private slots:
    void on_btnBack_clicked();
    void on_btnRefresh_clicked();
    void on_btnClear_clicked();

private:
    void initUi();
    void showSimulatedBoardData();
    void showBoardJson(const QString &message);
    QString boolState(bool value) const;

private:
    Ui::MonitorWidget *ui;
};

#endif // MONITORWIDGET_H
