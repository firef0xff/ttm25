#include "attestation_launch_control.h"
#include "communication/fins/funcs/memory_area_read.h"
#include "communication/fins/funcs/memory_area_write.h"
#include "net_connection.h"

namespace cpu
{
namespace data
{
AttestationLaunchControls::AttestationLaunchControls():
    mAddr(10,0)
{
    Reset();
}

void AttestationLaunchControls::Write()
{
    auto lock = Locker();
    fins::MemoryAreaWrite cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}
void AttestationLaunchControls::Read()
{
    auto lock = Locker();
    fins::MemoryAreaRead cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}

void AttestationLaunchControls::Reset()
{
    mData.clear();
    fins::AddElement< fins::BOOL >( mData, &W10_00, false );
    fins::AddElement< fins::BOOL >( mData, &W10_01, false );

    fins::AddElement< fins::BOOL >( mData, &W10_02, false );
    fins::AddElement< fins::BOOL >( mData, &W10_03, false );
    fins::AddElement< fins::BOOL >( mData, &W10_04, false );

    fins::AddElement< fins::BOOL >( mData, &W10_05, false );
    fins::AddElement< fins::BOOL >( mData, &W10_06, false );
}

std::unique_lock< std::recursive_mutex > AttestationLaunchControls::Locker()
{
    return std::unique_lock< std::recursive_mutex >( mMutex );
}

void AttestationLaunchControls::AttPressureStart( bool b )
{
    *W10_02 = b;
}
bool AttestationLaunchControls::AttPressureStart() const
{
    return *W10_02;
}

void AttestationLaunchControls::AttPressureStop( bool b )
{
    *W10_03 = b;
}
bool AttestationLaunchControls::AttPressureStop() const
{
    return *W10_03;
}

void AttestationLaunchControls::AttPressureSave( bool b )
{
    *W10_04 = b;
}
bool AttestationLaunchControls::AttPressureSave() const
{
    return *W10_04;
}

void AttestationLaunchControls::AttTimeStart( bool b )
{
    *W10_00 = b;
}
bool AttestationLaunchControls::AttTimeStart() const
{
    return *W10_00;
}

void AttestationLaunchControls::AttTimeStop( bool b )
{
    *W10_01 = b;
}
bool AttestationLaunchControls::AttTimeStop() const
{
    return *W10_01;
}

void AttestationLaunchControls::AttPressureSpeedStart( bool b )
{
    *W10_05 = b;
}
bool AttestationLaunchControls::AttPressureSpeedStart() const
{
    return *W10_05;
}

void AttestationLaunchControls::AttPressureSpeedStop( bool b )
{
    *W10_06 = b;
}
bool AttestationLaunchControls::AttPressureSpeedStop() const
{
    return *W10_06;
}

}//namespace data
}//namespace cpu
