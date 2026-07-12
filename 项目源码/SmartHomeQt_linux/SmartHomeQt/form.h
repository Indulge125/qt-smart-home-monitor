#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "homewidget.h"
#include "networkwidget.h"
#include "monitorwidget.h"
#include "devicewidget.h"
#include "settingswidget.h"
#include "boardclient.h"
class QPushButton;
namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();

    void setCurrentUser(const QString &username);

signals:
    void logoutRequested();

private slots:
    void on_btnLogout_clicked();
    void on_btnHome_clicked();
    void on_btnNetwork_clicked();
    void on_btnMonitor_clicked();
    void on_btnDevice_clicked();
    void on_btnSettings_clicked();

private:
    Ui::Form *ui;
    HomeWidget *homeWidget;
    NetworkWidget *networkWidget;
    MonitorWidget *monitorWidget;
    DeviceWidget *deviceWidget;
    SettingsWidget *settingsWidget;
    BoardClient *boardClient;
    void switchPage(QWidget *page, QPushButton *button);
    void setActiveButton(QPushButton *button);
    void applyTheme(const QString &themeName, const QString &backgroundColor);
    QString sidebarColorForTheme(const QString &themeName) const;
    QString recolorStyleSheet(const QString &styleSheet, const QString &backgroundColor) const;
};

#endif // FORM_H
