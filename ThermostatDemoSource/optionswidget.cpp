#include "optionswidget.h"
#include "awayscreen.h"
#include "thermostatwidget.h"
#include "schedulescreen.h"
#include "settingscreen.h"

OptionsWidget::OptionsWidget(QWidget *parent) :
    QWidget(parent)
{
    int optionButtonIconSize = 50;
    // create settings button
    settingsButton = new QPushButton();
    settingsButton->setIcon(QIcon(":/Images/glossy-blue-orb-icon-gears.png"));
    settingsButton->setIconSize(QSize(optionButtonIconSize,optionButtonIconSize));
    settingsButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    settingsButton->setFocusPolicy(Qt::NoFocus);

    // create settings screen
    settingScreen = new SettingScreen;
    // when settings button is clicked, show settings screen
    connect(settingsButton,SIGNAL(clicked()),settingScreen,SLOT(showFullScreen()));
    // pass C/F value from settings screen to options widget and out to main window
    connect(settingScreen, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
    // pass new city string value from settings screen to options widget and out to main window
    connect(settingScreen, SIGNAL(cityChanged(QString)), this, SIGNAL(cityChanged(QString)));

    // create new calender button
    calendarButton = new QPushButton();
    calendarButton->setIcon(QIcon(":/Images/glossy-blue-orb-icon-calendar.png"));
    calendarButton->setIconSize(QSize(optionButtonIconSize,optionButtonIconSize));
    calendarButton->setFocusPolicy(Qt::NoFocus);
    calendarButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // create new schedule screen
    scheduleScreen = new ScheduleScreen;
    connect(calendarButton,SIGNAL(clicked()),scheduleScreen,SLOT(showFullScreen()));
    connect(settingScreen, SIGNAL(valueChanged()), scheduleScreen, SIGNAL(valueChanged()));

    // create new away button
    awayButton = new QPushButton();
    awayButton->setIcon(QIcon(":/Images/glossy-blue-orb-icon-airplane.png"));
    awayButton->setIconSize(QSize(optionButtonIconSize,optionButtonIconSize));
    awayButton->setFocusPolicy(Qt::NoFocus);
    awayButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // create new away screen
    awayScreen = new AwayScreen;
    connect(awayButton,SIGNAL(clicked()),awayScreen,SLOT(showFullScreen()));
    connect(this, SIGNAL(currentTempChanged()), awayScreen, SLOT(updateCurrentTemp()));
    connect(settingScreen, SIGNAL(valueChanged()), awayScreen, SLOT(updateUnit()));

    // create layout
    QHBoxLayout *optionsLayout = new QHBoxLayout;
    optionsLayout->addWidget(settingsButton);
    optionsLayout->addWidget(calendarButton);
    optionsLayout->addWidget(awayButton);
    optionsLayout->setSpacing(0);

    // show layout
    setLayout(optionsLayout);

}
