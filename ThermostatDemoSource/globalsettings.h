#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QString>

class GlobalSettings
{
public:

    enum TemperatureFormat { TempFormatC, TempFormatF };
    enum TimeFormat { TimeFormat12h, TimeFormat24h };

    static GlobalSettings* getInstance();
    virtual ~GlobalSettings();

    bool save();
    bool load();

    void setProxyInfo(QString proxyHost, qint16 proxyPort);
    QString proxyHost();
    qint16 proxyPort();

    void setCurrentCity(QString currentCity);
    QString currentCity();

    void setTemperatureFormat(TemperatureFormat temperatureFormat);
    TemperatureFormat temperatureFormat();

    void setTimeFormat(TimeFormat timeFormat);
    TimeFormat timeFormat();

    void setDataPath(QString dataPath);
    QString dataPath();

private:
    GlobalSettings();

    static GlobalSettings* m_instance;

    //proxy info
    QString m_proxyHost;
    qint16 m_proxyPort;

    //app info
    QString m_currentCity;
    QString m_dataPath;

    TemperatureFormat m_temperatureFormat;
    TimeFormat m_timeFormat;
};

#endif // GLOBALSETTINGS_H
