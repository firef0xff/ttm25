#pragma once
#include <QString>
#include <stdint.h>
#include <fins/communicator.h>
#include <fins/endpoint.h>

namespace fins
{

class Command;
}

namespace cpu
{

class NetConnection
{
public:
    ~NetConnection();
    static void Init(   fins::EndPoint source,
                        fins::EndPoint dest,
                        QString host = QString("192.168.250.1"),
                        int16_t port = 9600 );

    static void Execute(fins::Command &cmd );
    static fins::Communicator& Communicator();
private:
    NetConnection( fins::EndPoint source, fins::EndPoint dest,
                   QString host,  int16_t port );

    fins::Communicator mConnection;
    fins::EndPoint mSource;
    fins::EndPoint mDest;
};

}

