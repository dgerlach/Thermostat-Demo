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

/***********************************************************************************************************
* WebData
*   Class that wraps a webdataengine used for retrieving data from an online API. Handles network connection
*   manager and also configures the proxy, first by trying none and then a known one, then failing.
************************************************************************************************************/

WebData::WebData(QObject *parent) :
    QObject(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    // create network access manager
    manager = new QNetworkAccessManager(this);

    webDataEngine = new OpenWeatherMapDataEngine(manager);
    connect(webDataEngine, SIGNAL(dataAvailable(WeatherData*)), this, SLOT(processDataAvailable(WeatherData*)));
    connect(webDataEngine, SIGNAL(networkTimeout()), this, SLOT(processNetworkTimeout()));

    //set proxy by default to none so we can try it
    m_proxyState = WebData::InternalTI;

    configureProxy();

}

//FUNCTION: configureProxy()
//
//  configures the correct proxy based on the ProxyState enum defined

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

//FUNCTION: configureNoProxy()
//
//  Sets the network access manager to use no proxy at all

void WebData::configureNoProxy()
{
    manager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    m_proxyState = WebData::ExternalTI;
}

//FUNCTION: configureTIProxy()
//
//  Sets the network access manager to use the internal TI proxy

void WebData::configureTIProxy()
{
    manager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "wwwgate.ti.com", 80));
    m_proxyState = WebData::InternalTI;
}

//FUNCTION: configureSetProxy()
//
//  Sets the network access manager to use proxy that is configured in the settings

void WebData::configureSetProxy()
{
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(m_globalSettings->proxyHost());
    proxy.setPort(m_globalSettings->proxyPort());
    manager->setProxy(proxy);
    m_proxyState = WebData::Configured;
}

//FUNCTION: changeCity()
//
//  Sets the webDataEngine's city variable and then dispatches a request

void WebData::changeCity(QString city)
{
    webDataEngine->setCity(city);
    webDataEngine->dispatchRequest();
}

//FUNCTION: loadLocalData()
//
//  invokes the webdataengine's loadLocalData function to pull locally cached data

void WebData::loadLocalData()
{
    webDataEngine->loadLocalData();
}

//FUNCTION: licenseString()
//
//  Returns the defined license string in the webdataengine

QString WebData::licenseString()
{
    return webDataEngine->licenseString();
}

//FUNCTION: processDataAvailable()
//
//  Catches the dataAvailable signal from the webdataengine and sets the proxy if it worked
//  Otherwise emits signal with either web data or cache data

void WebData::processDataAvailable(WeatherData *weatherData)
{
    if(!weatherData->cachedData())
    {
        m_globalSettings->setProxyInfo(manager->proxy().hostName(), manager->proxy().port());
        m_proxyState = WebData::Configured;
    }

    emit dataAvailable(weatherData);
}

//FUNCTION: processNetworkTimeout()
//
//  Catches the networkTimeout signal form the webdataengine and tries a new proxy if
//  still in the configuration phase. Other wise emits a normal networkTimeout signal

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
}
