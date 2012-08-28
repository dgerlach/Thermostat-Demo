#include "webdata.h"
#include "settingscreen.h"
#include "globalsettings.h"

#include <QtDebug>
#include <QTime>

webdata::webdata(QObject *parent) :
    QObject(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    // create network access manager
    manager = new QNetworkAccessManager(this);

    configureProxy();
}

void webdata::configureProxy()
{
    // check and see if proxy settings are necessary
    if(m_globalSettings->proxyHost() != "") {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(m_globalSettings->proxyHost());
        proxy.setPort(m_globalSettings->proxyPort());
        manager->setProxy(proxy);
    }
}

void webdata::responseReceived()
{
    qDebug() << reply->error() << reply->errorString();

    if(reply->error() == QNetworkReply::NoError)
        parseXML();
    else
        emit networkTimeout();
}

void webdata::parseXML()
{
    // clear all previous data
    currentDate.clear();
    currentTime.clear();
    forecastIconList.clear();
    weekdayList.clear();
    forecastTempList.clear();

    // create XML reader on the document held in reply
    reader.setDevice(reply);

    // the following while loop reads through the XML document, pulls out
    // the information needed for the demo, and converts it to the proper
    // format when appropriate
    while (!reader.atEnd() && !reader.hasError()) {
        reader.readNext();
        if (reader.isStartElement()) {
            QString name = reader.name().toString();
            if (name == "local_time_rfc822") {
                QString currentDateTime = reader.readElementText();
                QStringList splitCurrentDateTime = currentDateTime.split(" ");

                for (int i = 0; i < 3; ++i) {
                    currentDate.append(splitCurrentDateTime.at(i).toLocal8Bit().constData());
                    currentDate.append(" ");
                }
                currentTime.append(splitCurrentDateTime.at(4));
                QStringList timeList = currentTime.split(":");
                // create accurate QTime object
                clock.setHMS(timeList.at(0).toInt(),timeList.at(1).toInt(),timeList.at(2).toInt());

            }
            else if (name == "temp_f") {
                QString temp = reader.readElementText();
                QStringList tempList = temp.split(".");
                currentTemp = tempList.at(0);
            }
            else if (name == "icon") {
                forecastIconList.append(reader.readElementText());
            }
            else if (name == "weekday_short") {
                weekdayList.append(reader.readElementText());
            }
            else if (name == "fahrenheit") {
                forecastTempList.append(reader.readElementText());
            }
        }
    }

    if (reader.hasError()) {
        qDebug() << "XML error: " << reader.errorString() << endl;
    }
    else if (reader.atEnd()) {
        qDebug() << "Reached end of XML document" << endl;
    }
    reply->deleteLater();
    emit dataAvailable();
}


QString webdata::date()
{
    // return current date in provided city
    return currentDate;
}

QString webdata::time()
{
    // return local time in provided city
    return clock.toString("h:mm AP");
}

QString webdata::temp()
{
    // return current temp in Celsius or Fehrenheit
    return unitConversion(currentTemp);
}

QString webdata::currentIcon()
{
    // return string for what current icon should be
    return forecastIconList.at(0);
}

QString webdata::day1Icon()
{
    // return string for what day 1 icon should be
    return forecastIconList.at(1);
}

QString webdata::day2Icon()
{
    // return string for what day 2 icon should be
    return forecastIconList.at(3);
}

QString webdata::day3Icon()
{
    // return string for what day 3 icon should be
    return forecastIconList.at(5);
}

QString webdata::day1()
{
    // return day 1 abbreviation (i.e. Wed)
    return weekdayList.at(0);
}

QString webdata::day2()
{
    // return day 2 abbreviation
    return weekdayList.at(1);
}

QString webdata::day3()
{
    // return day 3 abbreviation
    return weekdayList.at(2);
}

QString webdata::day1High()
{
    // return day 1 high
    return unitConversion(forecastTempList.at(0));
}

QString webdata::day1Low()
{
    // return day 1 low
    return unitConversion(forecastTempList.at(1));
}

QString webdata::day2High()
{
    // return day 2 high
    return unitConversion(forecastTempList.at(2));
}

QString webdata::day2Low()
{
    // return day 2 low
    return unitConversion(forecastTempList.at(3));
}

QString webdata::day3High()
{
    // return day 3 high
    return unitConversion(forecastTempList.at(4));
}

QString webdata::day3Low()
{
    // return day 3 low
    return unitConversion(forecastTempList.at(5));
}

QTime webdata::clockObject()
{
    // return clock object itself (not string with time)
    return clock;
}

void webdata::changeCity(QString city)
{
    qDebug() << city;
    // if the city is changed, this function receives the city name and generates
    // the proper URL to send the right request
    QStringList cityList;
    cityList = city.split(",");
    cityList.replaceInStrings(" ","_");
    QString cityUrl = "http://api.wunderground.com/api/9d8a29de9939cbb7/geolookup/conditions/forecast/q/" + cityList.at(1) + "/" + cityList.at(0) + ".xml";

    qDebug() << cityUrl;

    // receive document and parse it
    request.setUrl(QUrl(cityUrl));

    reply = manager->get(request);
    connect(reply,SIGNAL(finished()),this,SLOT(responseReceived()));
}

QString webdata::unitConversion(QString currentValue)
{
    // check to see if setting is Farhenheit or Celsius and make proper conversion
    // values are read in as Fahrenheit, so only need to convert to Celsius if necessary
    int FInt, CInt;

    if(!(m_globalSettings->temperatureFormat() == GlobalSettings::TempFormatF)) {
        // false means C
        // we want Celsius, but currently in Fahrenheit
        FInt = currentValue.toInt();
        CInt = ((FInt - 32) * 5)/9;
        return QString::number(CInt);
    } else {
        return currentValue;
    }
}
