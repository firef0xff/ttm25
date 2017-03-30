#include "main_window.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>


int main(int argc, char *argv[])
{
    QStringList paths = QApplication::libraryPaths();
    paths.append( QFileInfo(argv[0]).dir().path() + QStringLiteral( "/plugins" ) );
    QApplication::setLibraryPaths( paths );

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
