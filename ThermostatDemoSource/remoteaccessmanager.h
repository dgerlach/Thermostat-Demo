#ifndef REMOTEACCESSMANAGER_H
#define REMOTEACCESSMANAGER_H

#include <QHash>
#include <QObject>
#include <QVariant>

class GlobalSettings;
class QTcpServer;
class QTcpSocket;

class RemoteAccessManager : public QObject
{
    Q_OBJECT
public:
    explicit RemoteAccessManager(QObject *parent = 0);
    ~RemoteAccessManager();
    
    void start();
    void stop();

signals:
    void remoteChangeReceived(QHash<QString, QVariant> getVariables);
    
public slots:
    void handleIncomingConnection();
    void processIncomingConnection();

private:
    QTcpServer* m_tcpServer;
    qint16 m_listenPort;

    GlobalSettings* m_globalSettings;
    QTcpSocket* m_clientConnection;

    QByteArray hashToJSONByteArray(QHash<QString, QVariant> hash);

    QHash<QString, QVariant> m_oldData;
};

#endif // REMOTEACCESSMANAGER_H
