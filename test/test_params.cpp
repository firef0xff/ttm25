#include "test_params.h"
#include <memory>
#include <mutex>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QLocale>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include "test.h"

namespace test
{
namespace
{
Parameters p;
}//namespace
Parameters* CURRENT_PARAMS = &p;

bool ParseValue ( qint32& param, QString const& val )
{
    bool t = false;
    auto b =  val.toInt( &t );

    if ( t )
        param = b;
    return t;
}
bool ParseValue ( double& param, QString const& val )
{
    QChar const decimalsep = QLocale::system().decimalPoint();
    bool t = false;
    auto b =  QString(val).replace( decimalsep, "." ).toDouble( &t );

    if ( t )
        param = b;
    return t;
}

QString ToString( qint32 const& v )
{   
    return QString::number( v );
}
QString ToString( bool const& v )
{
    if ( v )
        return QString( "Да" );
    return QString( "Нет" );
}
QString ToString( double const& v )
{
    QChar const decimalsep = QLocale::system().decimalPoint();
    return QString::number( v ).replace( ".", decimalsep );
}


Parameters::Parameters():
    mTestForExec(),
    mTests()
{}
Parameters::~Parameters()
{}
void Parameters::Reset()
{
    mTestForExec = nullptr;
    mUser.clear();
    mDate = QDateTime();
}

void Parameters::TestForExec ( Test& test_case)
{
    mTestForExec = &test_case;
}
Test* Parameters::TestForExec () const
{
    return mTestForExec;
}

void Parameters::Date( QDateTime const& data )
{
    mDate = data;
}
QDateTime const& Parameters::Date()
{
    return mDate;
}

void Parameters::User( QString const& user )
{
    mUser = user;
}
QString const& Parameters::User()
{
    return mUser;
}

QJsonObject Parameters::Serialise() const
{
    QJsonObject obj;
    QJsonArray tests;
    if (mTestForExec)
        tests.push_back( mTestForExec->ID() );

    obj.insert("TestForExec", tests);
    obj.insert("Date", mDate.toString(Qt::ISODate));
    obj.insert("User", mUser);
    return obj;
}
bool Parameters::Deserialize(const QJsonObject &obj )
{
    Reset();
    bool res = true;
    QJsonArray tests = obj.value("TestCase").toArray();

    foreach ( QJsonValue const& val, tests )
    {
        bool find = false;
        for ( auto it = TestsCase().cbegin(), end = TestsCase().cend();
              it != end && !find ; ++it)
        {
            test::Test& ptr = *it->second;
            if ( ptr.ID() == val.toInt() )
            {
                mTestForExec = &ptr;
                find = true;
            }
        }
        res *= find;
    }

    mDate = QDateTime::fromString( obj.value("Date").toString(), Qt::ISODate );
    mUser = obj.value("User").toString();

    return res;
}

void Parameters::StendInit() const
{
    WriteToController();
}
void Parameters::StendDeInit() const
{}
void Parameters::WriteToController() const
{}

bool Parameters::Draw(QPainter &/*painter*/, QRect &/*free_rect*/, QString const& /*compare_width*/ ) const
{
    return true;
}
bool Parameters::DrawResults(QPainter &/*painter*/, QRect &/*free_rect*/ ) const
{
    return true;
}

QString Parameters::ToString() const
{
    QString res;
    res += "Дата проведения испытания: ";
    res += mDate.toString("dd MMMM yyyy г. hh:mm");
    res += "\n";
    res += "Испытатель:";
    res += mUser;
    res += "\n";
    return res;
}

void Parameters::AddTest( Parameters::TestPtr t)
{
    auto id = t->ID();
    mTests.insert( std::pair<int32_t, Parameters::TestPtr >( id, std::move(t)));
}
Parameters::TestCase const& Parameters::TestsCase()
{
    return mTests;
}

void ParamsToFile( QString fname )
{
    QFile f( fname );
    f.open(QIODevice::WriteOnly);
    QJsonDocument doc;
    doc.setObject( CURRENT_PARAMS->Serialise() );

    f.write( doc.toJson() );

    f.close();
}
Parameters* ParamsFromFile( QString fname )
{
    QFile f( fname );
    Parameters* ret = nullptr;
    if ( f.exists() )
    {
        f.open(QIODevice::ReadOnly);
        auto doc = QJsonDocument::fromJson( f.readAll() );
        QJsonObject obj = doc.object();

        CURRENT_PARAMS->Deserialize( obj );
        f.close();
    }

    return ret;
}

void DataToFile( QString fname, Parameters const& params )
{
    QFile f( fname );
    f.open(QIODevice::WriteOnly);
    QJsonDocument doc;
    doc.setObject( GetTestData( params ) );
    f.write( doc.toJson() );

    f.close();
}
bool DataFromFile( QString fname )
{
    QFile f( fname );
    if ( f.exists() )
    {
        f.open(QIODevice::ReadOnly);
        auto doc = QJsonDocument::fromJson( f.readAll() );
        QJsonObject params = doc.object().value("Params").toObject();
        QJsonArray tests_data = doc.object().value("Results").toArray();

        CURRENT_PARAMS->Deserialize( params );

        foreach (QJsonValue const& val, tests_data)
        {
            QJsonObject obj = val.toObject();
            uint8_t id = obj.value("id").toInt();
            QJsonObject data = obj.value("data").toObject();
            if ( CURRENT_PARAMS->TestForExec()->ID() != id )
                return false;
            CURRENT_PARAMS->TestForExec()->Deserialize( data );
        }
        f.close();
    }

    return true;
}

QJsonObject GetTestData( Parameters const& params )
{
    QJsonObject data;
    data.insert( "Params", params.Serialise() );

    QJsonArray tests_data;
    Test *d = params.TestForExec();
    QJsonObject obj;
    obj.insert("id", d->ID() );
    obj.insert("data", d->Serialise() );
    tests_data.push_back( obj );

    data.insert( "Results", tests_data );
    return data;
}

QJsonObject ReadFromFile( QString const& f_name )
{
    QFile f( f_name );
    if ( f.exists() )
    {
        f.open(QIODevice::ReadOnly);
        auto doc = QJsonDocument::fromJson( f.readAll() );
        f.close();

        return doc.object();
    }

    return QJsonObject();
}

}//namespace test

