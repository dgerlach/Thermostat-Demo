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

    AwayScreen *awayScreen;
    ScheduleScreen *scheduleScreen;
    SettingScreen *settingScreen;
    
signals:
    void valueChanged();
    void cityChanged();
    void currentTempChanged();

public slots:

private:
    QPushButton *settingsButton;
    QPushButton *calendarButton;
    QPushButton *awayButton;

};

#endif // OPTIONSWIDGET_H
