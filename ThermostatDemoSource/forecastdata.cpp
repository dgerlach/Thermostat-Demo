#include "forecastdata.h"

ForecastData::ForecastData()
{
}

void ForecastData::setDay(QString day)
{
    m_day = day;
}

QString ForecastData::day()
{
    return m_day;
}

void ForecastData::setHighTemp(int tempHigh)
{
    m_tempHigh = tempHigh;
}

int ForecastData::highTemp()
{
    return m_tempHigh;
}

void ForecastData::setLowTemp(int lowTemp)
{
    m_tempLow = lowTemp;
}

int ForecastData::lowTemp()
{
    return m_tempLow;
}

void ForecastData::setIcon(QString icon)
{
    m_icon = icon;
}

QString ForecastData::icon()
{
    return m_icon;
}
