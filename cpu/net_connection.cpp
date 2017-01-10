#include "net_connection.h"

#include <communication/fins/endpoint.h>

#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

namespace cpu
{
namespace
{
static std::unique_ptr< NetConnection > NET_CONNECTION;
static std::mutex NET_CONNECTION_MUTEX;
}//namespace

NetConnection::NetConnection( fins::EndPoint source, fins::EndPoint dest,
                              QString host,  int16_t port ):
    mConnection( host, port ),
    mSource( source ),
    mDest( dest )
{

}

NetConnection::~NetConnection()
{

}

void NetConnection::Init(fins::EndPoint source, fins::EndPoint dest,
                         QString host,  int16_t port )
{
    if ( !NET_CONNECTION )
    {
        std::lock_guard< std::mutex > lock( NET_CONNECTION_MUTEX );
        if ( !NET_CONNECTION )
        {
            NET_CONNECTION.reset( new NetConnection( source, dest, host, port ) );
        }
    }
}

void NetConnection::Execute( fins::Command& cmd )
{
    if ( !NET_CONNECTION )
        throw std::runtime_error("Connection not inited");

    NetConnection& con = *NET_CONNECTION;

    fins::EndPoint source( fins::EndPoint::NA_LOCAL, 1, fins::EndPoint::A2_CPU );
    fins::EndPoint dest( fins::EndPoint::NA_LOCAL, 1, fins::EndPoint::A2_CPU );

    fins::Paskage pkg( con.mDest, con.mSource, cmd );

    std::lock_guard< std::mutex > lock( NET_CONNECTION_MUTEX );

    std::this_thread::sleep_for( std::chrono::milliseconds(100) );
#ifndef DEMO
    con.mConnection.Send( pkg );
#endif
}

fins::Communicator& NetConnection::Communicator()
{
    if ( !NET_CONNECTION )
        throw std::runtime_error("Connection not inited");

    NetConnection& con = *NET_CONNECTION;
    return con.mConnection;
}

}
