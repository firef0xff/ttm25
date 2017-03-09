#include "err_codes.h"
#include "communication/fins/funcs/memory_area_read.h"
#include "communication/fins/funcs/memory_area_write.h"

#include "net_connection.h"

namespace cpu
{
namespace data
{

ErrCodes::ErrCodes():
    mAddr(12,0)
{
    Reset();
}

void ErrCodes::Read()
{
    fins::MemoryAreaRead cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}

void ErrCodes::Write()
{
    fins::MemoryAreaWrite cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}

void ErrCodes::Reset()
{
    mData.clear();
    fins::AddElement< fins::BOOL >( mData, &W12_00, false );
    fins::AddElement< fins::BOOL >( mData, &W12_01, false );

    fins::AddElement< fins::BOOL >( mData, &W12_02, false );
    fins::AddElement< fins::BOOL >( mData, &W12_03, false );

    fins::AddElement< fins::BOOL >( mData, &W12_04, false );
    fins::AddElement< fins::BOOL >( mData, &W12_05, false );
}

bool ErrCodes::IsFaucetClose()
{
    return *W12_00;
}
bool ErrCodes::IsLowWaterLavel()
{
    return *W12_01;
}
bool ErrCodes::IsHightPressureSpeed()
{
    return *W12_02;
}
bool ErrCodes::IsLowPressure()
{
    return *W12_03;
}
bool ErrCodes::IsHightPressure()
{
    return *W12_04;
}
bool ErrCodes::IsLowPumpFrequency()
{
    return *W12_05;
}
} //namespace deta

} //namespace cpu
