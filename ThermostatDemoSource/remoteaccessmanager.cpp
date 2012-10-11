#include "remoteaccessmanager.h"

#include <QDateTime>
#include <QFile>
#include <QHash>
#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>

#include "globalsettings.h"
#include "mainwindow.h"

/***********************************************************************************************************
* RemoteAccessManager
*   Class that has instantiates an HTTP server using a QTcpServer and listens for connections.
*   Serves a page that allows for remote configuration of the thermostat though ajax calls
************************************************************************************************************/

RemoteAccessManager::RemoteAccessManager(QObject *parent) :
    QObject(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    m_listenPort = 8081;
    m_tcpServer = new QTcpServer;
}

RemoteAccessManager::~RemoteAccessManager()
{
    stop();
    if(m_tcpServer) delete m_tcpServer;
}

//Function: start()
//
//  starts the tcpserver listening on specified port and make connections for incoming connections

void RemoteAccessManager::start()
{
    qDebug() << m_tcpServer->listen(QHostAddress::Any, m_listenPort);
    qDebug() << m_tcpServer->errorString();
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(handleIncomingConnection()));
}

void RemoteAccessManager::stop()
{
    m_tcpServer->close();
}

//Function: handleIncomingConnection()
//
//  accepts an incoming connection from the tcpserver and makes a connection for when the socket
//  has received data from the client (presumably a web server)

void RemoteAccessManager::handleIncomingConnection()
{
    m_clientConnection = m_tcpServer->nextPendingConnection();
    connect(m_clientConnection, SIGNAL(readyRead()), this, SLOT(processIncomingConnection()));
    if(!m_clientConnection)
    {
        qDebug() << "Error connecting to client";
        return;
    }

}

//Function: processIncomingConnection()
//
//  once a new socket has received data, parse that data and thenserve back what was requested

