#ifndef FORECASTDATA_H
#define FORECASTDATA_H

#include <QString>

class ForecastData
{
public:
    ForecastData();

    void setDay(QString day);
    QString day();

    void setHighTemp(int tempHigh);
    int highTemp();

    void setLowTemp(int lowTemp);
    int lowTemp();

    void setIcon(QString icon);
    QString icon();

private:
    QString m_day;
    int m_tempHigh;
    int m_tempLow;
    QString m_icon;
};

#endif // FORECASTDATA_H
