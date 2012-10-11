#include "mainwindow.h"
#include "forecastdata.h"
#include "weatherwidget.h"
#include "thermostatwidget.h"
#include "optionswidget.h"
#include "remoteaccessmanager.h"
#include "settingscreen.h"
#include "globalsettings.h"
#include "weatherdata.h"
#include "utilities.h"

#include <QtGui>

#include <QtDebug>

/***********************************************************************************************************
* MainWindow
*   Main Widget that holds all the child widgets and handles their layouts. Also coordinates loading of
*   data from the web for the weather widget from the webdata class.
************************************************************************************************************/

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    // the MainWindow provides the container for the ThermostatWidget, WeatherWidget, and OptionsWidget
    // it is also provides much of the interface between WebData and the widgets that require web updates
    // it is set to initial hard-coded values first, and only updated from the web if internet options
    // have been allowed
    setObjectName("MainWindow");

    //get a handle to the global settings singleton
    m_globalSettings = GlobalSettings::getInstance();

    // sets initial weather background
    setStyleSheet("mainwindow {border-image: url(:/Images/background-sunny-very-few-clouds.JPG)}");

    // remove cursor (i.e. mouse pointer) throughout application
    //qApp->setOverrideCursor( QCursor( Qt::BlankCursor ) );

    //set a default temp for the thermostat
    m_currentThermostatTemp = 72;

    // create web data object
    webData = new WebData;

    //connect the signals for asynchronous data transfer
    connect(webData, SIGNAL(dataAvailable(WeatherData*)), this, SLOT(setWebData(WeatherData*)));
    connect(webData, SIGNAL(networkTimeout()), this, SLOT(webDataFailed()));

    // create weather widget
    weatherWidget = new WeatherWidget(this);
    connect(weatherWidget, SIGNAL(webReloadRequested()), this, SLOT(loadWebData()));

    // create thermostat widget
    thermostatWidget = new ThermostatWidget;
    thermostatWidget->setCurrentTempPtr(&m_currentThermostatTemp);

    // create options widget
    optionsWidget = new OptionsWidget;
    optionsWidget->awayScreen->setCurrentTempPtr(&m_currentThermostatTemp);
    optionsWidget->settingScreen->setLicenseString(webData->licenseString());

    // connect 10 second timer that causes current temp to follow setpoint to pass signal through options widget
    connect(thermostatWidget, SIGNAL(timeout()), optionsWidget, SIGNAL(currentTempChanged()));
    // connect new city name signal to the MainWindow function changeCity()
    connect(optionsWidget, SIGNAL(cityChanged()), this, SLOT(loadWebData()));
    connect(optionsWidget, SIGNAL(valueChanged()), webData, SLOT(configureProxy()));

    // connect Celsius/Fahrenheit change signals
    connect(optionsWidget, SIGNAL(valueChanged()), thermostatWidget, SLOT(updateUnit()));
    connect(optionsWidget, SIGNAL(valueChanged()), weatherWidget, SIGNAL(valueChanged()));
    connect(optionsWidget, SIGNAL(valueChanged()), this, SLOT(updateClock()));

    // make energy button change when setpoint is reached
    connect(thermostatWidget, SIGNAL(setpointIsReached(bool)),this,SLOT(energySaving(bool)));

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

    //layout all of the widgets we just instatiated
    createScreenLayout();

    //set clock to  local system time
    dateTime = QDateTime::currentDateTime();

    //start a timer to attempt to update the clock every second
    clockTimer = new QTimer(this);
    connect(clockTimer,SIGNAL(timeout()),this,SLOT(updateClock()));
    clockTimer->start(1000);


    //create and start the remote access manager
    RemoteAccessManager *m_remoteAccessManager = new RemoteAccessManager(this);
    m_remoteAccessManager->start();
    //connect(m_remoteAccessManager, SIGNAL(remoteChangeReceived(QHash<QString,QVariant>)), this, SLOT(processCommand(QHash<QString,QVariant>)));

    //first set based on local cache so user sees something!
    webData->loadLocalData();

    //call the changecity function which dispatches a network request for api weather data.
    loadWebData();
}

//FUNCTION: createScreenLayout
//
//  all code to generate the main layout for the MainWindow is here.
//

void MainWindow::createScreenLayout()
{
    // create left portion of the layout holding the options buttons and thermostat control
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(optionsWidget);
    leftLayout->addWidget(thermostatWidget);
    leftLayout->setStretchFactor(optionsWidget, 1);
    leftLayout->setStretchFactor(thermostatWidget, 3);

    // combine the previous layout with the weatherwidget and the close button
    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addSpacing(6);
    middleLayout->addLayout(leftLayout);
    middleLayout->addSpacing(6);
    middleLayout->addWidget(weatherWidget);
    middleLayout->addWidget(closeButton);
    middleLayout->addSpacing(6);
    middleLayout->setStretchFactor(leftLayout, 1);
    middleLayout->setStretchFactor(weatherWidget, 1);
    middleLayout->setAlignment(closeButton,Qt::AlignTop);

    //combine the previous nested layout with the date time and energy buttons
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(energyButton);
    bottomLayout->addWidget(dateButton);
    bottomLayout->addWidget(timeButton);
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);

    //it is easier to define size constraints for a widget so
    //we will encapsulate everything in sizeLimiterWidget

    QWidget *sizeLimiterWidget = new QWidget;
    sizeLimiterWidget->setMaximumSize(800, 450);
    sizeLimiterWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizeLimiterWidget->setLayout(middleLayout);
    QHBoxLayout* contentsBox = new QHBoxLayout;
    contentsBox->addWidget(sizeLimiterWidget);

    //build the main layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch(0);
    mainLayout->addLayout(contentsBox);
    mainLayout->setStretchFactor(contentsBox, 1);
    mainLayout->addStretch(0);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setAlignment(sizeLimiterWidget,Qt::AlignCenter);
    mainLayout->setAlignment(bottomLayout,Qt::AlignBottom);

    // show layout
    setLayout(mainLayout);
}

