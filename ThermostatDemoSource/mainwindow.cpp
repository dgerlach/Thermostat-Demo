#include "mainwindow.h"
#include "weatherwidget.h"
#include "thermostatwidget.h"
#include "optionswidget.h"
#include "settingscreen.h"
#include "globalsettings.h"

#include <QtGui>

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{

    setObjectName("MainWindow");
    // the MainWindow provides the container for the ThermostatWidget, WeatherWidget, and OptionsWidget
    // it is also provides much of the interface between WebData and the widgets that require web updates
    // it is set to initial hard-coded values first, and only updated from the web if internet options
    // have been allowed

    m_globalSettings = GlobalSettings::getInstance();

    // sets initial weather background
    setStyleSheet("mainwindow {border-image: url(:/Images/background-sunny-very-few-clouds.JPG)}");

    // remove cursor (i.e. mouse pointer) throughout application
    qApp->setOverrideCursor( QCursor( Qt::BlankCursor ) );

    // create weather widget
    weatherWidget = new WeatherWidget;
    weatherWidget->setObjectName("weatherWidget");
    weatherWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    weatherWidget->setAttribute(Qt::WA_StyledBackground,true);

    // create thermostat widget
    ThermostatWidget *thermostatWidget = new ThermostatWidget;
    thermostatWidget->setObjectName("thermostatWidget");
    thermostatWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    thermostatWidget->setAttribute(Qt::WA_StyledBackground,true);

    // create options widget
    OptionsWidget *optionsWidget = new OptionsWidget;
    optionsWidget->setObjectName("optionsWidget");
    optionsWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    optionsWidget->setAttribute(Qt::WA_StyledBackground,true);
    // connect 10 second timer that causes current temp to follow setpoint to pass signal through options widget
    connect(thermostatWidget, SIGNAL(timeout()), optionsWidget, SIGNAL(currentTempChanged()));
    // connect new city name signal to the MainWindow function changeCity()
    connect(optionsWidget, SIGNAL(cityChanged(QString)), this, SLOT(changeCity(QString)));

    // create energy button
    energyButton = new QPushButton("48kW");
    energyButton->setObjectName("energyButton");
    energyButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    energyButton->setAttribute(Qt::WA_StyledBackground,true);
    energyButton->setFocusPolicy(Qt::NoFocus);

    // create time button
    timeButton = new QPushButton("4:18PM");
    timeButton->setObjectName("timeButton");
    timeButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    timeButton->setAttribute(Qt::WA_StyledBackground);
    timeButton->setFocusPolicy(Qt::NoFocus);

    // create date button
    dateButton = new QPushButton("Mon, Jun 17");
    dateButton->setObjectName("dateButton");
    dateButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    dateButton->setAttribute(Qt::WA_StyledBackground);
    dateButton->setFocusPolicy(Qt::NoFocus);

    // create close button
    closeButton = new QPushButton();
    closeButton->setIcon(QIcon(":/Images/simple-red-square-icon-x.png"));
    closeButton->setIconSize(QSize(20,20));
    closeButton->setObjectName("closeButton");
    closeButton->setFocusPolicy(Qt::NoFocus);
    connect(closeButton,SIGNAL(clicked()),this, SLOT(close()));

    // create screen layout
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addStretch();
    leftLayout->addWidget(optionsWidget);
    leftLayout->addWidget(thermostatWidget);
    leftLayout->addStretch();
    leftLayout->setAlignment(optionsWidget,Qt::AlignCenter);

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addStretch();
    middleLayout->addLayout(leftLayout);
    middleLayout->addSpacing(6);
    middleLayout->addWidget(weatherWidget);
    middleLayout->addStretch();
    middleLayout->addWidget(closeButton);
    middleLayout->addSpacing(6);
    middleLayout->setAlignment(closeButton,Qt::AlignTop);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(energyButton);
    bottomLayout->addWidget(dateButton);
    bottomLayout->addWidget(timeButton);
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(middleLayout);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setContentsMargins(0,6,0,0);
    mainLayout->setAlignment(bottomLayout,Qt::AlignBottom);

    // connect Celsius/Fahrenheit change signals
    connect(optionsWidget, SIGNAL(valueChanged()), this, SLOT(updateUnit()));
    connect(optionsWidget, SIGNAL(valueChanged()), thermostatWidget, SLOT(updateUnit()));

    // show layout
    setLayout(mainLayout);

    // create web data object and set all dynamic properties based on web data
    webData = new WebData;

    //first set based on local cache so user sees something!
    webData->loadLocalData();
    getWebData();

    clockTimer = new QTimer(this);
    connect(clockTimer,SIGNAL(timeout()),this,SLOT(updateClock()));
    clockTimer->start(60000); // makes the clock tick every 60 seconds

    changeCity(m_globalSettings->currentCity());


    // make energy button change when setpoint is reached
    connect(thermostatWidget, SIGNAL(setpointIsReached(bool)),this,SLOT(energySaving(bool)));
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    m_globalSettings->save();
    delete m_globalSettings;
    e->accept();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    /* Applying CSS styles to custom widgets inherited from QWidget
    requires reimplementing paintEvent(). Without doing it, custom
    widgets will support only the background, background-clip,
    and background-origin properties.  */

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWindow::updateClock()
{
    // make clock tick
    clock = clock.addSecs(60);
    timeButton->setText(clock.toString("h:mm AP"));

}

void MainWindow::setBackground(QString icon)
{
    // set the MainWindow background based on the current conditions

    if (icon == "partlysunny" || icon == "mostlycloudy" ) {
        if(clock.hour() < 5 || clock.hour() > 20) {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_night_sky.jpeg)}");
        } else {
            setStyleSheet("MainWindow {border-image: url(:/Images/mostlycloudy.jpg)}");
        }
    }
    else if (icon == "fog") {
        setStyleSheet("MainWindow {border-image: url(:/Images/fog.jpg)}");
    }
    else if (icon == "hazy") {
        if(clock.hour() < 5 || clock.hour() > 20) {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_night_sky.jpeg)}");
        } else {
            setStyleSheet("MainWindow {border-image: url(:/Images/hazy.jpg)}");
        }
    }
    else if (icon == "cloudy") {
        setStyleSheet("MainWindow {border-image: url(:/Images/overcast.jpg)}");
    }
    else if (icon == "rain" || icon == "chancerain") {
        setStyleSheet("MainWindow {border-image: url(:/Images/rain.jpg)}");
    }
    else if (icon == "sleet" || icon == "chancesleet") {
        setStyleSheet("MainWindow {border-image: url(:/Images/sleet.jpg)}");
    }
    else if (icon == "flurries" || icon == "snow" ||
             icon == "chanceflurries" || icon == "chancesnow") {
        setStyleSheet("MainWindow {border-image: url(:/Images/snow.jpg)}");
    }
    else if (icon == "clear" || icon == "sunny") {
        if(clock.hour() < 5 || clock.hour() > 20) {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_night_sky.jpeg)}");
        } else {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_sky.jpg)}");
        }
    }
    else if (icon == "mostlysunny" || icon == "partlycloudy" ||
             icon == "unknown") {
        if(clock.hour() < 5 || clock.hour() > 20) {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_night_sky.jpeg)}");
        } else {
            setStyleSheet("MainWindow {border-image: url(:/Images/background-sunny-very-few-clouds.JPG)}");
        }
    }
    else if (icon == "tstorms" || icon == "chancetstorms") {
        setStyleSheet("MainWindow {border-image: url(:/Images/storm.jpg)}");

    }

}

