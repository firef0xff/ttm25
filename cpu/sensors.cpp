#include "sensors.h"
#include "net_connection.h"
#include "fins/funcs/memory_area_read.h"

namespace cpu
{
namespace data
{

Sensors::Sensors():
    mAddr(50,0)
{
    Reset();
}

void Sensors::Reset()
{
    mData.clear();
    fins::AddElement< fins::REAL >( mData, &D50, 0.0f );
    fins::AddElement< fins::REAL >( mData, &D52, 0.0f );

    fins::AddElement< fins::REAL >( mData, &D54, 0.0f );
    fins::AddElement< fins::REAL >( mData, &D56, 0.0f );
}
void Sensors::Read()
{
    fins::MemoryAreaRead cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}
float Sensors::Pressure() const
{
    return *D50;
}
float Sensors::Expenditure() const
{
    return *D52;
}
float Sensors::Volume() const
{
    return *D54;
}
float Sensors::Time() const
{
    return *D56;
}
} //namespace deta

} //namespace cpu
