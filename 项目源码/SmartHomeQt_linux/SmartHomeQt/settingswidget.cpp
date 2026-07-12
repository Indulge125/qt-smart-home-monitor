#include "settingswidget.h"
#include "ui_settingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    setWindowTitle("智能家居系统 - 系统设置");

    ui->labelTitle->setText("系统设置界面");

    ui->comboBoxTheme->clear();
    ui->comboBoxTheme->addItem("默认浅灰");
    ui->comboBoxTheme->addItem("清爽蓝");
    ui->comboBoxTheme->addItem("护眼绿");
    ui->comboBoxTheme->addItem("暖阳米");
    ui->comboBoxTheme->addItem("淡雅紫");
    ui->comboBoxTheme->addItem("薄荷青");

    ui->textEditPreview->setReadOnly(true);

    updatePreview(ui->comboBoxTheme->currentText());
    showMessage("切换界面风格后，会立即修改整个系统的页面底色");
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::on_btnBack_clicked()
{
    emit backToMain();
}

void SettingsWidget::on_comboBoxTheme_currentTextChanged(const QString &themeName)
{
    updatePreview(themeName);
    showMessage("当前界面风格：" + themeName);
    emit themeChanged(themeName, backgroundColorForTheme(themeName));
}

QString SettingsWidget::backgroundColorForTheme(const QString &themeName) const
{
    if (themeName == "清爽蓝")
        return "#eef6ff";

    if (themeName == "护眼绿")
        return "#f4fbf7";

    if (themeName == "暖阳米")
        return "#fff8ed";

    if (themeName == "淡雅紫")
        return "#f7f2ff";

    if (themeName == "薄荷青")
        return "#eefcf8";

    return "#f6f8fb";
}

void SettingsWidget::updatePreview(const QString &themeName)
{
    QString text;

    text += "当前界面风格：" + themeName + "\n";
    text += "当前页面底色：" + backgroundColorForTheme(themeName) + "\n\n";
    text += "说明：\n";
    text += "1. 这里仅负责切换整个项目界面的底色。\n";
    text += "2. 左侧导航栏会随界面风格同步调整底色。\n";
    text += "3. 网络连接、监视界面、设备控制等业务逻辑不在这里配置。\n";
    text += "4. 修改后会立即作用到主窗口和已加载的功能页面。\n";

    ui->textEditPreview->setPlainText(text);
}

void SettingsWidget::showMessage(const QString &message)
{
    ui->labelMessage->setText(message);
    ui->labelMessage->setStyleSheet("color:#15803d;");
}
