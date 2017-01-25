#include "attestation_params.h"
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
std::unique_ptr<AttestationParams> INST;
std::mutex MUTEX;
}
AttestationParams& AttestationParams::Instance()
{
    if ( !INST )
    {
        std::lock_guard< std::mutex > lock( MUTEX );
        if ( !INST )
        {
            INST.reset( new AttestationParams() );
        }
    }
    return *INST;
}

AttestationParams::AttestationParams():
    mPressureSpeed(0)
{}
AttestationParams::~AttestationParams()
{}

void AttestationParams::Reset()
{
    Parameters::Reset();
    mPressureSpeed = 0;
}
QString AttestationParams::ToString() const
{
    return QString();
}

QJsonObject AttestationParams::Serialise() const
{
    auto obj = Parameters::Serialise();
    obj.insert("PressureSpeed", mPressureSpeed);
    obj.insert("UpdatePeriod", mUpdatePeriod);
    return obj;
}
bool AttestationParams::Deserialize( QJsonObject const& obj )
{
    Parameters::Deserialize( obj );
    bool res = obj.contains("PressureSpeed") && obj.contains("UpdatePeriod");
    mPressureSpeed = obj.value("PressureSpeed").toDouble();
    mUpdatePeriod = obj.value("UpdatePeriod").toInt();
    return res;
}

void AttestationParams::WriteToController() const
{
    auto& params = cpu::CpuMemory::Instance().Params;
    params.Reset();
    params.PressureSpeed( mPressureSpeed );
    params.Write();
}

double AttestationParams::PressureSpeed() const
{
    return mPressureSpeed;
}
bool AttestationParams::PressureSpeed( QString const& val )
{
    return ParseValue( mPressureSpeed, val );
}
void AttestationParams::UpdatePeriod( int val )
{
    mUpdatePeriod = val;
}
int AttestationParams::UpdatePeriod() const
{
    return mUpdatePeriod;
}
}//namespace test

