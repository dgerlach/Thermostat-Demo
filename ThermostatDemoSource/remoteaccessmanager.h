#ifndef REMOTEACCESSMANAGER_H
#define REMOTEACCESSMANAGER_H

#include <QObject>

class GlobalSettings;
class QTcpServer;
class QTcpSocket;

class RemoteAccessManager : public QObject
{
    Q_OBJECT
public:
    explicit RemoteAccessManager(QObject *parent = 0);
    
    void start();

signals:
    
public slots:
    void handleIncomingConnection();
    void processIncomingConnection();

private:
    QTcpServer* m_tcpServer;
    qint16 m_listenPort;

    GlobalSettings* m_globalSettings;
    QTcpSocket* m_clientConnection;
};

#endif // REMOTEACCESSMANAGER_H
