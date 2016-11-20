#include "strings_source.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

namespace app
{
StringsSource::StringsSource( QString const& source ):
    mSource( source )
{
    Load();
}

QVector< QString >& StringsSource::Strings()
{
    return mData;
}
QVector< QString >const& StringsSource::Strings() const
{
    return mData;
}
void StringsSource::Load()
{
    QFile f( mSource );
    if ( f.exists() )
    {
        f.open(QIODevice::ReadOnly);
        QJsonParseError err;
        auto Document = QJsonDocument::fromJson( f.readAll() , &err );

        if ( err.error != QJsonParseError::NoError )
        {
            qWarning() << err.errorString()<<" позиция: " << QString::number(err.offset);
        }

        auto arr = Document.array();
        mData.clear();
        mData.reserve( arr.size() );
        for ( auto it = arr.begin(), end = arr.end(); it != end; ++it )
        {
            auto item = *it;
            mData.push_back( item.toString() );
        }
        f.close();
    }
}
void StringsSource::Save() const
{
    QJsonDocument Document;
    QJsonArray arr;
    for ( auto it = mData.begin(), end = mData.end(); it != end; ++it )
    {
        auto item = *it;
        arr.push_back( item );
    }
    Document.setArray( arr );
    QFile f( mSource );
    f.open(QIODevice::WriteOnly);
    f.write( Document.toJson() );
    f.close();
}

}//namespace app
