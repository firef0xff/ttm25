#include "work_params.h"
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
    mConstPressureTime(0)
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
            obj.contains("ConstPressureTime");

    if ( !res )
        return res;

    mModel = obj.value("Model").toString();
    mSize = obj.value("Size").toString();
    mCustomer = obj.value("Customer").toString();

    mOrderNo = obj.value("OrderNo").toInt();
    mTireNo = obj.value("TireNo").toInt();
    mBreakPressure = obj.value("BreakPressure").toDouble();
    mConstPressureTime = obj.value("ConstPressureTime").toInt();
    return res;
}

void WorkParams::WriteToController() const
{}


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


}//namespace test

