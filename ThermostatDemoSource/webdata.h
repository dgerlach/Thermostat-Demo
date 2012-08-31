#ifndef WEBDATA_H
#define WEBDATA_H

#include <QObject>
#include <QtNetwork>
#include <QUrl>

class GlobalSettings;
class ForecastData;
class WeatherData;

class WebData : public QObject
{
    Q_OBJECT
public:
    explicit WebData(QObject *parent = 0);
    QTime clockObject();

    void changeCity(QString);

    bool writeToCache(QByteArray *xmlData);
    bool readFromCache(QByteArray *xmlData, QString alternateCacheFile = "");
    void loadLocalData();
    
signals:
    void dataAvailable(WeatherData* weatherData);
    void networkTimeout();
    
public slots:
    void configureProxy();
private slots:
    void parseXML(QByteArray *xmlData);
    void responseReceived();


private:
    WeatherData* parseWeatherData(QXmlStreamReader& reader);
    ForecastData* parseForecastData(QXmlStreamReader& reader);

    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QNetworkReply *reply;
    QXmlStreamReader reader;
    QString currentDate;
    QString currentTime;
    QTime clock;
    QString currentTemp;
    QStringList forecastIconList;
    QStringList weekdayList;
    QStringList forecastTempList;
    QString unitConversion(QString);

    GlobalSettings *m_globalSettings;


};

#endif // WEBDATA_H
