#include "params.h"
#include "communication/fins/funcs/memory_area_write.h"
#include "net_connection.h"

namespace cpu
{
namespace data
{

Params::Params():
    mAddr(0,0),
    mbAddr(3,15)
{
    Reset();
}

void Params::Reset()
{
    mData.clear();
    mbData.clear();
    fins::AddElement< fins::FLOAT >( mData, &D00, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D02, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D04, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D06, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D08, 0.0f );
    fins::AddElement< fins::FLOAT >( mData, &D10, 0.0f );
    fins::AddElement< fins::BOOL >( mbData, &W3_15, false );
}

void Params::Write()
{
    fins::MemoryAreaWrite cmd1( mAddr, mData );
    NetConnection::Execute( cmd1 );
    fins::MemoryAreaWrite cmd2( mbAddr, mbData );
    NetConnection::Execute( cmd2 );
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
void Params::ConstPressureTime( float v )
{
    *D10 = v;
}
void Params::BreakPressure( bool v )
{
    *W3_15 = v;
}
}//namespace data
}//namespace cpu
