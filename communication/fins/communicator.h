#pragma once
#include <QUdpSocket>
#include <QObject>
#include <QHostAddress>
#include "paskage.h"
#include <memory>
#include <mutex>
#include <condition_variable>

namespace fins
{

class Communicator :public QObject
{
    Q_OBJECT
public:
    Communicator( QString host = QString("192.168.250.1"), qint16 port = 9600 );
    ~Communicator();

    void send( Paskage& pkg );

private:
    QHostAddress mHost;
    qint16 mPort;
    QUdpSocket mSocket;

    std::mutex mMutex;
    std::condition_variable mCond;
private slots:
    void slotError       (QAbstractSocket::SocketError);

public slots:
    void slotSendToServer(Paskage &pkg);
};

}
