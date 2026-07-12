#include "widget.h"
#include "form.h"
#include "registerwidget.h"

#include <QApplication>
#include <QPointer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget loginWidget;
    RegisterWidget registerWidget;
    QPointer<Form> mainForm = nullptr;

    QObject::connect(&loginWidget, &Widget::loginSuccess,
                     [&loginWidget, &registerWidget, &mainForm](const QString &username) {
        if (mainForm.isNull()) {
            mainForm = new Form;

            QObject::connect(mainForm, &Form::logoutRequested,
                             [&loginWidget, &registerWidget, &mainForm]() {
                if (!mainForm.isNull())
                    mainForm->hide();

                registerWidget.hide();
                loginWidget.show();
            });
        }

        mainForm->setCurrentUser(username);
        mainForm->show();
        registerWidget.hide();
        loginWidget.hide();
    });

    QObject::connect(&loginWidget, &Widget::goRegister,
                     [&loginWidget, &registerWidget]() {
        registerWidget.show();
        loginWidget.hide();
    });

    QObject::connect(&registerWidget, &RegisterWidget::backToLogin,
                     [&loginWidget, &registerWidget]() {
        loginWidget.show();
        registerWidget.hide();
    });

    loginWidget.show();

    return a.exec();
}
