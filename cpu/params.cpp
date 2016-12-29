#include "params.h"
#include "communication/fins/funcs/memory_area_write.h"
#include "net_connection.h"

namespace cpu
{
namespace data
{

Params::Params():
    mAddr(0,0)
{
    Reset();
}

void Params::Reset()
{
    mData.clear();
    fins::AddElement< fins::FLOAT >( mData, &D00, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D02, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D04, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D06, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D08, 0.0f );
}

void Params::Write()
{
    fins::MemoryAreaWrite cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}

void Params::Frequency( float v )
{
    *D00 = v;
}
void Params::Pressure( float v )
{
    *D02 = v;
}
void Params::Expenditure( float v )
{
    *D04 = v;
}
void Params::PressureSpeed( float v )
{
    *D06 = v;
}
void Params::Volume( float v )
{
    *D08 = v;
}

}//namespace data
}//namespace cpu
