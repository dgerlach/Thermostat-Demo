#include "remoteaccessmanager.h"

#include <QDateTime>
#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>

#include "globalsettings.h"

/***********************************************************************************************************
* RemoteAccessManager
*   Class that has instantiates an HTTP server using a QTcpServer and listens for connections.
*   Serves a page that allows for remote configuration of the thermostat though ajax calls
************************************************************************************************************/

RemoteAccessManager::RemoteAccessManager(QObject *parent) :
    QObject(parent)
{
    m_globalSettings = GlobalSettings::getInstance();

    m_listenPort = 8945;
    m_tcpServer = new QTcpServer;
}

//Function: start()
//
//  starts the tcpserver listening on specified port and make connections for incoming connections

void RemoteAccessManager::start()
{
    m_tcpServer->listen(QHostAddress::Any, m_listenPort);
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(handleIncomingConnection()));
}

//Function: handleIncomingConnection()
//
//  accepts an incoming connection from the tcpserver and makes a connection for when the socket
//  has received data from the client (presumably a web server)

void RemoteAccessManager::handleIncomingConnection()
{
    qDebug() << "INCOMING CONNECTION";
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
                ba += "Content-Type: text/html\r\n";
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
                ba += "Content-Type: text/javascript\r\n";
                ba += "Content-Length: "+QByteArray::number(jsFile.size())+"\r\n";
                ba += "\r\n";
                ba+=jsFile.readAll();
            }
        }
        else
            ba = "HTTP/1.1 404 NOT FOUND";
    }

    m_clientConnection->write(ba); //write the response
    m_clientConnection->disconnectFromHost(); //http specifies completion with a closed connection
}
