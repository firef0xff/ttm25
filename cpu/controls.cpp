#include "controls.h"
#include "communication/fins/funcs/memory_area_read.h"
#include "communication/fins/funcs/memory_area_write.h"
#include "net_connection.h"

namespace cpu
{
namespace data
{

Controls::Controls():
    mAddr(2,0)
{
    Reset();
}

void Controls::Write()
{
    auto lock = Locker();
    fins::MemoryAreaWrite cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}
void Controls::Read()
{
    auto lock = Locker();
    fins::MemoryAreaRead cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}

void Controls::Reset()
{
    mData.clear();
    fins::AddElement< fins::BOOL >( mData, &W2_00, false );
    fins::AddElement< fins::BOOL >( mData, &W2_01, false );

    fins::AddElement< fins::BOOL >( mData, &W2_02, false );
    fins::AddElement< fins::BOOL >( mData, &W2_03, false );

    fins::AddElement< fins::BOOL >( mData, &W2_04, false );
    fins::AddElement< fins::BOOL >( mData, &W2_05, false );

    fins::AddElement< fins::BOOL >( mData, &W2_06, false );
    fins::AddElement< fins::BOOL >( mData, &W2_07, false );

    fins::AddElement< fins::BOOL >( mData, &W2_08, false );
    fins::AddElement< fins::BOOL >( mData, &W2_09, false );

    fins::AddElement< fins::BOOL >( mData, &W2_10, false );
    fins::AddElement< fins::BOOL >( mData, &W2_11, false );
}

void Controls::PumpOnOff( bool v )
{
    *W2_00 = v;
}
bool Controls::PumpOnOff() const
{
    return *W2_00;
}

void Controls::RegulatingOnOff( bool v )
{
    *W2_02 = v;
    *W2_03 = !v;
}
bool Controls::RegulatingOnOff() const
{
    return *W2_02 &&  !*W2_03;
}

void Controls::ResetOnOff( bool v )
{
    *W2_04 = v;
    *W2_05 = !v;
}
bool Controls::ResetOnOff() const
{
    return *W2_04 &&  !*W2_05;
}

void Controls::FeedOnOff( bool v )
{
    *W2_06 = v;
    *W2_07 = !v;
}
bool Controls::FeedOnOff() const
{
    return *W2_06 &&  !*W2_07;
}

void Controls::AirOnOff( bool v )
{
    *W2_08 = v;
    *W2_09 = !v;

}
bool Controls::AirOnOff() const
{
    return *W2_08 && !*W2_09;
}

void Controls::VacuumOnOff( bool v )
{
    *W2_10 = v;
    *W2_11 = !v;

}
bool Controls::VacuumOnOff() const
{
    return *W2_10 && !*W2_11;
}

std::unique_lock< std::recursive_mutex > Controls::Locker()
{
    return std::unique_lock< std::recursive_mutex >( mMutex );
}
} //namespace deta

} //namespace cpu
