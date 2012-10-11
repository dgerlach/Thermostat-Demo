#include "weatherwidget.h"

#include "forecastdata.h"
#include "forecastdatawidget.h"
#include "globalsettings.h"
#include "weatherdata.h"
#include "weatherdatawidget.h"
#include "utilities.h"

WeatherWidget::WeatherWidget(QWidget *parent) :
        QWidget(parent)
{
    //grab an instance of the global settings
    m_globalSettings = GlobalSettings::getInstance();

    setObjectName("weatherWidget");
    setMaximumWidth(parent->width()/1.7);
    setAttribute(Qt::WA_StyledBackground,true);

    m_weatherData = NULL;
    // set initial clock time in case no web access
    clock = new QTime(16,18);
    weatherDataWidget = new WeatherDataWidget(this);
    connect(this, SIGNAL(valueChanged()), weatherDataWidget, SLOT(updateData()));

    forecastDataWidget = new ForecastDataWidget(this);
    connect(this, SIGNAL(valueChanged()), forecastDataWidget, SLOT(updateData()));
    connect(this, SIGNAL(valueChanged()), this, SLOT(updateData()));

    buildStatusWidget();

    // create final layout
    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(weatherDataWidget, 3);
    mainLayout->addWidget(forecastDataWidget, 5);

    mainLayout->setSpacing(1);
    mainLayout->addWidget(statusStackedWidget, 0);
    mainLayout->setAlignment(statusStackedWidget, Qt::AlignBottom);

    // show layout
    setLayout(mainLayout);
    m_currentStatus = WeatherWidget::Updating;
}

void WeatherWidget::buildStatusWidget()
{
    //create status layout for the first status string
    statusLabel = new QLabel("Connecting...");
    statusLabel->setObjectName("statusLabel");
    statusMovieLabel = new QLabel;
    statusMovie = new QMovie(":/Images/ajax-loader.gif");
    statusMovieLabel->setMovie(statusMovie);
    statusMovie->start();

    QHBoxLayout *statusLayoutPage1 = new QHBoxLayout;
    statusLayoutPage1->addStretch();
    statusLayoutPage1->addWidget(statusMovieLabel);
    statusLayoutPage1->addWidget(statusLabel);
    statusLayoutPage1->addStretch();
    statusLayoutPage1->setContentsMargins(0,0,0,0);

    //container for a centered label and status image so it can be added
    //to the stacked widget
    QWidget* statusWidgetPage1 = new QWidget;
    statusWidgetPage1->setLayout(statusLayoutPage1);

    //create the status layout for the second status string
    statusLabel2 = new QLabel("Connecting...");
    statusLabel2->setObjectName("statusLabel2");

    QHBoxLayout *statusLayoutPage2 = new QHBoxLayout;
    statusLayoutPage2->addStretch();
    statusLayoutPage2->addWidget(statusLabel2);
    statusLayoutPage2->addStretch();
    statusLayoutPage2->setContentsMargins(0,0,0,0);

    //container for a centered label and status image so it can be added
    //to the stacked widget
    QWidget* statusWidgetPage2 = new QWidget;
    statusWidgetPage2->setLayout(statusLayoutPage2);

    //create the stacked widget so we can alternate between messages if they're too
    //long to fit on a single screen
    statusStackedWidget = new QStackedWidget;
    connect(&statusMessageTimer, SIGNAL(timeout()), this, SLOT(advanceStatusWidget()));

    statusStackedWidget->addWidget(statusWidgetPage1);
    statusStackedWidget->addWidget(statusWidgetPage2);

    statusStackedWidget->setContentsMargins(0,0,0,0);
}

