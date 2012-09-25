#include "utilities.h"

//FUNCTION: formatTemperature
//  int temp - temperature value in F
//  GlobalSettings::TemperatureFormat - enum that determines output format
//all temperatures are stored internally as fahrenheit so return number if no format conversion
//needed, otherwise convert to celsius
int formatTemperature(int temp, GlobalSettings::TemperatureFormat tempFormat)
{
    if(tempFormat == GlobalSettings::TempFormatC)
        return (float)(temp-32)*(0.555555555);
    else
        return temp;
}

//FUNCTION: formatTemperature
//  int temp - temperature value in F
//  GlobalSettings::TemperatureFormat - enum that determines output format
//  bool includeUnits - specifies whether or not to include degree symbol
//all temperatures are stored internally as fahrenheit so return number if no format conversion
//needed, otherwise convert to celsius. returns as string
QString formatTemperatureString(int temp, GlobalSettings::TemperatureFormat tempFormat, bool includeUnits)
{
    QString tempString = QString::number(formatTemperature(temp, tempFormat));

    if(includeUnits)
    {
        if(tempFormat == GlobalSettings::TempFormatC)
            tempString += QString::fromUtf8("°");
        else
            tempString += QString::fromUtf8("°");
    }
    return tempString;
}

QString formatTimeString(QTime time, GlobalSettings::TimeFormat timeFormat, bool includeAMPM)
{
    if(timeFormat == GlobalSettings::TimeFormat24h)
        return time.toString("hh:mm");
    else if( timeFormat == GlobalSettings::TimeFormat12h && !includeAMPM )
        return time.toString("h:mm AP").left(5);
    else
        return time.toString("h:mm AP");
}

QString formatHourString(QTime time, GlobalSettings::TimeFormat timeFormat, bool includeAMPM)
{
    if(timeFormat == GlobalSettings::TimeFormat24h)
        return time.toString("hh:mm");
    else if( timeFormat == GlobalSettings::TimeFormat12h && !includeAMPM )
        return time.toString("h AP").left(5);
    else
        return time.toString("h AP");
}
