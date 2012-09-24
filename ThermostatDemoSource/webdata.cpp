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
    connect(webDataEngine, SIGNAL(dataAvailable(WeatherData*)), this, SLOT(processDataAvailable(WeatherData*)));
    connect(webDataEngine, SIGNAL(networkTimeout()), this, SLOT(processNetworkTimeout()));

    m_proxyState = WebData::ExternalTI;

    configureProxy();

}

void WebData::configureProxy()
{
    //qDebug() << m_proxyState << manager->proxy().hostName() << manager->proxy().port();

    switch(m_proxyState)
    {
    case WebData::ExternalTI:
        configureNoProxy();
        break;
    case WebData::InternalTI:
        configureTIProxy();
        break;
    case WebData::Configured:
        configureSetProxy();
        break;
    default:
        break;
    }
}

void WebData::configureNoProxy()
{
    manager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    m_proxyState = WebData::ExternalTI;
}

void WebData::configureTIProxy()
{
    manager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "wwwgate.ti.com", 80));
    m_proxyState = WebData::InternalTI;
}

void WebData::configureSetProxy()
{
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(m_globalSettings->proxyHost());
    proxy.setPort(m_globalSettings->proxyPort());
    manager->setProxy(proxy);
    m_proxyState = WebData::Configured;
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

QString WebData::licenseString()
{
    return webDataEngine->licenseString();
}

void WebData::processDataAvailable(WeatherData *weatherData)
{
    qDebug() << weatherData->cachedData();
    if(!weatherData->cachedData())
    {
        qDebug() << "NOT CACHED DATA";
        m_globalSettings->setProxyInfo(manager->proxy().hostName(), manager->proxy().port());
        m_proxyState = WebData::Configured;
    }

    emit dataAvailable(weatherData);
}

void WebData::processNetworkTimeout()
{
    //if we fail with a particular proxy, move on to the next in the list
    //heirarchy is external (none) then TI then one that is set in the configuration
    //once we get to configure call it quits on retrying

    if(m_proxyState == WebData::ExternalTI)
        configureTIProxy();
    else if(m_proxyState == WebData::InternalTI)
        configureSetProxy();

    configureProxy();

    if(m_proxyState != WebData::Configured)
        webDataEngine->dispatchRequest();
    else
        emit networkTimeout();

    qDebug() << m_proxyState;
}
