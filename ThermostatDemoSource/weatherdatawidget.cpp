#include "weatherdatawidget.h"

#include <QtGui>
#include "globalsettings.h"
#include "weatherdata.h"
#include "utilities.h"

WeatherDataWidget::WeatherDataWidget(QWidget *parent) :
    QWidget(parent)
{
    m_data=NULL;
    m_globalSettings = GlobalSettings::getInstance();

    m_currentTemp = new QLabel();
    m_currentCity = new QLabel();
    m_currentCity->setWordWrap(true);
    m_currentIconHolderLabel = new QLabel();
    m_currentIconHolderLabel->setScaledContents(true);
    m_currentIconPixmap = new QPixmap();

    // create current layout
    QVBoxLayout *currentVLayout = new QVBoxLayout;
    currentVLayout->addWidget(m_currentTemp);
    currentVLayout->addWidget(m_currentCity);

    // add icon to current layout
    QHBoxLayout *currentHLayout = new QHBoxLayout;
    currentHLayout->addWidget(m_currentIconHolderLabel);
    currentHLayout->setAlignment(m_currentIconHolderLabel, Qt::AlignCenter);
    currentHLayout->setContentsMargins(0,0,0,0);
    currentHLayout->addLayout(currentVLayout);

    setStyleSheet("color:#ffffff;background: 0;padding:0px;");
    setLayout(currentHLayout);
    QTimer::singleShot(0, this, SLOT(scaleContents()));
}


void WeatherDataWidget::setData(WeatherData *data)
{
    m_data = data;
}

void WeatherDataWidget::updateData()
{
    m_currentTemp->setText(formatTemperatureString(m_data->currentTemp(), m_globalSettings->temperatureFormat()));
    m_currentCity->setText(m_data->currentCity());
    m_currentIconPixmap->load(iconNameToPixmap(m_data->icon()));
    QTimer::singleShot(0, this, SLOT(scaleContents()));
}

void WeatherDataWidget::scaleContents()
{
    int scaleFactor = this->height();
    m_currentTemp->setStyleSheet("font-size:"+QString::number((int)(.48*scaleFactor))+"px;");
    m_currentCity->setStyleSheet("font-size:"+QString::number((int)(.16*scaleFactor))+"px;");
    m_currentIconHolderLabel->setPixmap(m_currentIconPixmap->scaledToHeight(.9*scaleFactor));
}

QString WeatherDataWidget::iconNameToPixmap(QString icon)
{
    QString pixmap;
    // set icon for current weather conditions
    if (icon == "partlysunny" || icon == "mostlycloudy" ) {
        if(m_data->localTime().time().hour() < 5 || m_data->localTime().time().hour() > 20) {
            pixmap = (":Images/weather-moony-few-clouds.png");
        } else {
            pixmap = (":Images/weather-few-clouds.png");
        }
    }
    else if (icon == "fog") {
        pixmap = (":Images/weather-fog.png");
    }
    else if (icon == "hazy") {
        if(m_data->localTime().time().hour() < 5 || m_data->localTime().time().hour() > 20) {
            pixmap = (":Images/weather-moony.png");
        } else {
            pixmap = (":Images/weather-haze.png");
        }
    }
    else if (icon == "cloudy") {
        pixmap = (":Images/weather-overcast.png");
    }
    else if (icon == "rain" || icon == "chancerain") {
        pixmap = (":Images/weather-showers.png");
    }
    else if (icon == "sleet" || icon == "chancesleet") {
        pixmap = (":Images/weather-sleet.png");
    }
    else if (icon == "flurries" || icon == "snow" ||
              icon == "chanceflurries" || icon == "chancesnow") {
        pixmap = (":Images/weather-snow.png");
    }
    else if (icon == "clear" || icon == "sunny") {
        if(m_data->localTime().time().hour() < 5 || m_data->localTime().time().hour() > 20) {
            pixmap = (":Images/weather-moony.png");
        } else {
            pixmap = (":Images/weather-sunny.png");
        }
    }
    else if (icon == "mostlysunny" || icon == "partlycloudy" ||
             icon == "unknown") {
        if(m_data->localTime().time().hour() < 5 || m_data->localTime().time().hour() > 20) {
            pixmap = (":Images/weather-moony-very-few-clouds");

        } else {
            pixmap = (":Images/weather-sunny-very-few-clouds.png");
        }
    }
    else if (icon == "tstorms" || icon == "chancetstorms") {
        pixmap = (":Images/weather-thundershower.png");
    }

    return pixmap;
}

QVariantHash WeatherDataWidget::getCurrentData()
{
    QVariantHash data;
    if(m_data)
    {
        data.insert("currentTemp", formatTemperatureString(m_data->currentTemp(), m_globalSettings->temperatureFormat()));
        data.insert("currentCity", m_data->currentCity());
        data.insert("currentImage", iconNameToPixmap(m_data->icon()).mid(1));
    }
    return data;
}
