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
    mBreakPressure(0),
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
    mBreakPressure = 0;
    mConstPressureTime = 0;


    mFrequency = 0;
    mPressure = 0;
    mExpenditure = 0;
    mPressureSpeed = 0;
    mVolume = 0;
}
QString WorkParams::ToString() const
{}

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
    mTireNo = obj.value("TireNo").toInt();
    mBreakPressure = obj.value("BreakPressure").toDouble();
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

    params.Write();
}


QString const& WorkParams::Model() const
{
    return mModel;
}
void WorkParams::Model( QString const& val )
{
    mModel = val;
}

QString const& WorkParams::Size() const
{
    return mSize;
}
void WorkParams::Size( QString const& val )
{
    mSize = val;
}

QString const& WorkParams::Customer() const
{
    return mCustomer;
}
void WorkParams::Customer( QString const& val )
{
    mCustomer = val;
}

qint32 WorkParams::OrderNo() const
{
    return mOrderNo;
}
void WorkParams::OrderNo( QString const& val )
{
    ParseValue( mOrderNo, val );
}

QString WorkParams::TireNo() const
{
    return mTireNo;
}
void WorkParams::TireNo( QString const& val )
{
    mTireNo = val;
}

double WorkParams::BreakPressure() const
{
    return mBreakPressure;
}
void WorkParams::BreakPressure( QString const& val )
{
    ParseValue( mBreakPressure, val );
}

qint32 WorkParams::ConstPressureTime() const
{
    return mConstPressureTime;
}
void WorkParams::ConstPressureTime( QString const& val )
{
    ParseValue( mConstPressureTime, val );
}

double WorkParams::Frequency() const
{
    return mFrequency;
}
void WorkParams::Frequency( QString const& val )
{
    ParseValue( mFrequency, val );
}

double WorkParams::Pressure() const
{
    return mPressure;
}
void WorkParams::Pressure( QString const& val )
{
    ParseValue( mPressure, val );
}

double WorkParams::Expenditure() const
{
    return mExpenditure;
}
void WorkParams::Expenditure( QString const& val )
{
    ParseValue( mExpenditure, val );
}

double WorkParams::PressureSpeed() const
{
    return mPressureSpeed;
}
void WorkParams::PressureSpeed( QString const& val )
{
    ParseValue( mPressureSpeed, val );
}

double WorkParams::Volume() const
{
    return mVolume;
}
void WorkParams::Volume( QString const& val )
{
    ParseValue( mVolume, val );
}

}//namespace test

