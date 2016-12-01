#include "communicator.h"

namespace fins
{

Communicator::Communicator( QString host, qint16 port ):
    mHost(host),
    mPort(port),
    mSocket( this )
{

    connect(&mSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(slotError(QAbstractSocket::SocketError)));

//    mSocket.bind( mHost, mPort );
    mSocket.connectToHost( mHost, mPort );
}
Communicator::~Communicator()
{
    disconnect(&mSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                         this, SLOT(slotError(QAbstractSocket::SocketError)));
    mSocket.close();
}

void Communicator::slotError       ( QAbstractSocket::SocketError e )
{

//    QString strError =
//        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
//                     "The host was not found." :
//                     err == QAbstractSocket::RemoteHostClosedError ?
//                     "The remote host is closed." :
//                     err == QAbstractSocket::ConnectionRefusedError ?
//                     "The connection was refused." :
//                     QString(m_pTcpSocket->errorString())
//                    );
//    m_ptxtInfo->append(strError);
}
void Communicator::slotSendToServer( Paskage& pkg )
{
    mSocket.waitForConnected();
//    шлем запрос
    mSocket.write(reinterpret_cast<char const *>( pkg.Data() ), pkg.Size());

////    ждем ответ
    size_t readed = 0;
    QByteArray answer;
    auto resp_size = pkg.ResponseSize();
    while ( readed < resp_size )
    {
        mSocket.waitForReadyRead();
        auto data_size = mSocket.bytesAvailable();
        answer.resize( readed + data_size );
        mSocket.read( answer.data() + readed, answer.size() - readed );
        readed += data_size;

    }
    pkg.SetResponce( reinterpret_cast<uint8_t const*>( answer.data() ), answer.size() );
}


}//namespace fins

#include "moc_communicator.cpp"
