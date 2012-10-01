#ifndef OPENWEATHERMAPDATAENGINE_H
#define OPENWEATHERMAPDATAENGINE_H

#include <QHash>

#include "webdataengine.h"

class OpenWeatherMapDataEngine : public WebDataEngine
{
    Q_OBJECT
public:
    explicit OpenWeatherMapDataEngine(QNetworkAccessManager *manager, QObject *parent = 0);
    
    void setCity(QString city);
    void dispatchRequest();
    void loadLocalData();

    QString licenseString();

signals:
    
public slots:
    void responseReceived();
    void currentWeatherResponseReceived();
    void forecastResponseReceived();
    void handleNetworkTimeout();

private:

    bool readFromCache(QString alternateCachePath = "");
    bool writeToCache();

    WeatherData* m_weatherData;

    void dispatchWeatherDataRequests();

    qint64 parseCityInformation(QString *jsonData);
    bool parseJSONWeatherData(QString* jsonData, WeatherData* weatherData);
    bool parseJSONForecastData(QString* jsonData, WeatherData* weatherData);
    void generateJSONWeatherLookupTables();
    void checkIfDone();

    qint64 m_cityId;
    QNetworkReply *m_forecastReply;
    QTimer m_forecastNetworkTimer;

    QHash<int, QString> m_iconNameToWeatherHash;
    QHash<QString, int> m_weatherPriorityHash;

    //for some reason api returns temp in kelvin
    int kelvinToFahrenheit(double k);

    //api gives back image url that we can convert to use meaningful weather names
    int convertImageNameToIndex(QString img);

    //flags to make sure we've received both portions of the weather data
    bool m_forecastReceived;
    bool m_weatherReceived;

    QString m_rawJSONWeatherString;
    QString m_rawJSONForecastString;
};

#endif // OPENWEATHERMAPDATAENGINE_H
