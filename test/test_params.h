#pragma once
#include <QString>
#include <QPainter>
#include <QDateTime>
#include <memory>
namespace test
{

bool ParseValue ( qint32& param, QString const& val );
bool ParseValue ( double &param, QString const& val );


QString ToString( qint32 const& v );
QString ToString( bool const& v );
QString ToString( double const& v );

class Test;
class Parameters
{
public:
    Parameters();
    virtual ~Parameters();
    virtual QString ToString() const;

    virtual QJsonObject Serialise() const;
    virtual bool Deserialize( QJsonObject const& obj );

    virtual void StendInit() const;
    virtual void StendDeInit() const;
    virtual void WriteToController() const;

    virtual bool Draw(QPainter &painter, QRect &free_rect , const QString &compare_width) const;
    virtual bool DrawResults(QPainter &/*painter*/, QRect &/*free_rect*/ ) const;

    virtual bool HasResults()
    {
        return false;
    }

    virtual void Reset();

    void TestForExec (Test &test_case);
    Test* TestForExec () const;

    void Date( QDateTime const& date );
    QDateTime const& Date();

    void User( QString const& user );
    QString const& User();

    typedef std::unique_ptr<test::Test> TestPtr;
    typedef std::map< int32_t, std::unique_ptr<test::Test> > TestCase;

    void AddTest( TestPtr t);
    TestCase const& TestsCase();
protected:

    Test* mTestForExec;        //тест для выполнения
    QDateTime mDate;            //дата проведения испытаний
    QString   mUser;            //испытатель.
    TestCase mTests;
};

extern Parameters* CURRENT_PARAMS;

void ParamsToFile(QString fname);
Parameters* ParamsFromFile( QString fname );

void DataToFile( QString fname, Parameters const& params );
bool DataFromFile( QString fname );

QJsonObject GetTestData(const Parameters &params);


QJsonObject ReadFromFile( QString const& f_name );

}//namespace test