void MainWindow::energySaving(bool setpointReached)
{
    // change energyButton based on whether or not setpoint is reached
    if(setpointReached == true) {
        energyButton->setStyleSheet("background: green;");
        energyButton->setText("48kW");
    }
    else if(setpointReached == false) {
        energyButton->setStyleSheet("background: red;");
        energyButton->setText("64kW");
    }

}

void MainWindow::updateUnit()
{
    // provide Celius/Fahrenheit conversions for weather widget
    int FInt, CInt;
    float FFloat, CFloat;
    QString FString, CString;

    if(m_globalSettings->temperatureFormat() == GlobalSettings::TempFormatF) {
        // true means F
        // we want Fahrenheit, but currently in Celsius
        CString = weatherWidget->getCurrentTemp();
        CString.chop(1);
        CFloat = CString.toFloat();
        FInt = (int)((CFloat*1.8)+32);
        FString = QString::number(FInt);
        weatherWidget->setCurrentTemp(FString);

        CString = weatherWidget->getDay1Low();
        CString.chop(1);
        CFloat = CString.toFloat();
        FInt = (int)((CFloat*1.8)+32);
        FString = QString::number(FInt);
        weatherWidget->setDay1Low(FString);

        CString = weatherWidget->getDay2Low();
        CString.chop(1);
        CFloat = CString.toFloat();
        FInt = (int)((CFloat*1.8)+32);
        FString = QString::number(FInt);
        weatherWidget->setDay2Low(FString);

        CString = weatherWidget->getDay3Low();
        CString.chop(1);
        CFloat = CString.toFloat();
        FInt = (int)((CFloat*1.8)+32);
        FString = QString::number(FInt);
        weatherWidget->setDay3Low(FString);

        CString = weatherWidget->getDay1High();
        CString.chop(1);
        CFloat = CString.toFloat();
        FInt = (int)((CFloat*1.8)+32);
        FString = QString::number(FInt);
        weatherWidget->setDay1High(FString);

        CString = weatherWidget->getDay2High();
        CString.chop(1);
        CFloat = CString.toFloat();
        FInt = (int)((CFloat*1.8)+32);
        FString = QString::number(FInt);
        weatherWidget->setDay2High(FString);

        CString = weatherWidget->getDay3High();
        CString.chop(1);
        CFloat = CString.toFloat();
        FInt = (int)((CFloat*1.8)+32);
        FString = QString::number(FInt);
        weatherWidget->setDay3High(FString);



    } else {
        // false means C
        // we want Celsius, but currently in Fahrenheit
        FString = weatherWidget->getCurrentTemp();
        FString.chop(1); // removes degree symbol at end
        FFloat = FString.toFloat();
        CInt = (int)((FFloat - 32) * 5)/9;
        CString = QString::number(CInt);
        weatherWidget->setCurrentTemp(CString);

        FString = weatherWidget->getDay1Low();
        FString.chop(1); // removes degree symbol at end
        FFloat = FString.toFloat();
        CInt = (int)((FFloat - 32) * 5)/9;
        CString = QString::number(CInt);
        weatherWidget->setDay1Low(CString);

        FString = weatherWidget->getDay2Low();
        FString.chop(1); // removes degree symbol at end
        FFloat = FString.toFloat();
        CInt = (int)((FFloat - 32) * 5)/9;
        CString = QString::number(CInt);
        weatherWidget->setDay2Low(CString);

        FString = weatherWidget->getDay3Low();
        FString.chop(1); // removes degree symbol at end
        FFloat = FString.toFloat();
        CInt = (int)((FFloat - 32) * 5)/9;
        CString = QString::number(CInt);
        weatherWidget->setDay3Low(CString);

        FString = weatherWidget->getDay1High();
        FString.chop(1); // removes degree symbol at end
        FFloat = FString.toFloat();
        CInt = (int)((FFloat - 32) * 5)/9;
        CString = QString::number(CInt);
        weatherWidget->setDay1High(CString);

        FString = weatherWidget->getDay2High();
        FString.chop(1); // removes degree symbol at end
        FFloat = FString.toFloat();
        CInt = (int)((FFloat - 32) * 5)/9;
        CString = QString::number(CInt);
        weatherWidget->setDay2High(CString);

        FString = weatherWidget->getDay3High();
        FString.chop(1); // removes degree symbol at end
        FFloat = FString.toFloat();
        CInt = (int)((FFloat - 32) * 5)/9;
        CString = QString::number(CInt);
        weatherWidget->setDay3High(CString);
    }
}

