#include "utilities.h"

//FUNCTION: formatTemperature
//  int temp - temperature value in F
//  GlobalSettings::TemperatureFormat -
//all temperatures are stored internally as fahrenheit so return number if no format conversion
//needed, otherwise convert to celsius
int formatTemperature(int temp, GlobalSettings::TemperatureFormat tempFormat)
{
    if(tempFormat == GlobalSettings::TempFormatC)
        return (float)(temp-32)*(0.555555555);
    else
        return temp;
}

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
