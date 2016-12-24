#include "attestation_params.h"
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

AttestationParams::AttestationParams()
{}
AttestationParams::~AttestationParams()
{}

void AttestationParams::Reset()
{
    Parameters::Reset();
}
QString AttestationParams::ToString() const
{
    return QString();
}

QJsonObject AttestationParams::Serialise() const
{
    auto obj = Parameters::Serialise();
    return obj;
}
bool AttestationParams::Deserialize( QJsonObject const& /*obj*/ )
{
    return true;
}

void AttestationParams::WriteToController() const
{}


}//namespace test

