QT       += core gui mqtt network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    boardclient.cpp \
    devicewidget.cpp \
    form.cpp \
    homewidget.cpp \
    main.cpp \
    monitorwidget.cpp \
    networkwidget.cpp \
    registerwidget.cpp \
    settingswidget.cpp \
    usermanager.cpp \
    widget.cpp

HEADERS += \
    boardclient.h \
    devicewidget.h \
    form.h \
    homewidget.h \
    monitorwidget.h \
    networkwidget.h \
    registerwidget.h \
    settingswidget.h \
    usermanager.h \
    widget.h

FORMS += \
    devicewidget.ui \
    form.ui \
    homewidget.ui \
    monitorwidget.ui \
    networkwidget.ui \
    registerwidget.ui \
    settingswidget.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
