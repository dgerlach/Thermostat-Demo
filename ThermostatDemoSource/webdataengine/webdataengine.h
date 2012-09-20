#ifndef WEBDATAENGINE_H
#define WEBDATAENGINE_H

#include <QObject>
#include <QTimer>

class GlobalSettings;
class QNetworkAccessManager;
class QNetworkReply;
class WeatherData;

class WebDataEngine : public QObject
{
    Q_OBJECT
public:
    explicit WebDataEngine(QNetworkAccessManager *manager, QObject *parent = 0);

    virtual void setCity(QString city) = 0;
    virtual void dispatchRequest() = 0;
    virtual void loadLocalData() = 0;

    virtual QString licenseString() = 0;

signals:
    void dataAvailable(WeatherData* weatherData);
    void networkTimeout();

public slots:


protected:
    QNetworkAccessManager *m_manager;
    QString m_preparedCity;
    QString m_fullCity;
    QNetworkReply *m_reply;
    QTimer m_networkTimer;
    GlobalSettings *m_globalSettings;
};

#endif // WEBDATAENGINE_H
