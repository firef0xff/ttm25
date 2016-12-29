#include "controls.h"
#include "communication/fins/funcs/memory_area_read.h"
#include "communication/fins/funcs/memory_area_write.h"
#include "net_connection.h"

namespace cpu
{
namespace data
{

Controls::Controls():
    mAddr(0,0)
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
    fins::AddElement< fins::BOOL >( mData, &H0_00, false );
    fins::AddElement< fins::BOOL >( mData, &H0_01, false );

    fins::AddElement< fins::BOOL >( mData, &H0_02, false );
    fins::AddElement< fins::BOOL >( mData, &H0_03, false );

    fins::AddElement< fins::BOOL >( mData, &H0_04, false );
    fins::AddElement< fins::BOOL >( mData, &H0_05, false );

    fins::AddElement< fins::BOOL >( mData, &H0_06, false );
    fins::AddElement< fins::BOOL >( mData, &H0_07, false );

    fins::AddElement< fins::BOOL >( mData, &H0_08, false );
    fins::AddElement< fins::BOOL >( mData, &H0_09, false );

    fins::AddElement< fins::BOOL >( mData, &H0_10, false );
    fins::AddElement< fins::BOOL >( mData, &H0_11, false );
}

void Controls::PumpOnOff( bool v )
{
    *H0_00 = v;
}
bool Controls::PumpOnOff() const
{
    return *H0_00;
}

void Controls::RegulatingOpen( bool v )
{
    *H0_03 = v;
}
bool Controls::RegulatingOpen() const
{
    return *H0_03;
}

void Controls::RegulatingClose( bool v )
{
    *H0_02 = v;
}
bool Controls::RegulatingClose() const
{
    return *H0_02;
}

void Controls::ResetOnOff( bool v )
{
    *H0_04 = v;
    *H0_05 = !v;
}
bool Controls::ResetOnOff() const
{
    return *H0_04 &&  !*H0_05;
}

void Controls::FeedOnOff( bool v )
{
    *H0_06 = v;
    *H0_07 = !v;
}
bool Controls::FeedOnOff() const
{
    return *H0_06 &&  !*H0_07;
}

void Controls::AirOnOff( bool v )
{
    *H0_08 = v;
    *H0_09 = !v;

}
bool Controls::AirOnOff() const
{
    return *H0_08 && !*H0_09;
}

void Controls::VacuumOnOff( bool v )
{
    *H0_10 = v;
    *H0_11 = !v;

}
bool Controls::VacuumOnOff() const
{
    return *H0_10 && !*H0_11;
}

std::unique_lock< std::recursive_mutex > Controls::Locker()
{
    return std::unique_lock< std::recursive_mutex >( mMutex );
}
} //namespace deta

} //namespace cpu
