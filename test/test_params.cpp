#include "test_params.h"
#include <memory>
#include <mutex>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QLocale>
#include <QTextDocument>
#include "test.h"

namespace test
{
namespace
{
Parameters p;
}//namespace
Parameters& CURRENT_PARAMS = p;

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

std::vector<Test*> Tests()
{
#warning TODO
    return std::vector<Test*>();
}

Parameters::Parameters():
    mTestCase(),
    mDefaultExpenditure( 0.0 )
{}
void Parameters::Reset()
{
    mTestCase = nullptr;
    mUser.clear();
    mDate = QDateTime();
    mDefaultExpenditure = 0.0;
}

void Parameters::TestCase ( Test& test_case)
{
    mTestCase = &test_case;
}
Test* Parameters::TestCase () const
{
    return mTestCase;
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

void Parameters::ReportType( QString const& val )
{
    mReportType = val;
}
QString const& Parameters::ReportType()
{
    return mReportType;
}

bool Parameters::DefaultExpenditure( QString const& value )
{
    return ParseValue( mDefaultExpenditure, value );
}
const double &Parameters::DefaultExpenditure() const
{
    return mDefaultExpenditure;
}

QJsonObject Parameters::Serialise() const
{
    QJsonObject obj;
    QJsonArray tests;
    if (mTestCase)
        tests.push_back( mTestCase->ID() );

    obj.insert("TestCase", tests);
    obj.insert("Date", mDate.toString(Qt::ISODate));
    obj.insert("User", mUser);
    obj.insert("ReportType", mReportType);
    obj.insert("DefaultExpenditure", mDefaultExpenditure);
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
        for ( auto it = Tests().cbegin(), end = Tests().cend();
              it != end && !find ; ++it)
        {
            test::Test* ptr = *it;
            if ( ptr->ID() == val.toInt() )
            {
                mTestCase = ptr;
                find = true;
            }
        }
        res *= find;
    }

    mDate = QDateTime::fromString( obj.value("Date").toString(), Qt::ISODate );
    mUser = obj.value("User").toString();
    mReportType = obj.value("ReportType").toString();
    mDefaultExpenditure = obj.value("DefaultExpenditure").toDouble();

    return res;
}

void Parameters::StendInit() const
{
    WriteToController();
}
void Parameters::StendDeInit() const
{}

bool Parameters::Draw(QPainter &painter, QRect &free_rect, QString const& compare_width ) const
{
    QFont text_font = painter.font();
    text_font.setPointSize( 12 );
    painter.setFont( text_font );

    QRect r( 0, 0, free_rect.width(), free_rect.height() );
    painter.save();
    painter.translate( free_rect.topLeft() );
    QTextDocument doc;
    doc.setUndoRedoEnabled( false );
    doc.setTextWidth( free_rect.width() );
    doc.setUseDesignMetrics( true );
    doc.setDefaultTextOption ( QTextOption (Qt::AlignLeft )  );
    doc.setPlainText( ToString() );

    doc.drawContents( &painter, r );

    painter.restore();
    return true;
}

QString Parameters::ModelId() const
{
    return "";
}

void ParamsToFile( QString fname )
{
    QFile f( fname );
    f.open(QIODevice::WriteOnly);
    QJsonDocument doc;
    doc.setObject( CURRENT_PARAMS.Serialise() );

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

        CURRENT_PARAMS.Deserialize( obj );
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

        CURRENT_PARAMS.Deserialize( params );

        foreach (QJsonValue const& val, tests_data)
        {
            QJsonObject obj = val.toObject();
            uint8_t id = obj.value("id").toInt();
            QJsonObject data = obj.value("data").toObject();
            if ( CURRENT_PARAMS.TestCase()->ID() != id )
                return false;
            CURRENT_PARAMS.TestCase()->Deserialize( data );
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
    Test *d = params.TestCase();
    QJsonObject obj;
    obj.insert("id", d->ID() );
    obj.insert("data", d->Serialise() );
    tests_data.push_back( obj );

    data.insert( "Results", tests_data );
    return data;
}

void SaveToEtalone( const Parameters &params )
{
    auto obj = ReadFromEtalone();
    obj.insert( params.ModelId(), GetTestData(params) );
    QFile f( "models.json" );
    f.open(QIODevice::WriteOnly);
    QJsonDocument doc;
    doc.setObject( obj );
    f.write( doc.toJson() );

    f.close();
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

QJsonObject ReadFromEtalone()
{    
    return ReadFromFile( "models.json" );
}

}//namespace test

