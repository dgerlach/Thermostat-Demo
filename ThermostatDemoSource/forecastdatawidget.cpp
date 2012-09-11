#include "forecastdatawidget.h"

#include "forecastdata.h"
#include "utilities.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QTimer>

#include <QtDebug>

#define FORECAST_DAYS 3

/***********************************************************************************************************
* ForecastDataWidget
*   Widget that groups together the rows of forecast data for easy display in a grid layout. Handles display
*   of a ForecastData list in a QGridLayout
************************************************************************************************************/

ForecastDataWidget::ForecastDataWidget(QWidget *parent) :
    QWidget(parent)
{
    m_globalSettings = GlobalSettings::getInstance();
    widgetLayout = new QGridLayout ;

    for(int a = 0;a<FORECAST_DAYS;a++)
    {
        m_forecastDataWidgetGroup[a].highTemp = new QLabel("");
        m_forecastDataWidgetGroup[a].lowTemp = new QLabel("");
        m_forecastDataWidgetGroup[a].day = new QLabel("");
        m_forecastDataWidgetGroup[a].iconHolderLabel= new QLabel();
        m_forecastDataWidgetGroup[a].iconPixmap = new QPixmap();
        //m_forecastDataWidgetGroup[a].iconHolderLabel->setPixmap(*m_forecastDataWidgetGroup[a].iconPixmap);

        m_forecastDataWidgetGroup[a].day->setMargin(0);

        m_forecastDataWidgetGroup[a].highTemp->setMargin(0);
        m_forecastDataWidgetGroup[a].lowTemp->setMargin(0);
        m_forecastDataWidgetGroup[a].iconHolderLabel->setMargin(0);

        widgetLayout->addWidget(m_forecastDataWidgetGroup[a].day, a, 0);

        widgetLayout->addWidget(m_forecastDataWidgetGroup[a].iconHolderLabel, a, 1);
        widgetLayout->addWidget(m_forecastDataWidgetGroup[a].lowTemp, a, 2);

        widgetLayout->addWidget(m_forecastDataWidgetGroup[a].highTemp, a, 3);

    }

    widgetLayout->setHorizontalSpacing(0);
    widgetLayout->setVerticalSpacing(0);
    widgetLayout->setMargin(0);


    widgetLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout* l = new QHBoxLayout;
    l->setSpacing(0);
    l->setContentsMargins(0,0,0,0);
    l->addLayout(widgetLayout);

    setStyleSheet("color:#ffffff;background-color: none;padding:0px;spacing:0px;");
    setLayout(l);
    QTimer::singleShot(0, this, SLOT(scaleContents()));

}

//FUNCTION: createScreenLayout
//
//  A setter function that gives the widget the data list obtained from the web
//

void ForecastDataWidget::setData(QList<ForecastData *> data)
{
    m_data = data;
}

//FUNCTION::updateData()
//
//  Slot that updates all displayed values. Useful for unit changes, and invokes scale contents afterwards
void ForecastDataWidget::updateData()
{
    for(int a = 0;a<FORECAST_DAYS;a++)
    {
        m_forecastDataWidgetGroup[a].highTemp->setText(formatTemperatureString(m_data[a]->highTemp(), m_globalSettings->temperatureFormat()));
        m_forecastDataWidgetGroup[a].lowTemp->setText(formatTemperatureString(m_data[a]->lowTemp(), m_globalSettings->temperatureFormat()));
        m_forecastDataWidgetGroup[a].day->setText(m_data[a]->day());
        m_forecastDataWidgetGroup[a].iconPixmap->load(iconNameToPixmap(m_data[a]->icon()));
    }
    QTimer::singleShot(0, this, SLOT(scaleContents()));
}

//FUNCTION: scaleContents
//
//  Slot that scales the size of every widget contained within the layout to a satisfactory size based on available space.
//

void ForecastDataWidget::scaleContents()
{
    //scale factor is calculated by inverting the number of rows times a scale factor (for some breathing room) and then multiplying by
    //the total height of the widget (giving each row that proportion of the total space)
    int scaleFactor =(1/(FORECAST_DAYS*1.1))*this->height();

    for(int a = 0;a<FORECAST_DAYS;a++)
    {
        m_forecastDataWidgetGroup[a].highTemp->setStyleSheet("font-size:"+QString::number((int)(0.50*scaleFactor))+"px;");
        m_forecastDataWidgetGroup[a].lowTemp->setStyleSheet("font-size:"+QString::number((int)(0.50*scaleFactor))+"px;");
        m_forecastDataWidgetGroup[a].day->setStyleSheet("font-size:"+QString::number((int)(.50*scaleFactor))+"px;");
        m_forecastDataWidgetGroup[a].iconHolderLabel->setPixmap(m_forecastDataWidgetGroup[a].iconPixmap->scaledToHeight(.9*scaleFactor));
    }

}

//FUNCTION: iconNameToPixmap
//
//  Helper function that converts the weather related icon name given from the web to a relevant resource
//

QString ForecastDataWidget::iconNameToPixmap(QString icon)
{

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
