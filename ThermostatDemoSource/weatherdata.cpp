#include "weatherdata.h"

WeatherData::WeatherData()
{
    m_cached = false;
}

void WeatherData::setCurrentCity(QString city)
{
    m_city = city;
}

QString WeatherData::currentCity()
{
    return m_city;
}

void WeatherData::setCurrentTemp(int temp)
{
    m_currentTemp = temp;
}

int WeatherData::currentTemp()
{
    return m_currentTemp;
}

void WeatherData::setIcon(QString icon)
{
    m_icon = icon;
}

QString WeatherData::icon()
{
    return m_icon;
}

void WeatherData::setLastUpdated(QDateTime lastUpdated)
{
    m_lastUpdated = lastUpdated;
}

QDateTime WeatherData::lastUpdated()
{
    return m_lastUpdated;
}

void WeatherData::addForecastDay(ForecastData* day)
{
    m_forecastList << day;
}

void WeatherData::removeForecastDay(ForecastData *day)
{
    m_forecastList.removeOne(day);
}

QList<ForecastData *> WeatherData::forecastData()
{
    return m_forecastList;
}

void WeatherData::setLocalTime(QDateTime localTime)
{
    m_localTime = localTime;
}

QDateTime WeatherData::localTime()
{
    return m_localTime;
}

void WeatherData::setCachedDataFlag()
{
    m_cached = true;
}

bool WeatherData::cachedData()
{
    return m_cached;
}
