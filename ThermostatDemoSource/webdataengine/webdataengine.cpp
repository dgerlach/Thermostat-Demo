#include "webdataengine.h"

#include "globalsettings.h"

/***********************************************************************************************************
* WebDataEngine
*   Abstract class that allows for implementation of different methods for retrieving API data from a web
*   server. The important methods that must be reimplemented are:
*
*   -
************************************************************************************************************/

WebDataEngine::WebDataEngine(QNetworkAccessManager *manager, QObject *parent) :
    QObject(parent)
{
    m_manager = manager;
    m_globalSettings = GlobalSettings::getInstance();
}
