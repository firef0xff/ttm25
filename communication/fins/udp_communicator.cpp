#include "udp_communicator.h"

namespace fins
{

UDP_Communicator::UDP_Communicator( QString host, qint16 port ):
    mHost(host),
    mPort(port),
    mSocket( this ),
    mReaded( 0 )
{
    mSocket.bind( mHost, mPort );

    QObject::connect(&mSocket, &QUdpSocket::readyRead, this, &UDP_Communicator::read);
}
UDP_Communicator::~UDP_Communicator()
{
    QObject::disconnect(&mSocket, &QUdpSocket::readyRead, this, &UDP_Communicator::read);
    mSocket.close();
}

void UDP_Communicator::read()
{
    auto data_size = mSocket.pendingDatagramSize();
    mAnswer.resize( data_size );
    mSocket.readDatagram( mAnswer.data() + mReaded, mAnswer.size() - mReaded );
    mReaded += data_size;

    if ( mRequest && mReaded >= mRequest->ResponseSize() )
    {
        mRequest->SetResponce( reinterpret_cast<uint8_t const*>( mAnswer.data() ), mAnswer.size() );
        mAnswer.clear();
        mReaded = 0;
        mCond.notify_all();
    }
}

void UDP_Communicator::send( Paskage& pkg )
{    
//    сохраняем запрос в мапе ожидающих ответа
    mRequest = &pkg;
//    шлем запрос
    mSocket.writeDatagram( reinterpret_cast<char const *>( pkg.Data() ), pkg.Size(), mHost, mPort );
////    ждем ответ
    std::unique_lock< std::mutex > lock( mMutex );
    mCond.wait( lock );
    mRequest = nullptr;
}

}//namespace fins

#include "moc_udp_communicator.cpp"
