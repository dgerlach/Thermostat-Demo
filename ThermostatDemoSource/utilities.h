#ifndef UTILITIES_H
#define UTILITIES_H

#include "globalsettings.h"

#include <QString>
#include <QTime>

int formatTemperature(int temp, GlobalSettings::TemperatureFormat tempFormat);
QString formatTemperatureString(int temp, GlobalSettings::TemperatureFormat tempFormat, bool includeUnits = true);

QString formatTimeString(QTime &time, GlobalSettings::TimeFormat timeFormat);

#endif // UTILITIES_H
