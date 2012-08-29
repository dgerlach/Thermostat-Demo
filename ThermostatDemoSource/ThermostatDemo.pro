#-------------------------------------------------
#
# Project created by QtCreator 2012-07-02T08:59:53
#
#-------------------------------------------------

QT       += core gui svg network


TARGET = ThermostatDemo
TEMPLATE = app

SOURCES += main.cpp \
    weatherwidget.cpp \
    thermostatwidget.cpp \
    optionswidget.cpp \
    webdata.cpp \
    awayscreen.cpp \
    mainwindow.cpp \
    schedulescreen.cpp \
    schedulepoint.cpp \
    keyboard/keyboard.cpp \
    settingscreen.cpp \
    globalsettings.cpp \
    qkeyboardlineedit.cpp

HEADERS  += \
    weatherwidget.h \
    thermostatwidget.h \
    optionswidget.h \
    webdata.h \
    awayscreen.h \
    mainwindow.h \
    schedulescreen.h \
    schedulepoint.h \
    keyboard/keyboard.h \
    settingscreen.h \
    STARTHERE.h \
    globalsettings.h \
    qkeyboardlineedit.h

RESOURCES += \
    thermostatdemo.qrc

RCC_DIR = Images/

FORMS += \
    keyboard/keyboard.ui


target.path += /home/root
INSTALLS += target
