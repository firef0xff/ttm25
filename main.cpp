#include "main_window.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include "users/login.h"
#include "cpu/net_connection.h"

int main(int argc, char *argv[])
{
    QStringList paths = QApplication::libraryPaths();
    paths.append( QFileInfo(argv[0]).dir().path() + QStringLiteral( "/plugins" ) );
    QApplication::setLibraryPaths( paths );

    QApplication a(argc, argv);

    fins::EndPoint source( fins::EndPoint::NA_LOCAL, 1, fins::EndPoint::A2_CPU );
    fins::EndPoint dest( fins::EndPoint::NA_LOCAL, 1, fins::EndPoint::A2_CPU );
    QString host = "192.168.0.2";
    qint16 port = 9600;
    cpu::NetConnection::Init( source, dest, host, port );

    MainWindow w;
    Login login( &w );
    login.show();

    return a.exec();
}
