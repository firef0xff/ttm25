#pragma once
#include <QString>
#include <QPainter>
#include <QDateTime>

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
    virtual ~Parameters(){}
    virtual QString ToString() const
    {
#warning TODO
        return QString();
    }

    virtual QJsonObject Serialise() const;
    virtual bool Deserialize( QJsonObject const& obj );

    virtual void StendInit() const;
    virtual void StendDeInit() const;
    virtual void WriteToController() const
    {
#warning TODO
    }

    virtual bool Draw(QPainter &painter, QRect &free_rect , const QString &compare_width) const;
    virtual bool DrawResults(QPainter &/*painter*/, QRect &/*free_rect*/ ) const
    { return true; }
    virtual bool HasResults()
    {
        return false;
    }

    virtual QString ModelId() const;
    void Reset();

    void TestCase (Test &test_case);
    Test* TestCase () const;

    void Date( QDateTime const& date );
    QDateTime const& Date();

    void User( QString const& user );
    QString const& User();

    void ReportType(QString const& val );
    QString const& ReportType();

    bool DefaultExpenditure( QString const& value );
    double const& DefaultExpenditure() const;

protected:

    Test* mTestCase;        //тест для выполнения
    QDateTime mDate;            //дата проведения испытаний
    QString   mUser;            //испытатель.
    QString   mReportType;      //тип отчета.

    double mDefaultExpenditure; //расход по умолчанию
};

std::vector<Test*> Tests();

extern Parameters& CURRENT_PARAMS;

void ParamsToFile(QString fname);
Parameters* ParamsFromFile( QString fname );

void DataToFile( QString fname, Parameters const& params );
bool DataFromFile( QString fname );

QJsonObject GetTestData(const Parameters &params);

void SaveToEtalone( const Parameters &params );
QJsonObject ReadFromFile( QString const& f_name );
QJsonObject ReadFromEtalone();

}//namespace test
