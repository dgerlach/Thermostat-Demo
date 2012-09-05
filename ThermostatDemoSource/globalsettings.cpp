#include "globalsettings.h"

#include <QFileInfo>
#include <QSettings>
#include <QtDebug>

/***********************************************************************************************************
* GlobalSettings
*   Singleton class to be used from anywhere to allow global access to any configuration values
*   Also handles reading and writing of files using QSettings API to allow for standardized file formats
************************************************************************************************************/

//initialization of static instance variable
GlobalSettings* GlobalSettings::m_instance = NULL;

//private constructor so class must be instantiated through getInstance function
GlobalSettings::GlobalSettings()
{
}

//destructor handles deleting of the instance variable
GlobalSettings::~GlobalSettings()
{
    delete m_instance;
}

//FUNCTION: getInstance()
//
//  Checks to see if static instance member variable points to anything,
//  if not, creates instance of settings class, loads the data from a file,
//  then returns the pointer to the instance

GlobalSettings* GlobalSettings::getInstance()
{
    if(m_instance == NULL)
    {
        m_instance = new GlobalSettings;
        m_instance->load();
    }

    return m_instance;
}

//FUNCTION: save()
//
//  Uses standard QSettings Qt API to store the settings in a file that is easily
//  user editable.

bool GlobalSettings::save()
{
    QSettings settingsObject("ti", "thermostat");

    settingsObject.setValue("proxy-host", proxyHost());
    settingsObject.setValue("proxy-port", proxyPort());
    settingsObject.setValue("current-city", currentCity());
    settingsObject.setValue("temperature-format", temperatureFormat());
    settingsObject.setValue("time-format", timeFormat());

}

//FUNCTION: load()
//
//  Uses QSettings API to load config values from a previously written file and also can set default values if
//  no configuration file is present

bool GlobalSettings::load()
{
    QSettings settingsObject("ti", "thermostat");

    setProxyInfo(settingsObject.value("proxy-host", "").toString(), settingsObject.value("proxy-port", 0).toInt());
    setCurrentCity(settingsObject.value("current-city", "Dallas,TX").toString());
    setTemperatureFormat(static_cast<GlobalSettings::TemperatureFormat>(settingsObject.value("temperature-format", GlobalSettings::TempFormatF).toInt()));
    setTimeFormat(static_cast<GlobalSettings::TimeFormat>(settingsObject.value("time-format", GlobalSettings::TimeFormat12h).toInt()));
    setDataPath(QFileInfo(settingsObject.fileName()).absolutePath());

    qDebug() << "Settings loaded...";
    qDebug() << "Proxy Host: " << proxyHost();
    qDebug() << "Proxy Port: " << proxyPort();
    qDebug() << "Current City: " << currentCity();
    qDebug() << "Temperature Format: " << temperatureFormat();
    qDebug() << "Time Format: " << timeFormat();
    qDebug() << "Data Path: " << dataPath();

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
