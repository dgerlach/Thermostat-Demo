#include "openweathermapdataengine.h"

#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScriptEngine>
#include <QtScript>
#include <QtDebug>

#include "globalsettings.h"
#include "forecastdata.h"
#include "weatherdata.h"

OpenWeatherMapDataEngine::OpenWeatherMapDataEngine(QNetworkAccessManager *manager, QObject *parent) :
    WebDataEngine(manager, parent)
{
    generateJSONWeatherLookupTables();
    connect(&m_networkTimer, SIGNAL(timeout()), this, SLOT(handleNetworkTimeout()));
    connect(&m_forecastNetworkTimer, SIGNAL(timeout()), this, SLOT(handleNetworkTimeout()));
}

void OpenWeatherMapDataEngine::setCity(QString city)
{
    //save the full city name for use later
    m_fullCity = city;
    //openweathermap searches for a city name alone, so chop anything attached by commas
    m_preparedCity = city.left(city.lastIndexOf(","));
}

void OpenWeatherMapDataEngine::dispatchRequest()
{
    m_weatherData = new WeatherData;
    m_forecastReceived = false;
    m_weatherReceived = false;

    //for openweather map we first must find the city
    QString cityUrl = "http://openweathermap.org/data/2.1/find/name?q="+m_preparedCity;

    // receive document and parse it
    QNetworkRequest request;
    request.setUrl(QUrl(cityUrl));

    //set up timer to check for network timeout
    connect(&m_networkTimer, SIGNAL(timeout()), this, SLOT(handleNetworkTimeout()));
    m_networkTimer.start(15000);

    //make actual network request
    m_reply = m_manager->get(request);

    connect(m_reply, SIGNAL(finished()),this,SLOT(responseReceived()));
}

void OpenWeatherMapDataEngine::handleNetworkTimeout()
{
    m_networkTimer.stop();
    m_forecastNetworkTimer.stop();
    emit(networkTimeout());
}

void OpenWeatherMapDataEngine::responseReceived()
{
    m_networkTimer.stop();

    if(m_reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = m_reply->readAll();
        //qDebug() << data;

        m_cityId = parseCityInformation(&QString::fromAscii(data));
        //docs say do not delete in the slot so well pass it off to the event loop
        m_reply->deleteLater();
        if(m_cityId == -1)
            emit networkTimeout();
        else
            //now we need to grab current weather information and forecast data
            dispatchWeatherDataRequests();
    }
    else
    {
        qDebug() << "Network Error: " << m_reply->errorString();
        loadLocalData();
        emit networkTimeout();
    }
}

void OpenWeatherMapDataEngine::dispatchWeatherDataRequests()
{

    m_weatherData->setCurrentCity(m_fullCity);

    //first send request for current weather
    QString currentWeatherURL = "http://openweathermap.org/data/2.1/weather/city/"+QString::number(m_cityId);

    QNetworkRequest request;
    request.setUrl(QUrl(currentWeatherURL));

    //set up timer to check for network timeout

    m_networkTimer.start(15000);

    //make actual network request
    m_reply = m_manager->get(request);
    connect(m_reply,SIGNAL(finished()),this,SLOT(currentWeatherResponseReceived()));

    //next send request for current weather
    QString forecastWeatherURL = "http://openweathermap.org/data/2.1/forecast/city/"+QString::number(m_cityId);
    qDebug() << "Forecast url: " << forecastWeatherURL;
    request.setUrl(QUrl(forecastWeatherURL));

    //set up timer to check for network timeout
    m_forecastNetworkTimer.start(15000);

    //make actual network request
    m_forecastReply = m_manager->get(request);
    connect(m_forecastReply,SIGNAL(finished()),this, SLOT(forecastResponseReceived()));
}

void OpenWeatherMapDataEngine::currentWeatherResponseReceived()
{
    m_networkTimer.stop();

    if(m_reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network Error: " << m_reply->errorString();
        emit networkTimeout();
        m_reply->deleteLater();
        return;
    }
    m_rawJSONWeatherString = m_reply->readAll();
    bool result = parseJSONWeatherData(&m_rawJSONWeatherString, m_weatherData);
    if(!result)
        emit networkTimeout();
    else
    {
        m_weatherReceived = true;
        checkIfDone();
    }
}

void OpenWeatherMapDataEngine::forecastResponseReceived()
{
    if(m_forecastReply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network Error: " << m_reply->errorString();
        emit networkTimeout();
        m_forecastReply->deleteLater();
        return;
    }
    m_forecastNetworkTimer.stop();
    m_rawJSONForecastString = m_forecastReply->readAll();
    bool result = parseJSONForecastData(&m_rawJSONForecastString, m_weatherData);
    if(!result)
        emit networkTimeout();
    else
    {
        m_forecastReceived = true;
        checkIfDone();
    }
}

