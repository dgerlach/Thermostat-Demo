#include "weatherwidget.h"

#include "forecastdata.h"
#include "forecastdatawidget.h"
#include "weatherdata.h"
#include "weatherdatawidget.h"

WeatherWidget::WeatherWidget(QWidget *parent) :
        QWidget(parent)
{

    setObjectName("weatherWidget");
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setAttribute(Qt::WA_StyledBackground,true);

    m_weatherData = NULL;
    // set initial clock time in case no web access
    clock = new QTime(16,18);
    weatherDataWidget = new WeatherDataWidget(this);
    connect(this, SIGNAL(valueChanged()), weatherDataWidget, SLOT(updateData()));

    for(int a =0; a< 3; a++)
    {
        forecastDataWidget[a] = new ForecastDataWidget(this);
        connect(this, SIGNAL(valueChanged()), forecastDataWidget[a], SLOT(updateData()));
    }


    //create status layout
    statusMovieLabel = new QLabel;
    statusMovie = new QMovie(":/Images/ajax-loader.gif");
    statusMovieLabel->setMovie(statusMovie);
    statusLabel = new QLabel("Connecting...");
    statusLabel->setObjectName("statusLabel");
    statusMovie->start();
    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->addWidget(statusMovieLabel);
    statusLayout->addWidget(statusLabel);

    // create final layout
    QVBoxLayout *mainLayout = new QVBoxLayout;

    //mainLayout->addLayout(currentHLayout);
    mainLayout->addWidget(weatherDataWidget, 2);
    for(int a =0;a<3; a++)
        mainLayout->addWidget(forecastDataWidget[a], 1);
    mainLayout->setSpacing(1);
    mainLayout->addLayout(statusLayout, 0);
    mainLayout->setAlignment(statusLayout, Qt::AlignBottom);

    qDebug() << "WW HIEHGT" << height();
    // show layout
    setLayout(mainLayout);
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
    countryShort.clear();
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

    for(int a=1;a<=3;a++)
    {
        forecastDataWidget[a-1]->setData(weatherData->forecastData().at(a));
        forecastDataWidget[a-1]->updateData();
    }

}

void WeatherWidget::setStatusUpdated()
{
    statusMovieLabel->setVisible(false);
    statusMovie->stop();
    if(m_weatherData)
        statusLabel->setText("Updated on " + m_weatherData->lastUpdated().toString("MMM d 'at' h:mm AP"));
    else
        statusLabel->setText("Updated");
}

void WeatherWidget::setStatusUpdating()
{
    //statusMovie->setFileName(":/Images/ajax-loader.gif");
    statusMovieLabel->setVisible(true);
    statusMovie->start();
    statusLabel->setText("Updating...");
}

void WeatherWidget::setStatusFailed()
{
    statusMovie->stop();
    statusMovieLabel->setVisible(false);
    statusLabel->setText("Update Failed");
}
