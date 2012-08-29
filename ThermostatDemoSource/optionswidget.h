#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QtGui>
#include "awayscreen.h"
#include "schedulescreen.h"
#include "settingscreen.h"

class OptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OptionsWidget(QWidget *parent = 0);
    
signals:
    void valueChanged();
    void cityChanged(QString);
    void currentTempChanged();

public slots:

private:
    QPushButton *settingsButton;
    QPushButton *calendarButton;
    QPushButton *awayButton;
    AwayScreen *awayScreen;
    ScheduleScreen *scheduleScreen;
    SettingScreen *settingScreen;
};

#endif // OPTIONSWIDGET_H
