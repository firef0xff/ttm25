#include "udp_communicator.h"

namespace fins
{

UDP_Communicator::UDP_Communicator( QString host, qint16 port ):
    mHost(host),
    mPort(port),
    mSocket( this )
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

}

void UDP_Communicator::send(uint8_t const* buf, size_t size)
{
    mSocket.writeDatagram( reinterpret_cast<char const *>( buf ), size, mHost, mPort );
}

/*void UDP_Communicator::read()
{
  QByteArray datagram;
  datagram.resize(socket->pendingDatagramSize());
  QHostAddress *address = new QHostAddress();
  socket->readDatagram(datagram.data(), datagram.size(), address);

  QDataStream in(&datagram, QIODevice::ReadOnly);

  qint64 size = -1;
  if(in.device()->size() > sizeof(qint64)) {
    in >> size;
  } else return;
  if (in.device()->size() - sizeof(qint64) < size) return;

  qint8 type = 0;
  in >> type;

  if (type == USUAL_MESSAGE) {
    QString str;
    in >> str;
    // код по перенаправке сообщения в классы выше //
  } else if (type == PERSON_ONLINE) {
    // Добавление пользователя с считанным QHostAddress //
  } else if (type == WHO_IS_ONLINE) {
    sending(nickname, qint8(PERSON_ONLINE));
  }
}*/

}//namespace fins

#include "moc_udp_communicator.cpp"
