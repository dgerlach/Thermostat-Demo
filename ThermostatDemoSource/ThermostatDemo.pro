#-------------------------------------------------
#
# Project created by QtCreator 2012-07-02T08:59:53
#
#-------------------------------------------------

QT       += core gui svg network script


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
    qkeyboardlineedit.cpp \
    weatherdata.cpp \
    forecastdata.cpp \
    forecastdatawidget.cpp \
    utilities.cpp \
    weatherdatawidget.cpp \
    webdataengine/webdataengine.cpp \
    webdataengine/openweathermapdataengine.cpp \
    webdataengine/wundergrounddataengine.cpp \
    remoteaccessmanager.cpp

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
    qkeyboardlineedit.h \
    weatherdata.h \
    forecastdata.h \
    forecastdatawidget.h \
    utilities.h \
    weatherdatawidget.h \
    main.h \
    webdataengine/webdataengine.h \
    webdataengine/openweathermapdataengine.h \
    webdataengine/wundergrounddataengine.h \
    remoteaccessmanager.h

RESOURCES += \
    thermostatdemo.qrc

RCC_DIR = Images/

FORMS += \
    keyboard/keyboard.ui


target.path += /home/root
INSTALLS += target

OTHER_FILES +=
