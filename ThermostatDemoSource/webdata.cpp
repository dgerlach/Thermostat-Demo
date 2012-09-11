#include "webdata.h"
#include "globalsettings.h"

#include "weatherdata.h"
#include "webdataengine/openweathermapdataengine.h"
#include "webdataengine/wundergrounddataengine.h"

#include "forecastdata.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

#include <QtDebug>
#include <QTime>

WebData::WebData(QObject *parent) :
    QObject(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    // create network access manager
    manager = new QNetworkAccessManager(this);

    webDataEngine = new OpenWeatherMapDataEngine(manager);
    connect(webDataEngine, SIGNAL(dataAvailable(WeatherData*)), this, SIGNAL(dataAvailable(WeatherData*)));

    configureProxy();
}

void WebData::configureProxy()
{
    // check to see if proxy settings are necessary
    if(m_globalSettings->proxyHost() != "") {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(m_globalSettings->proxyHost());
        proxy.setPort(m_globalSettings->proxyPort());
        manager->setProxy(proxy);
    }
    else
        manager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
}

void WebData::changeCity(QString city)
{
    webDataEngine->setCity(city);
    webDataEngine->dispatchRequest();
}

void WebData::loadLocalData()
{
    webDataEngine->loadLocalData();
}
