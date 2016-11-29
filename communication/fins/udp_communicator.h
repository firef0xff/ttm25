#pragma once
#include <QUdpSocket>
#include <QObject>
#include "paskage.h"
#include <memory>
#include <mutex>
#include <condition_variable>

namespace fins
{

class UDP_Communicator :public QObject
{
    Q_OBJECT
public:
    UDP_Communicator( QString host = QString("192.168.250.1"), qint16 port = 9600 );
    ~UDP_Communicator();

    void send( Paskage& pkg );

private:
    QHostAddress mHost;
    qint16 mPort;
    QUdpSocket mSocket;

    Paskage* mRequest;
    QByteArray mAnswer;
    size_t mReaded = 0;

    std::mutex mMutex;
    std::condition_variable mCond;
private slots:
    void read();
};

}
