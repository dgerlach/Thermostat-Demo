#ifndef WEBDATA_H
#define WEBDATA_H

#include <QObject>
#include <QtNetwork>
#include <QUrl>

class GlobalSettings;
class ForecastData;
class WeatherData;
class WebDataEngine;

class WebData : public QObject
{
    Q_OBJECT
public:
    explicit WebData(QObject *parent = 0);

    void changeCity(QString);
    void loadLocalData();

signals:
    void dataAvailable(WeatherData* weatherData);
    void networkTimeout();
    
public slots:
    void configureProxy();

private:

    GlobalSettings *m_globalSettings;
    QNetworkAccessManager *manager;
    WebDataEngine* webDataEngine;
};

#endif // WEBDATA_H
