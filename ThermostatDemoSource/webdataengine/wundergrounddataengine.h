#ifndef WUNDERGROUNDDATAENGINE_H
#define WUNDERGROUNDDATAENGINE_H

#include <QXmlStreamReader>
#include "webdataengine.h"

class ForecastData;


class WundergroundDataEngine : public WebDataEngine
{
    Q_OBJECT
public:
    explicit WundergroundDataEngine(QNetworkAccessManager *manager, QObject *parent = 0);

    void setCity(QString city);
    void dispatchRequest();
    void loadLocalData();

    QString licenseString();
signals:
    
public slots:
    void responseReceived();
    void parseXML(QByteArray *xmlData, bool cached = false);
private:
    bool writeToCache(QByteArray *xmlData);
    bool readFromCache(QByteArray *xmlData, QString alternateCacheFile = "");

    WeatherData* parseXMLWeatherData(QXmlStreamReader& reader);
    ForecastData* parseXMLForecastData(QXmlStreamReader& reader);

    QXmlStreamReader reader;
    
};

#endif // WUNDERGROUNDDATAENGINE_H
