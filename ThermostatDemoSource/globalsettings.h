#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QString>

class GlobalSettings
{
public:

    enum TemperatureFormat { TempFormatC, TempFormatF };
    enum TimeFormat { TimeFormat12h, TimeFormat24h };

    static GlobalSettings* getInstance();

    bool writeToFile();
    bool readFromFile();

    void setProxyInfo(QString proxyHost, qint16 proxyPort);
    QString proxyHost();
    qint16 proxyPort();

    void setCurrentCity(QString currentCity);
    QString currentCity();

    void setTemperatureFormat(TemperatureFormat temperatureFormat);
    TemperatureFormat temperatureFormat();

    void setTimeFormat(TimeFormat timeFormat);
    TimeFormat timeFormat();

private:
    GlobalSettings();

    static GlobalSettings* m_instance;

    //proxy info
    QString m_proxyHost;
    qint16 m_proxyPort;

    //app info
    QString m_currentCity;

    TemperatureFormat m_temperatureFormat;
    TimeFormat m_timeFormat;
};

#endif // GLOBALSETTINGS_H
