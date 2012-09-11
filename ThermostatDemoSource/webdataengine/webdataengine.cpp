#include "webdataengine.h"

#include "globalsettings.h"

WebDataEngine::WebDataEngine(QNetworkAccessManager *manager, QObject *parent) :
    QObject(parent)
{
    m_manager = manager;
    m_globalSettings = GlobalSettings::getInstance();
}