void MainWindow::changeCity(QString city)
{
    qDebug() << city;
    // get new city information and pass it to WebData to send
    // a new request and get updated information for that city
    weatherWidget->setCity(city);
    weatherWidget->setStatusUpdating();
    webData->changeCity(city);
    connect(webData, SIGNAL(dataAvailable()), this, SLOT(getWebData()));
    connect(webData, SIGNAL(networkTimeout()), this, SLOT(webDataFailed()));
}

void MainWindow::getWebData()
{
    // pull a new set of important web information
    dateButton->setText(webData->date());
    timeButton->setText(webData->time());
    clock = webData->clockObject();

    setBackground(webData->currentIcon());

    weatherWidget->setClock(clock);
    weatherWidget->setCurrentTemp(webData->temp());
    weatherWidget->setCurrentIcon(webData->currentIcon());
    weatherWidget->setDay1Icon(webData->day1Icon());
    weatherWidget->setDay2Icon(webData->day2Icon());
    weatherWidget->setDay3Icon(webData->day3Icon());
    weatherWidget->setDay1(webData->day1());
    weatherWidget->setDay2(webData->day2());
    weatherWidget->setDay3(webData->day3());
    weatherWidget->setDay1High(webData->day1High());
    weatherWidget->setDay1Low(webData->day1Low());
    weatherWidget->setDay2High(webData->day2High());
    weatherWidget->setDay2Low(webData->day2Low());
    weatherWidget->setDay3High(webData->day3High());
    weatherWidget->setDay3Low(webData->day3Low());
    weatherWidget->setStatusUpdated();
}

void MainWindow::webDataFailed()
{
    weatherWidget->setStatusFailed();
}