void RemoteAccessManager::processIncomingConnection()
{
    QList<QByteArray> request;
    m_clientConnection = static_cast<QTcpSocket *>(sender());

    while(m_clientConnection->bytesAvailable())
        request << m_clientConnection->readLine(4096);

    QList<QByteArray> requestParser = request[0].split(0x20);
    QByteArray ba;

    QDateTime currentDateTime(QDateTime::currentDateTime());
    QString currentDateTimeString = currentDateTime.toString("ddd, dd MMM yyyy hh:mm:ss");

    if(requestParser[0] == "GET")
    {
        if(requestParser[1] == "/")
        {
            QFile htmlFile(":/remote/index.html");

            if(!htmlFile.open(QFile::ReadOnly))
                ba = "HTTP/1.1 404 NOT FOUND";
            else
            {
                ba = "HTTP/1.1 200 OK\r\n";
                ba += "Date: "+currentDateTimeString+"\r\n";
                ba += "Content-Type: text/html; charset=utf-8\r\n";
                ba += "Content-Length: "+QByteArray::number(htmlFile.size())+"\r\n";
                ba += "\r\n";
                ba += htmlFile.readAll();
            }
        }
        else if(requestParser[1].contains(".jpg") || requestParser[1].contains(".png"))
        {
            QFile imageFile(":"+QString::fromAscii(requestParser[1]));

            if(!imageFile.open(QFile::ReadOnly))
                ba = "HTTP/1.1 404 NOT FOUND";
            else
            {
                ba = "HTTP/1.1 200 OK\r\n";
                ba += "Date: "+currentDateTimeString+"\r\n";
                ba += "Content-Type: image/"+requestParser[1]+"\r\n";
                ba += "Content-Length: "+QByteArray::number(imageFile.size())+"\r\n";
                ba += "\r\n";
                ba+=imageFile.readAll();
            }
        }
        else if(requestParser[1].contains(".js"))
        {
            QFile jsFile(":"+QString::fromAscii(requestParser[1]));

            if(!jsFile.open(QFile::ReadOnly))
                ba = "HTTP/1.1 404 NOT FOUND";
            else
            {
                ba = "HTTP/1.1 200 OK\r\n";
                ba += "Date: "+currentDateTimeString+"\r\n";
                ba += "Content-Type: text/javascript; charset=utf-8\r\n";
                ba += "Content-Length: "+QByteArray::number(jsFile.size())+"\r\n";
                ba += "\r\n";
                ba+=jsFile.readAll();
            }
        }
        else if(requestParser[1].contains(".css"))
        {
            QFile cssFile(":"+QString::fromAscii(requestParser[1]));

            if(!cssFile.open(QFile::ReadOnly))
                ba = "HTTP/1.1 404 NOT FOUND";
            else
            {
                ba = "HTTP/1.1 200 OK\r\n";
                ba += "Date: "+currentDateTimeString+"\r\n";
                ba += "Content-Type: text/css; charset=utf-8\r\n";
                ba += "Content-Length: "+QByteArray::number(cssFile.size())+"\r\n";
                ba += "\r\n";
                ba+=cssFile.readAll();
            }
        }
        else if(requestParser[1].contains("?"))
        {
            //create a hash table so we can lookup based on variable name
            //ie getVar["command"] will give us the value set in the http request
            QHash<QString, QVariant> getVar;

            //let's parse the get variables

            QString getVarsString = requestParser[1].mid(requestParser[1].lastIndexOf("?")+1);

            foreach(QString keyValuePair, getVarsString.split("&"))
            {
                QStringList splitKeyValue = keyValuePair.split("=");
                getVar[splitKeyValue[0]]=splitKeyValue[1];
            }

            QHash<QString, QVariant> data, optimizedData;

            //QMetaObject::invokeMethod(parent(), Qt::BlockingQueuedConnection,  SLOT("remoteChangeReceived"), Q_RETURN_ARG(QHash<QString, QVariant>, data), Q_ARG(QHash<QString, QVariant>, getVar));
            data = static_cast<MainWindow*>(parent())->processCommand(getVar);



            optimizedData = data;

            qDebug() << (data["weatherCurrent"] == m_oldData["weatherCurrent"]);
            qDebug() << (data["weatherCurrent"]);
            qDebug() << m_oldData["weatherCurrent"];



            if(data["weatherCurrent"] == m_oldData["weatherCurrent"])
                optimizedData.remove("weatherCurrent");
            if(data["weatherForecast"] == m_oldData["weatherForecast"])
                optimizedData.remove("weatherForecast");
            if(getVar["command"] == "update")
                m_oldData = (data);

            QByteArray jsonByteArray;

            if(getVar["command"] == "update_forced")
                jsonByteArray=hashToJSONByteArray(data);
            else
                jsonByteArray=hashToJSONByteArray(optimizedData);

            ba = "HTTP/1.1 200 OK\r\n";
            ba += "Date: "+currentDateTimeString+"\r\n";
            ba += "Content-Type: text/html; charset=ISO-8859-1\r\n";
            ba += "Content-Length: "+QByteArray::number(jsonByteArray.size())+"\r\n";
            ba += "\r\n";
            ba += jsonByteArray;
        }
        else
            ba = "HTTP/1.1 404 NOT FOUND";
    }

    m_clientConnection->write(ba); //write the response
    m_clientConnection->disconnectFromHost(); //http specifies completion with a closed connection
}

QByteArray RemoteAccessManager::hashToJSONByteArray(QHash<QString, QVariant> hash)
{
    QByteArray jsonByteArray="{";
    QHashIterator<QString, QVariant> it(hash);


    while(it.hasNext())
    {
        it.next();
        if(it.value().type() == QVariant::Hash)
            jsonByteArray+="\""+it.key().toAscii()+"\""+QByteArray::fromRawData(":",1)+""+hashToJSONByteArray(it.value().toHash())+",";
        else
            jsonByteArray += "\""+it.key().toAscii()+"\""+QByteArray::fromRawData(":",1)+"\""+it.value().toByteArray()+"\""+QByteArray::fromRawData(",",1);
    }

    jsonByteArray = jsonByteArray.left(jsonByteArray.size()-1);

    //terminate the json object
    jsonByteArray += "}";

    return jsonByteArray;
}