void OpenWeatherMapDataEngine::checkIfDone()
{
    if(m_forecastReceived && m_weatherReceived)
    {
        m_weatherData->setLastUpdated(QDateTime::currentDateTime());
        writeToCache();
        emit(dataAvailable(m_weatherData));
    }
}

bool OpenWeatherMapDataEngine::parseJSONWeatherData(QString *jsonData, WeatherData *weatherData)
{
    QScriptEngine engine;
    QScriptValue result = engine.evaluate("weatherObject="+*jsonData);

    qDebug() << *jsonData;

    if(result.isError())return false;

    int temp = kelvinToFahrenheit(result.property("main").property("temp").toNumber());
    QDateTime localTime = QDateTime::fromTime_t(result.property("dt").toNumber());

    int iconIndex = convertImageNameToIndex(result.property("weather").property(0).property("icon").toString());

    QString icon = m_iconNameToWeatherHash[iconIndex];

    weatherData->setCurrentTemp(temp);
    weatherData->setLocalTime(localTime);
    weatherData->setIcon(icon);
    return true;
}

bool OpenWeatherMapDataEngine::parseJSONForecastData(QString *jsonData, WeatherData* weatherData)
{
    QScriptEngine engine;
    QScriptValue result = engine.evaluate("weatherObject="+*jsonData);


    if(result.isError())return false;

    QScriptValueIterator it(result.property("list"));

    int high = 0;
    int low = 500;
    QString icon = "";

    QDateTime workingDateTime;
    while (it.hasNext()) {
        it.next();
        //qDebug() << it.name() << it.value().toString() << it.value().property("img").toNumber();

        QDateTime dateTime = QDateTime::fromTime_t(it.value().property("dt").toNumber());

        //on the first iteration initialize the workingDateTime to parsedDateTime so we dont end up with a blank first day
        if(workingDateTime.isNull())
            workingDateTime = dateTime;

        if(workingDateTime.date().day() == dateTime.date().day())
        {
            if(high < it.value().property("main").property("temp_max").toNumber()) high = it.value().property("main").property("temp_max").toInteger();
            if(low > it.value().property("main").property("temp_min").toNumber()) low = it.value().property("main").property("temp_min").toInteger();

            int iconIndex = convertImageNameToIndex(it.value().property("weather").property(0).property("icon").toString());

            if(m_weatherPriorityHash[icon]< m_weatherPriorityHash[m_iconNameToWeatherHash[iconIndex]]) icon = m_iconNameToWeatherHash[iconIndex];

        }
        else
        {
            ForecastData *forecastDay = new ForecastData;
            forecastDay->setDay(workingDateTime.date().toString("ddd"));
            forecastDay->setHighTemp(kelvinToFahrenheit(high));
            forecastDay->setLowTemp(kelvinToFahrenheit(low));
            forecastDay->setIcon(icon);
            weatherData->addForecastDay(forecastDay);

            //initialize for next day of parsing;
            workingDateTime = dateTime;
            high = 0;
            low=500;
            icon = "";
        }
    }
    return true;
}


void OpenWeatherMapDataEngine::generateJSONWeatherLookupTables()
{
    m_iconNameToWeatherHash.insert(1, "sunny");
    m_iconNameToWeatherHash.insert(2, "partlysunny");
    m_iconNameToWeatherHash.insert(3, "cloudy");
    m_iconNameToWeatherHash.insert(4, "cloudy");
    m_iconNameToWeatherHash.insert(5, "rain");
    m_iconNameToWeatherHash.insert(6, "tstorms");
    m_iconNameToWeatherHash.insert(7, "sleet");
    m_iconNameToWeatherHash.insert(8, "snow");
    m_iconNameToWeatherHash.insert(9, "rain");
    m_iconNameToWeatherHash.insert(10, "rain");
    m_iconNameToWeatherHash.insert(11, "tstorms");
    m_iconNameToWeatherHash.insert(12, "sleet");
    m_iconNameToWeatherHash.insert(13, "snow");
    m_iconNameToWeatherHash.insert(14, "snow");
    m_iconNameToWeatherHash.insert(15, "cloudy");
    m_iconNameToWeatherHash.insert(16, "sunny");
    m_iconNameToWeatherHash.insert(17, "partlysunny");
    m_iconNameToWeatherHash.insert(18, "rain");
    m_iconNameToWeatherHash.insert(19, "snow");


    m_weatherPriorityHash.insert("", 0);
    m_weatherPriorityHash.insert("sunny", 1);
    m_weatherPriorityHash.insert("partlysunny", 2);
    m_weatherPriorityHash.insert("cloudy", 3);
    m_weatherPriorityHash.insert("rain", 4);
    m_weatherPriorityHash.insert("tstorms", 5);
    m_weatherPriorityHash.insert("sleet", 6);
    m_weatherPriorityHash.insert("snow", 7);
}


