#include "launch_control.h"
#include "fins/funcs/memory_area_read.h"
#include "fins/funcs/memory_area_write.h"
#include "net_connection.h"

namespace cpu
{
namespace data
{

LaunchControl::LaunchControl():
    mAddr(3,0)
{
    Reset();
}

void LaunchControl::Write()
{
    auto lock = Locker();
    fins::MemoryAreaWrite cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}
void LaunchControl::Read()
{
    auto lock = Locker();
    fins::MemoryAreaRead cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}

void LaunchControl::Reset()
{
    mData.clear();
    fins::AddElement< fins::BOOL >( mData, &W3_00, false );
    fins::AddElement< fins::BOOL >( mData, &W3_01, false );

    fins::AddElement< fins::BOOL >( mData, &W3_02, false );
    fins::AddElement< fins::BOOL >( mData, &W3_03, false );

    fins::AddElement< fins::BOOL >( mData, &W3_04, false );
}

std::unique_lock< std::recursive_mutex > LaunchControl::Locker()
{
    return std::unique_lock< std::recursive_mutex >( mMutex );
}

void LaunchControl::Prepare( bool v )
{
    *W3_00 = v;
}
bool LaunchControl::Prepare() const
{
    return *W3_00;
}

void LaunchControl::Start( bool v )
{
    *W3_01 = v;
}
bool LaunchControl::Start() const
{
    return *W3_01;
}

void LaunchControl::Stop( bool v )
{
    *W3_02 = v;
}
bool LaunchControl::Stop() const
{
    return *W3_02;
}

void LaunchControl::Terminated( bool v )
{
    *W3_03 = v;
}
bool LaunchControl::Terminated() const
{
    return *W3_03;
}

void LaunchControl::Done( bool v )
{
    *W3_04 = v;
}
bool LaunchControl::Done() const
{
    return *W3_04;
}

}//namespace data
}//namespace cpu
