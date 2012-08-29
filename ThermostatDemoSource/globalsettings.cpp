#include "globalsettings.h"

#include <QFileInfo>
#include <QSettings>
#include <QtDebug>

/***********************************************************************************************************
* GlobalSettings
*   Singleton class to be used from anywhere to allow global access to any configuration values
*   Also handles reading and writing of files using QSettings API to allow for standardized file formats
************************************************************************************************************/


GlobalSettings* GlobalSettings::m_instance = NULL;

GlobalSettings::GlobalSettings()
{
}

GlobalSettings* GlobalSettings::getInstance()
{
    if(m_instance == NULL)
    {
        m_instance = new GlobalSettings;
        m_instance->load();
    }

    return m_instance;
}

bool GlobalSettings::save()
{
    QSettings settingsObject("ti", "thermostat");

    settingsObject.setValue("proxy-host", proxyHost());
    settingsObject.setValue("proxy-port", proxyPort());
    settingsObject.setValue("current-city", currentCity());
    settingsObject.setValue("temperature-format", temperatureFormat());
    settingsObject.setValue("time-format", timeFormat());

}

bool GlobalSettings::load()
{
    QSettings settingsObject("ti", "thermostat");

    setProxyInfo(settingsObject.value("proxy-host", "").toString(), settingsObject.value("proxy-port", 0).toInt());
    setCurrentCity(settingsObject.value("current-city", "Dallas,TX").toString());
    setTemperatureFormat(static_cast<GlobalSettings::TemperatureFormat>(settingsObject.value("temperature-format", GlobalSettings::TempFormatF).toInt()));
    setTimeFormat(static_cast<GlobalSettings::TimeFormat>(settingsObject.value("time-format", GlobalSettings::TimeFormat12h).toInt()));
    setDataPath(QFileInfo(settingsObject.fileName()).absolutePath());

    qDebug() << "Settings loaded...";
    qDebug() << proxyHost();
    qDebug() << proxyPort();
    qDebug() << currentCity();
    qDebug() << temperatureFormat();
    qDebug() << timeFormat();

}

void GlobalSettings::setProxyInfo(QString proxyHost, qint16 proxyPort)
{
    m_proxyHost = proxyHost;
    m_proxyPort = proxyPort;
}
void GlobalSettings::setCurrentCity(QString currentCity)
{
    m_currentCity = currentCity;
}

QString GlobalSettings::proxyHost()
{
    return m_proxyHost;
}

qint16 GlobalSettings::proxyPort()
{
    return m_proxyPort;
}

QString GlobalSettings::currentCity()
{
    return m_currentCity;
}

void GlobalSettings::setTemperatureFormat(TemperatureFormat temperatureFormat)
{
    m_temperatureFormat = temperatureFormat;
}

GlobalSettings::TemperatureFormat GlobalSettings::temperatureFormat()
{
    return m_temperatureFormat;
}

void GlobalSettings::setTimeFormat(TimeFormat timeFormat)
{
    m_timeFormat = timeFormat;
}

GlobalSettings::TimeFormat GlobalSettings::timeFormat()
{
    return m_timeFormat;
}

void GlobalSettings::setDataPath(QString dataPath)
{
    m_dataPath = dataPath;
}

QString GlobalSettings::dataPath()
{
    return m_dataPath;
}