qlonglong OpenWeatherMapDataEngine::parseCityInformation(QString* jsonData)
{
    QScriptEngine engine;

    //must have an object set equal to the class data received from the web or qt throws parse error
    QScriptValue result = engine.evaluate("weatherObject="+*jsonData);

    if(result.property("message").toString() != "")
        return -1;
    else
        return result.property("list").property("0").property("id").toInteger();
}

int OpenWeatherMapDataEngine::kelvinToFahrenheit(double k)
{
    return qRound((k-273.15)*1.8+32);
}

int OpenWeatherMapDataEngine::convertImageNameToIndex(QString img)
{
    QString iconName = img;

    int lastSlash = iconName.lastIndexOf("/");
    int lastDot = iconName.lastIndexOf(".");
    int endShift=1;

    if(lastSlash == -1 && lastDot == -1 && iconName[iconName.size()-1].isLetter())
        return iconName.mid(0,iconName.size()-1).toInt();

    if(iconName[lastDot-1].isLetter())
        endShift = 2;

   return  iconName.mid(lastSlash+1, lastDot-lastSlash-endShift).toInt();
}

void OpenWeatherMapDataEngine::loadLocalData()
{
    m_weatherData = new WeatherData;
    m_weatherData->setCachedDataFlag();

    bool result = readFromCache();
    //if we can't read from the cache file, read from the one included in the qrc!
    if(!result)
        readFromCache(":/data/cache.dat");

    if(!parseJSONWeatherData(&m_rawJSONWeatherString, m_weatherData))
    {
        emit networkTimeout();
    }
    else if(!parseJSONForecastData(&m_rawJSONForecastString, m_weatherData))
    {
        emit networkTimeout();
    }
    else
    {
        emit(dataAvailable(m_weatherData));
    }
}

//FUNCTION writeToCache
//
//  *xmlData : QByteArray containing the XML byte stream retrieved through the API
//             call
//
//  Writes the data to a local cache file called cache.xml found in the same location
//  as the configuration file.
//

bool OpenWeatherMapDataEngine::writeToCache()
{
    QFile cacheFile(m_globalSettings->dataPath() +"/cache.dat");

    bool result = cacheFile.open(QFile::WriteOnly);

    if(!result)
    {
        qDebug() << "Cannot open cache file for writing!";
        return false;
    }

    QDataStream stream(&cacheFile);
    stream.setVersion(QDataStream::Qt_4_6);
    stream << m_weatherData->currentCity();
    stream << m_weatherData->lastUpdated();
    stream << m_rawJSONWeatherString;
    stream << m_rawJSONForecastString;

    if(cacheFile.error() != QFile::NoError)
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
//  *xmlData : QByteArray to read the local json stream to.
//  alternateCacheFile: QString that can contain a different cache to read from,
//                      by default it will read from the same as the write function
//
//  Reads the xml data from a local cache or if desired will read from somewhere else.
//  like a resource file.

bool OpenWeatherMapDataEngine::readFromCache(QString alternateCacheFile)
{
    QFile cacheFile;

    if(alternateCacheFile.length() > 0)
        cacheFile.setFileName(alternateCacheFile);
    else
        cacheFile.setFileName(m_globalSettings->dataPath() +"/cache.dat");

    bool result = cacheFile.open(QFile::ReadOnly);

    if(!result)
    {
        qDebug() << "XMLCACHE: Cannot open cache file for reading!";
        return false;
    }

    QDataStream stream(&cacheFile);
    stream.setVersion(QDataStream::Qt_4_6);
    QString cityString;
    stream >> cityString;
    m_weatherData->setCurrentCity(cityString);
    QDateTime lastUpdated;
    stream >> lastUpdated;
    m_weatherData->setLastUpdated(lastUpdated);
    stream >> m_rawJSONWeatherString;
    stream >> m_rawJSONForecastString;

    //we can at least assume if the size is zero, something is not right.
    if(cacheFile.error() != QFile::NoError)
    {
        qDebug() << "XMLCACHE: No data loaded...";
        return false;
    }

    cacheFile.close();
    //if we made it this far things are looking good, return true

    return true;
}

QString OpenWeatherMapDataEngine::licenseString()
{
    return QString("Weather data from  openweathermap.org under CC-BY-SA");
}
