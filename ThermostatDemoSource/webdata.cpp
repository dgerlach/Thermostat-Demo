#include "webdata.h"
#include "settingscreen.h"
#include "globalsettings.h"

#include <QtDebug>
#include <QTime>

WebData::WebData(QObject *parent) :
    QObject(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    // create network access manager
    manager = new QNetworkAccessManager(this);

    configureProxy();
}

void WebData::configureProxy()
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

//FUNCTION: responseReceived()
//
//  Slot that handles the reply from the QNetworkAccessManager. Also triggered
//  in the event of an error, both success and fail cases are handled.
//  Either grabs the data returned from the request, writes it to a local cache
//  and then parses it or tries to read from a previously written cache file and
//  then parses that.
//

void WebData::responseReceived()
{
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray xmlData = reply->readAll();
        qDebug () << xmlData;
        writeToCache(&xmlData);
        parseXML(&xmlData);
    }
    else
    {
        loadLocalData();
        emit networkTimeout();
    }

    //docs say do not delete in the slot so well pass it off to the event loop
    reply->deleteLater();
}

void WebData::loadLocalData()
{
    QByteArray xmlData;

    bool result = readFromCache(&xmlData);

    if(!result)
        //if we can't read from the cache file, read from the one included in the qrc!
        readFromCache(&xmlData, ":/data/cache.xml");

    parseXML(&xmlData);
}

void WebData::parseXML(QByteArray* xmlData)
{
    // clear all previous data
    currentDate.clear();
    currentTime.clear();
    forecastIconList.clear();
    weekdayList.clear();
    forecastTempList.clear();

    QBuffer xmlBuffer(xmlData);

    // create XML reader on the document held in reply
    reader.setDevice(&xmlBuffer);
    xmlBuffer.open(QBuffer::ReadWrite);

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

    emit dataAvailable();
    xmlBuffer.close();
}

//FUNCTION writeToCache
//
//  *xmlData : QByteArray containing the XML byte stream retrieved through the API
//             call
//
//  Writes the data to a local cache file called cache.xml found in the same location
//  as the configuration file.
//

bool WebData::writeToCache(QByteArray *xmlData)
{
    QFile cacheFile(m_globalSettings->dataPath() +"/cache.xml");

    bool result = cacheFile.open(QFile::WriteOnly);

    if(!result)
    {
        qDebug() << "Cannot open cache file for writing!";
        return false;
    }

    int bytesWritten = cacheFile.write(*xmlData);

    if(bytesWritten == -1)
    {
        qDebug() << "Cannot write to cache file!";
        return false;
    }

    cacheFile.close();
    //if we made it this far things are looking good, return true

    return true;
}

//FUNCTION readFromCache
//
//  *xmlData : QByteArray to read the local XML byte stream to.
//  alternateCacheFile: QString that can contain a different cache to read from,
//                      by default it will read from the same as the write function
//
//  Reads the xml data from a local cache or if desired will read from somewhere else.
//  like a resource file.

bool WebData::readFromCache(QByteArray *xmlData, QString alternateCacheFile)
{
    QFile cacheFile;

    if(alternateCacheFile.length() > 0)
        cacheFile.setFileName(alternateCacheFile);
    else
        cacheFile.setFileName(m_globalSettings->dataPath() +"/cache.xml");

    bool result = cacheFile.open(QFile::ReadOnly);

    if(!result)
    {
        qDebug() << "XMLCACHE: Cannot open cache file for reading!";
        return false;
    }

    //no way to report errors, 64kb is much bigger than we expect(~4-5 kb)
    *xmlData = cacheFile.read(65535);

    //we can at least assume if the size is zero, something is not right.
    if(xmlData->size() == 0)
    {
        qDebug() << "XMLCACHE: No data loaded...";
        return false;
    }

    cacheFile.close();
    //if we made it this far things are looking good, return true

    return true;
}

QString WebData::date()
{
    // return current date in provided city
    return currentDate;
}

QString WebData::time()
{
    // return local time in provided city
    return clock.toString("h:mm AP");
}

QString WebData::temp()
{
    // return current temp in Celsius or Fehrenheit
    return unitConversion(currentTemp);
}

QString WebData::currentIcon()
{
    // return string for what current icon should be
    return forecastIconList.at(0);
}

QString WebData::day1Icon()
{
    // return string for what day 1 icon should be
    return forecastIconList.at(1);
}

QString WebData::day2Icon()
{
    // return string for what day 2 icon should be
    return forecastIconList.at(3);
}

QString WebData::day3Icon()
{
    // return string for what day 3 icon should be
    return forecastIconList.at(5);
}

QString WebData::day1()
{
    // return day 1 abbreviation (i.e. Wed)
    return weekdayList.at(0);
}

QString WebData::day2()
{
    // return day 2 abbreviation
    return weekdayList.at(1);
}

QString WebData::day3()
{
    // return day 3 abbreviation
    return weekdayList.at(2);
}

QString WebData::day1High()
{
    // return day 1 high
    return unitConversion(forecastTempList.at(0));
}

QString WebData::day1Low()
{
    // return day 1 low
    return unitConversion(forecastTempList.at(1));
}

QString WebData::day2High()
{
    // return day 2 high
    return unitConversion(forecastTempList.at(2));
}

QString WebData::day2Low()
{
    // return day 2 low
    return unitConversion(forecastTempList.at(3));
}

QString WebData::day3High()
{
    // return day 3 high
    return unitConversion(forecastTempList.at(4));
}

QString WebData::day3Low()
{
    // return day 3 low
    return unitConversion(forecastTempList.at(5));
}

QTime WebData::clockObject()
{
    // return clock object itself (not string with time)
    return clock;
}

void WebData::changeCity(QString city)
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

QString WebData::unitConversion(QString currentValue)
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
