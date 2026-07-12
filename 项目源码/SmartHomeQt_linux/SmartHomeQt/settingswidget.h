#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

signals:
    void backToMain();
    void themeChanged(const QString &themeName, const QString &backgroundColor);

private slots:
    void on_btnBack_clicked();
    void on_comboBoxTheme_currentTextChanged(const QString &text);

private:
    QString backgroundColorForTheme(const QString &themeName) const;
    void updatePreview(const QString &themeName);
    void showMessage(const QString &message);

private:
    Ui::SettingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
