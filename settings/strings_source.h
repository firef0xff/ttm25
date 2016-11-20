#pragma once
#include <QString>
#include <QVector>

namespace app
{

class StringsSource
{
public:
    StringsSource( QString const& source );
    QVector< QString >& Strings();
    QVector< QString >const& Strings() const;
    void Load();
    void Save() const;
private:
    QVector< QString > mData;
    QString mSource;
};

}//namespace app