void WeatherWidget::setCity(QString city)
{
    // set city from string passed form setting screen
    // format city string correctly to display properly
    QStringList cityList = city.split(",");
    QString cityShort = cityList.at(0);
    if(cityShort.size() > 9) {
        cityShort.resize(9);
    }
    QString countryShort;

    if(cityList.at(1).size() < 3) {
        countryShort = cityList.at(1);
    } else if (cityList.at(1) == "Japan") {
        countryShort.append("JP");
    } else if (cityList.at(1) == "South Korea") {
        countryShort.append("KR");
    } else if (cityList.at(1) == "Mexico") {
        countryShort.append("MX");
    } else if (cityList.at(1) == "India") {
        countryShort.append("IN");
    } else if (cityList.at(1) == "Indonesia") {
        countryShort.append("ID");
    } else if (cityList.at(1) == "Brazil") {
        countryShort.append("BR");
    } else if (cityList.at(1) == "China") {
        countryShort.append("CN");
    } else if (cityList.at(1) == "Germany") {
        countryShort.append("DE");
    }else if (cityList.at(1) == "Russia") {
        countryShort.append("RU");
    } else if (cityList.at(1) == "Egypt") {
        countryShort.append("EG");
    } else if (cityList.at(1) == "South Africa") {
        countryShort.append("ZA");
    } else if (cityList.at(1) == "Taiwan") {
        countryShort.append("TW");
    }



}

void WeatherWidget::setWeatherData(WeatherData *weatherData)
{
    m_weatherData = weatherData;

    weatherDataWidget->setData(weatherData);
    weatherDataWidget->updateData();

    forecastDataWidget->setData(weatherData->forecastData());
    forecastDataWidget->updateData();
}

WeatherData * WeatherWidget::weatherData()
{
    return m_weatherData;
}

void WeatherWidget::setStatusUpdated()
{
    statusMessageTimer.stop();

    //start on the first status message
    statusStackedWidget->setCurrentIndex(0);

    statusMovieLabel->setVisible(false);
    statusMovie->stop();
    if(m_weatherData)
    {
        QString updatedString = m_weatherData->lastUpdated().toString("MMM d 'at' ") + formatTimeString(m_weatherData->lastUpdated().time(), m_globalSettings->timeFormat());
        statusLabel->setText("Updated " + updatedString);
    }
    else
        statusLabel->setText("Updated");

    m_currentStatus = WeatherWidget::UpdateSuccess;
}

void WeatherWidget::setStatusUpdating()
{
    statusMessageTimer.stop();

    //start on the first status message
    statusStackedWidget->setCurrentIndex(0);

    statusMovieLabel->setVisible(true);
    statusMovie->start();
    statusLabel->setText("Updating...");

    m_currentStatus = WeatherWidget::Updating;
}

void WeatherWidget::setStatusFailed()
{
    //return if were already in a failed state
    if(m_currentStatus == WeatherWidget::UpdateFailed)
        return;

    //start on the first status message
    statusStackedWidget->setCurrentIndex(0);

    //stop the updating movie and hide it
    statusMovie->stop();
    statusMovieLabel->setVisible(false);

    QString updatedString;

    //generate a string to say when the last update occurred
    if(m_weatherData)
        updatedString = m_weatherData->lastUpdated().toString("MMM d ") + formatTimeString(m_weatherData->lastUpdated().time(), m_globalSettings->timeFormat());
    else
        updatedString = "Never";

    statusLabel->setText("Update Failed");
    statusLabel2->setText("Last Update " + updatedString);

    //switch between failure message and last update message every 3 seconds
    statusMessageTimer.start(3000);

    m_currentStatus = WeatherWidget::UpdateFailed;
}

void WeatherWidget::advanceStatusWidget()
{
    //add one to the current widget and mod 2 to toggle between the two pages
    statusStackedWidget->setCurrentIndex((statusStackedWidget->currentIndex()+1)%2);
}

void WeatherWidget::updateData()
{
    if(m_currentStatus == WeatherWidget::UpdateSuccess)
        setStatusUpdated();
}

void WeatherWidget::mousePressEvent(QMouseEvent *e)
{
    if(m_currentStatus == WeatherWidget::UpdateFailed)
        emit(webReloadRequested());
    else if(m_currentStatus == WeatherWidget::UpdateSuccess && m_weatherData->lastUpdated().addSecs(10) < QDateTime::currentDateTime())
        emit webReloadRequested();
    e->accept();
}

QVariantHash WeatherWidget::getCurrentData()
{
    QVariantHash data;
    data.insert("weatherCurrent", weatherDataWidget->getCurrentData());
    data.insert("weatherForecast", forecastDataWidget->getCurrentData());
    return data;
}
