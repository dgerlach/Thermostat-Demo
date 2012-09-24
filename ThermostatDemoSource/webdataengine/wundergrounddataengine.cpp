#include "wundergrounddataengine.h"

#include <QBuffer>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QStringList>
#include <QtDebug>
#include <QXmlStreamReader>

#include "forecastdata.h"
#include "globalsettings.h"
#include "weatherdata.h"

#define WUNDERGROUND_API_KEY "9d8a29de9939cbb7"

WundergroundDataEngine::WundergroundDataEngine(QNetworkAccessManager *manager, QObject *parent) :
    WebDataEngine(manager, parent)
{
}

void WundergroundDataEngine::setCity(QString city)
{
    m_fullCity = city;
    QStringList cityList;
    cityList = city.split(",");
    cityList.replaceInStrings(" ","_");
    m_preparedCity = cityList.at(1) + "/" + cityList.at(0);
}

void WundergroundDataEngine::dispatchRequest()
{
    QString cityUrl = "http://api.wunderground.com/api/"+QString(WUNDERGROUND_API_KEY)+"/geolookup/conditions/forecast/q/" + m_preparedCity + ".xml";

    // receive document and parse it
    QNetworkRequest request;
    request.setUrl(QUrl(cityUrl));

    qDebug() << cityUrl;

    //set up timer to check for network timeout
    connect(&m_networkTimer, SIGNAL(timeout()), this, SLOT(handleNetworkTimeout()));
    m_networkTimer.start(15000);

    //make actual network request
    m_reply = m_manager->get(request);
    connect(m_reply, SIGNAL(finished()),this,SLOT(responseReceived()));
}

void WundergroundDataEngine::parseXML(QByteArray* xmlData, bool cached)
{
    QBuffer xmlBuffer(xmlData);
    WeatherData *weatherData = NULL;
    ForecastData *forecastData = NULL;
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
            //qDebug() << name;
            if (name == "current_observation") {
                weatherData = parseXMLWeatherData(reader);
            }
            else if (name == "simpleforecast") {
                while(!reader.hasError() && !reader.atEnd())
                {
                    reader.readNext();
                    name = reader.name().toString();
                    if(name == "forecastday")
                    {
                        forecastData = parseXMLForecastData(reader);
                        weatherData->addForecastDay(forecastData);
                    }
                    if(name == "simpleforecast" && reader.isEndElement())
                        break;
                }

            }
        }
    }

    if (reader.hasError()) {
        qDebug() << "XML error: " << reader.errorString() << endl;
    }
    else if (reader.atEnd()) {
        qDebug() << "Reached end of XML document" << endl;
    }
    if(cached)
        weatherData->setCachedDataFlag();

    emit dataAvailable(weatherData);
    xmlBuffer.close();
}

WeatherData* WundergroundDataEngine::parseXMLWeatherData(QXmlStreamReader& reader)
{
    WeatherData *weatherData = new WeatherData;

    while(!reader.hasError() && !reader.atEnd())
    {
        reader.readNext();
        QString name = reader.name().toString();

        if (name == "local_time_rfc822") {
            QString rfc822TimeString = reader.readElementText();
            int timezoneIndex = rfc822TimeString.lastIndexOf(" ");
            QDateTime dateTime = QDateTime::fromString(rfc822TimeString.left(timezoneIndex), "ddd, dd MMM yyyy HH:mm:ss");
            dateTime.setTimeSpec(Qt::OffsetFromUTC);
            dateTime.setUtcOffset(rfc822TimeString.mid(timezoneIndex+1,2).toInt()*3600);
            weatherData->setLocalTime(dateTime);

        }
        else if (name == "temp_f") {
            //weird Qt thing, have to convert to float to avoid a zero then cast to int????
            weatherData->setCurrentTemp((int)(reader.readElementText().toFloat()+0.5));
        }
        else if (name == "icon") {
            weatherData->setIcon(reader.readElementText());
        }
        else if (name == "display_location") {
            while(!reader.hasError() && !reader.atEnd())
            {
                reader.readNext();
                name = reader.name().toString();
                if(name == "full")
                    weatherData->setCurrentCity(reader.readElementText());
                if(name == "display_location" && reader.isEndElement())
                    break;
            }
        }
        else if(name == "current_observation" && reader.isEndElement())
            break;
    }

    //set the current time so we know when this occurred
    weatherData->setLastUpdated(QDateTime::currentDateTime());

    return weatherData;
}

ForecastData* WundergroundDataEngine::parseXMLForecastData(QXmlStreamReader& reader)
{
    ForecastData *forecastData = new ForecastData;

    while(!reader.hasError() && !reader.atEnd())
    {
        reader.readNext();
        QString name = reader.name().toString();

        if (name == "icon") {
            forecastData->setIcon(reader.readElementText());
        }
        else if (name == "high") {
            while(!reader.hasError() && !reader.atEnd())
            {
                reader.readNext();
                name = reader.name().toString();
                if(name == "fahrenheit")
                    forecastData->setHighTemp((int)(reader.readElementText().toFloat()+0.5));
                if(name == "high" && reader.isEndElement())
                    break;
            }
        }
        else if (name == "low") {
            while(!reader.hasError() && !reader.atEnd())
            {
                reader.readNext();
                name = reader.name().toString();
                if(name == "fahrenheit")
                    forecastData->setLowTemp((int)(reader.readElementText().toFloat()+0.5));
                if(name == "low" && reader.isEndElement())
                    break;
            }
        }
        else if (name == "date") {
            while(!reader.hasError() && !reader.atEnd())
            {
                reader.readNext();
                name = reader.name().toString();
                if(name == "weekday_short")
                    forecastData->setDay(reader.readElementText());
                if(name == "date" && reader.isEndElement())
                    break;
            }
        }
        else if(name == "forecastday" && reader.isEndElement())
            break;
    }

    return forecastData;
}

//FUNCTION: responseReceived()
//
//  Slot that handles the reply from the QNetworkAccessManager. Also triggered
//  in the event of an error, both success and fail cases are handled.
//  Either grabs the data returned from the request, writes it to a local cache
//  and then parses it or tries to read from a previously written cache file and
//  then parses that.
//

void WundergroundDataEngine::responseReceived()
{
    m_networkTimer.stop();

    if(m_reply->error() == QNetworkReply::NoError)
    {
        QByteArray xmlData = m_reply->readAll();
        writeToCache(&xmlData);

        parseXML(&xmlData);
    }
    else
    {
        qDebug() << m_reply->errorString();
        loadLocalData();
        emit networkTimeout();
    }

    //docs say do not delete in the slot so well pass it off to the event loop
    m_reply->deleteLater();
}

void WundergroundDataEngine::loadLocalData()
{
    QByteArray xmlData;

    bool result = readFromCache(&xmlData);
    //if we can't read from the cache file, read from the one included in the qrc!
    if(!result)
        readFromCache(&xmlData, ":/data/cache.xml");

    parseXML(&xmlData, true);
}

//FUNCTION writeToCache
//
//  *xmlData : QByteArray containing the XML byte stream retrieved through the API
//             call
//
//  Writes the data to a local cache file called cache.xml found in the same location
//  as the configuration file.
//

bool WundergroundDataEngine::writeToCache(QByteArray *xmlData)
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

bool WundergroundDataEngine::readFromCache(QByteArray *xmlData, QString alternateCacheFile)
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

QString WundergroundDataEngine::licenseString()
{
    return QString("Weather data from wunderground.com.");
}
