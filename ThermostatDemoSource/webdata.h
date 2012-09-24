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
    enum ProxyState { ExternalTI, InternalTI, Configured };

    explicit WebData(QObject *parent = 0);

    void changeCity(QString);
    void loadLocalData();
    QString licenseString();

signals:
    void dataAvailable(WeatherData* weatherData);
    void networkTimeout();
    
public slots:
    void configureProxy();

    void processDataAvailable(WeatherData* weatherData);
    void processNetworkTimeout();

private:

    void configureNoProxy();
    void configureTIProxy();
    void configureSetProxy();

    GlobalSettings *m_globalSettings;
    QNetworkAccessManager *manager;
    WebDataEngine* webDataEngine;

    ProxyState m_proxyState;
};

#endif // WEBDATA_H
