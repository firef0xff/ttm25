#pragma once
#include <QUdpSocket>
#include <QObject>

namespace fins
{

class UDP_Communicator :public QObject
{
    Q_OBJECT
public:
    UDP_Communicator( QString host = QString("192.168.250.1"), qint16 port = 9600 );
    ~UDP_Communicator();

    void send( const uint8_t *buf, size_t size );

private:
    QHostAddress mHost;
    qint16 mPort;
    QUdpSocket mSocket;

private slots:
    void read();
};

}
