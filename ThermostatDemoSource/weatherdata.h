#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QDateTime>

class ForecastData;

class WeatherData
{
public:
    WeatherData();

    void setCurrentCity(QString city);
    QString currentCity();

    void setCurrentTemp(int temp);
    int currentTemp();

    void setIcon(QString icon);
    QString icon();

    void setLastUpdated(QDateTime lastUpdated);
    QDateTime lastUpdated();

    void addForecastDay(ForecastData* day);
    void removeForecastDay(ForecastData* day);
    QList<ForecastData * > forecastData();

    void setLocalTime(QDateTime localTime);
    QDateTime localTime();

    void setCachedDataFlag();
    bool cachedData();

private:
    QString m_city;
    int m_currentTemp;
    QString m_icon;
    QDateTime m_lastUpdated;
    QDateTime m_localTime;
    QList<ForecastData *> m_forecastList;
    bool m_cached;
};



#endif // WEATHERDATA_H
