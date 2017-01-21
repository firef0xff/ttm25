#include "work_params.h"
#include "../../cpu/cpu_memory.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>


namespace test
{
namespace
{
std::unique_ptr<WorkParams> INST;
std::mutex MUTEX;
}
WorkParams& WorkParams::Instance()
{
    if ( !INST )
    {
        std::lock_guard< std::mutex > lock( MUTEX );
        if ( !INST )
        {
            INST.reset( new WorkParams() );
        }
    }
    return *INST;
}

WorkParams::WorkParams():
    mModel(""),
    mSize(""),
    mCustomer(""),
    mOrderNo(0),
    mTireNo(""),
    mBreakPressure(false),
    mConstPressureTime(0),
    mFrequency(0),
    mPressure(0),
    mExpenditure(0),
    mPressureSpeed(0),
    mVolume(0)
{}
WorkParams::~WorkParams()
{}

void WorkParams::Reset()
{
    Parameters::Reset();
    mModel = "";
    mSize = "";
    mCustomer = "";

    mOrderNo = 0;
    mTireNo = "";
    mBreakPressure = false;
    mConstPressureTime = 0;


    mFrequency = 0;
    mPressure = 0;
    mExpenditure = 0;
    mPressureSpeed = 0;
    mVolume = 0;
}
QString WorkParams::ToString() const
{
    return QString();
}

QJsonObject WorkParams::Serialise() const
{
    auto obj = Parameters::Serialise();

    obj.insert("Model", mModel);
    obj.insert("Size", mSize);
    obj.insert("Customer", mCustomer);
    obj.insert("OrderNo", mOrderNo);
    obj.insert("TireNo", mTireNo);
    obj.insert("BreakPressure", mBreakPressure);
    obj.insert("ConstPressureTime", mConstPressureTime);

    obj.insert("Frequency", mFrequency);
    obj.insert("Pressure", mPressure);
    obj.insert("Expenditure", mExpenditure);
    obj.insert("PressureSpeed", mPressureSpeed);
    obj.insert("Volume", mVolume);

    return obj;
}
bool WorkParams::Deserialize( QJsonObject const& obj )
{
    Parameters::Deserialize( obj );
    bool res =
            obj.contains("Model") &&
            obj.contains("Size") &&
            obj.contains("Customer") &&
            obj.contains("OrderNo") &&
            obj.contains("TireNo") &&
            obj.contains("BreakPressure") &&
            obj.contains("ConstPressureTime")&&
            obj.contains("Frequency")&&
            obj.contains("Pressure")&&
            obj.contains("Expenditure")&&
            obj.contains("PressureSpeed")&&
            obj.contains("Volume");

    if ( !res )
        return res;

    mModel = obj.value("Model").toString();
    mSize = obj.value("Size").toString();
    mCustomer = obj.value("Customer").toString();

    mOrderNo = obj.value("OrderNo").toInt();
    mTireNo = obj.value("TireNo").toString();
    mBreakPressure = obj.value("BreakPressure").toBool();
    mConstPressureTime = obj.value("ConstPressureTime").toInt();

    mFrequency = obj.value("Frequency").toDouble();
    mPressure = obj.value("Pressure").toDouble();
    mExpenditure = obj.value("Expenditure").toDouble();
    mPressureSpeed = obj.value("PressureSpeed").toDouble();
    mVolume = obj.value("Volume").toDouble();
    return res;
}

void WorkParams::WriteToController() const
{
    auto& params = cpu::CpuMemory::Instance().Params;
    params.Frequency( mFrequency );
    params.Pressure( mPressure );
    params.Expenditure( mExpenditure );
    params.PressureSpeed( mPressureSpeed );
    params.Volume( mVolume );
    params.ConstPressureTime( mConstPressureTime );
    params.BreakPressure( mBreakPressure );

    params.Write();
}


QString const& WorkParams::Model() const
{
    return mModel;
}
bool WorkParams::Model( QString const& val )
{
    mModel = val;
    return true;
}

QString const& WorkParams::Size() const
{
    return mSize;
}
bool WorkParams::Size( QString const& val )
{
    mSize = val;
    return true;
}

QString const& WorkParams::Customer() const
{
    return mCustomer;
}
bool WorkParams::Customer( QString const& val )
{
    mCustomer = val;
    return true;
}

qint32 WorkParams::OrderNo() const
{
    return mOrderNo;
}
bool WorkParams::OrderNo( QString const& val )
{
    return ParseValue( mOrderNo, val );
}

QString WorkParams::TireNo() const
{
    return mTireNo;
}
bool WorkParams::TireNo( QString const& val )
{
    mTireNo = val;
    return true;
}

bool WorkParams::BreakPressure() const
{
    return mBreakPressure;
}
void WorkParams::BreakPressure( bool val )
{
    mBreakPressure = val;
}

qint32 WorkParams::ConstPressureTime() const
{
    return mConstPressureTime;
}
bool WorkParams::ConstPressureTime( QString const& val )
{
    return ParseValue( mConstPressureTime, val );
}

double WorkParams::Frequency() const
{
    return mFrequency;
}
bool WorkParams::Frequency( QString const& val )
{
    return ParseValue( mFrequency, val );
}

double WorkParams::Pressure() const
{
    return mPressure;
}
bool WorkParams::Pressure( QString const& val )
{
    return ParseValue( mPressure, val );
}

double WorkParams::Expenditure() const
{
    return mExpenditure;
}
bool WorkParams::Expenditure( QString const& val )
{
    return ParseValue( mExpenditure, val );
}

double WorkParams::PressureSpeed() const
{
    return mPressureSpeed;
}
bool WorkParams::PressureSpeed( QString const& val )
{
    return ParseValue( mPressureSpeed, val );
}

double WorkParams::Volume() const
{
    return mVolume;
}
bool WorkParams::Volume( QString const& val )
{
    return ParseValue( mVolume, val );
}

}//namespace test

