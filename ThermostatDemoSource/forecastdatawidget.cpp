#include "forecastdatawidget.h"

#include "forecastdata.h"
#include "utilities.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QTimer>

#include <QtDebug>

ForecastDataWidget::ForecastDataWidget(QWidget *parent) :
    QWidget(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    m_highTemp = new QLabel("");
    m_lowTemp = new QLabel("");
    m_day = new QLabel("");
    m_iconHolderLabel= new QLabel();
    m_iconPixmap = new QPixmap();
    m_iconHolderLabel->setPixmap(*m_iconPixmap);

    m_day->setMargin(0);

    widgetLayout = new QHBoxLayout;
    widgetLayout->addWidget(m_day);
    widgetLayout->addStretch();
    widgetLayout->addWidget(m_iconHolderLabel);
    widgetLayout->addStretch();
    widgetLayout->addWidget(m_lowTemp);
    widgetLayout->addSpacing(3);
    widgetLayout->addWidget(m_highTemp);

    widgetLayout->setSpacing(1);
    widgetLayout->setContentsMargins(5,1,5,1);

    setStyleSheet("color:#ffffff;background-color: none;padding:0px;");
    setLayout(widgetLayout);
    QTimer::singleShot(0, this, SLOT(scaleContents()));

}

void ForecastDataWidget::setData(ForecastData *data)
{
    m_data = data;
}

void ForecastDataWidget::updateData()
{
    m_highTemp->setText(formatTemperatureString(m_data->highTemp(), m_globalSettings->temperatureFormat()));
    m_lowTemp->setText(formatTemperatureString(m_data->lowTemp(), m_globalSettings->temperatureFormat()));
    m_day->setText(m_data->day());
    m_iconPixmap->load(iconNameToPixmap(m_data->icon()));
    QTimer::singleShot(0, this, SLOT(scaleContents()));
}

void ForecastDataWidget::scaleContents()
{
    int scaleFactor =this->height();
    m_highTemp->setStyleSheet("font-size:"+QString::number((int)(0.55*scaleFactor))+"px;");
    m_lowTemp->setStyleSheet("font-size:"+QString::number((int)(0.55*scaleFactor))+"px;");
    m_day->setStyleSheet("font-size:"+QString::number((int)(.55*scaleFactor))+"px;");
    m_iconHolderLabel->setPixmap(m_iconPixmap->scaledToHeight(.9*scaleFactor));
    widgetLayout->setContentsMargins(5,1,5,1);
}

QString ForecastDataWidget::iconNameToPixmap(QString icon)
{
    // set icon for day 3 of forecast
    if (icon == "partlysunny" || icon == "mostlycloudy" ) {
        return ":Images/weather-few-clouds.png";
    }
    else if (icon == "fog") {
        return ":Images/weather-fog.png";
    }
    else if (icon == "hazy") {
        return ":Images/weather-haze.png";
    }
    else if (icon == "cloudy") {
        return ":Images/weather-overcast.png";
    }
    else if (icon == "rain" || icon == "chancerain") {
        return ":Images/weather-showers.png";
    }
    else if (icon == "sleet" || icon == "chancesleet") {
        return ":Images/weather-sleet.png";
    }
    else if (icon == "flurries" || icon == "snow" ||
              icon == "chanceflurries" || icon == "chancesnow") {
        return ":Images/weather-snow.png";
    }
    else if (icon == "clear" || icon == "sunny") {
        return ":Images/weather-sunny.png";
    }
    else if (icon == "mostlysunny" || icon == "partlycloudy" ||
             icon == "unknown") {
        return ":Images/weather-sunny-very-few-clouds.png";
    }
    else if (icon == "tstorms" || icon == "chancetstorms") {
        return ":Images/weather-thundershower.png";
    }
    else
        return":Images/weather-sunny-very-few-clouds.png";

}