//FUNCTION: closeEvent
//
//  Reimplemented closeEvent handler just to allow the app to save the settings before exiting
//

void MainWindow::closeEvent(QCloseEvent *e)
{
    m_globalSettings->save();
    delete m_globalSettings;
    e->accept();
    qApp->quit();
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
    dateTime = QDateTime::currentDateTime();
    timeButton->setText(formatTimeString(dateTime.time(), m_globalSettings->timeFormat()));
    dateButton->setText(dateTime.date().toString("ddd, MMM d"));

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

//FUNCTION: changeCity
//
//  sends a web request through the webdata class to get weather data for the city
//  passed. returns immediately. the webdata class invokes setWebData slot on success
//  or webDataFailed otherwise.

void MainWindow::loadWebData()
{
    // get new city information and pass it to WebData to send
    // a new request and get updated information for that city
    weatherWidget->setStatusUpdating();
    webData->changeCity(m_globalSettings->currentCity());

}

//FUNCTION: setWebData
//
//  slot invoked by the webdata class on transfer of data across the network that handles
//  setting an appropriate background for the mainwidget and passing the parsed data along
//  to the weatherwidget

void MainWindow::setWebData(WeatherData* weatherData)
{

    if(!weatherData)
    {
        webDataFailed();
        return;
    }
    setBackground(weatherData->icon(), weatherData->localTime().time());
    weatherWidget->setWeatherData(weatherData);
    if(weatherData->cachedData())
        weatherWidget->setStatusFailed();
    else
        weatherWidget->setStatusUpdated();
}

//FUNCTION: webDataFailed
//
//  slot invoked by the webdata class on an unsuccessful attempt at fetching weather data
//  from the network. tells the weather widget to alert the user of the failure.

void MainWindow::webDataFailed()
{
    WeatherData *weatherData = weatherWidget->weatherData();

    //if the update fails, generate random data so things look nice!
    //easiest way to tell if the update failed and we have no cached data is
    //to compare the current configured city to the data's city

    if(weatherData->currentCity() != m_globalSettings->currentCity())
    {
        //generate a quick lookup for the icons
        QStringList icons;
        icons << "sunny" << "partlysunny" << "cloudy" << "rain" << "tstorms";

        //update the city to the current configured city
        weatherData->setCurrentCity(m_globalSettings->currentCity());

        //generate temperature numbers +/- 75
        weatherData->setCurrentTemp(75+(qrand()%10*qrand()%2*(-1)));

        //mod a random number by the number of icons then index into the array to pick one
        weatherData->setIcon(icons[qrand()%icons.size()]);

        //set the main page background to selected icon
        setBackground(weatherData->icon(), weatherData->localTime().time());

        //now iterate through the forecast days and do the same thing
        QList<ForecastData* >::iterator i;
        for(i=weatherData->forecastData().begin();i!=weatherData->forecastData().end();i++)
        {

            (*i)->setLowTemp(75+(qrand()%10*qrand()%2*(-1)));
            (*i)->setHighTemp((*i)->lowTemp()+qrand()%10);
            (*i)->setIcon(icons[qrand()%icons.size()]);
        }

        //set the data just to trigger an update of the widgets
        weatherWidget->setWeatherData(weatherData);
    }

    weatherWidget->setStatusFailed();
}

void MainWindow::setBackground(QString icon, QTime localTime)
{
    // set the MainWindow background based on the current conditions

    if (icon == "partlysunny" || icon == "mostlycloudy" ) {
        if(localTime.hour() < 5 || localTime.hour() > 20) {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_night_sky.jpeg)}");
        } else {
            setStyleSheet("MainWindow {border-image: url(:/Images/mostlycloudy.jpg)}");
        }
    }
    else if (icon == "fog") {
        setStyleSheet("MainWindow {border-image: url(:/Images/fog.jpg)}");
    }
    else if (icon == "hazy") {
        if(localTime.hour() < 5 || localTime.hour() > 20) {
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
        if(localTime.hour() < 5 || localTime.hour() > 20) {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_night_sky.jpeg)}");
        } else {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_sky.jpg)}");
        }
    }
    else if (icon == "mostlysunny" || icon == "partlycloudy" ||
             icon == "unknown") {
        if(localTime.hour() < 5 || localTime.hour() > 20) {
            setStyleSheet("MainWindow {border-image: url(:/Images/clear_night_sky.jpeg)}");
        } else {
            setStyleSheet("MainWindow {border-image: url(:/Images/background-sunny-very-few-clouds.JPG)}");
        }
    }
    else if (icon == "tstorms" || icon == "chancetstorms") {
        setStyleSheet("MainWindow {border-image: url(:/Images/storm.jpg)}");

    }

}

QHash<QString, QVariant> MainWindow::processCommand(QHash<QString, QVariant> command)
{
    if(command["command"] == "update" || command["command"] == "update_forced")
    {
        QVariantHash data;
        data.unite(weatherWidget->getCurrentData());
        data.unite(thermostatWidget->getCurrentData());
        return data;

    }
    else if(command["command"] == "change_temp")
    {
        if(command["dt"].toInt()>0)
            thermostatWidget->increaseTemp();
        else if(command["dt"].toInt()<0)
            thermostatWidget->decreaseTemp();
    }
    return QHash<QString, QVariant>();
}
