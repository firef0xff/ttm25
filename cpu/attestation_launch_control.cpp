#include "attestation_launch_control.h"
#include "fins/funcs/memory_area_read.h"
#include "fins/funcs/memory_area_write.h"
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
void AttestationLaunchControls::WriteElement( int i )
{
    auto lock = Locker();
    fins::BIT_WR addr(mAddr.mAddr, mAddr.mBit + i);
    fins::Elements data;
    auto item = fins::BOOL::Create();
    auto* real_type = static_cast<fins::BOOL*>( item.get() );

    int pos = 0;
    for ( auto it = mData.begin(), end = mData.end(); it != end; ++it, ++pos )
    {
        fins::Element::Ptr& ptr = *it;
        fins::BOOL* ptr_elem = static_cast<fins::BOOL*>(ptr.get());
        if ( pos == i )
        {
            *real_type = *ptr_elem;
            break;
        }
    }

    data.push_back( std::move( item ) );
    fins::MemoryAreaWrite cmd( addr, data );
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

    fins::AddElement< fins::BOOL >( mData, &W10_07, false );
    fins::AddElement< fins::BOOL >( mData, &W10_08, false );
    fins::AddElement< fins::BOOL >( mData, &W10_09, false );
}

std::unique_lock< std::recursive_mutex > AttestationLaunchControls::Locker()
{
    return std::unique_lock< std::recursive_mutex >( mMutex );
}

void AttestationLaunchControls::AttPressureStart( bool b )
{
    *W10_02 = b;
    WriteElement(2);
}
bool AttestationLaunchControls::AttPressureStart() const
{
    return *W10_02;
}

void AttestationLaunchControls::AttPressureStop( bool b )
{
    *W10_03 = b;
    WriteElement(3);
}
bool AttestationLaunchControls::AttPressureStop() const
{
    return *W10_03;
}

void AttestationLaunchControls::AttPressureSave( bool b )
{
    *W10_04 = b;
    WriteElement(4);
}
bool AttestationLaunchControls::AttPressureSave() const
{
    return *W10_04;
}

void AttestationLaunchControls::AttTimeStart( bool b )
{
    *W10_00 = b;
    WriteElement(0);
}
bool AttestationLaunchControls::AttTimeStart() const
{
    return *W10_00;
}

void AttestationLaunchControls::AttTimeStop( bool b )
{
    *W10_01 = b;
    WriteElement(1);
}
bool AttestationLaunchControls::AttTimeStop() const
{
    return *W10_01;
}

void AttestationLaunchControls::AttPressureSpeedStart( bool b )
{
    *W10_05 = b;
    WriteElement(5);
}
bool AttestationLaunchControls::AttPressureSpeedStart() const
{
    return *W10_05;
}

void AttestationLaunchControls::AttPressureSpeedStop( bool b )
{
    *W10_06 = b;
    WriteElement(6);
}
bool AttestationLaunchControls::AttPressureSpeedStop() const
{
    return *W10_06;
}

void AttestationLaunchControls::AttPressureTimeStart( bool b )
{
    *W10_07 = b;
    WriteElement(7);
}
bool AttestationLaunchControls::AttPressureTimeStart() const
{
    return *W10_07;
}

void AttestationLaunchControls::AttPressureTimeStop( bool b )
{
    *W10_08 = b;
    WriteElement(8);
}
bool AttestationLaunchControls::AttPressureTimeStop() const
{
    return *W10_08;
}

void AttestationLaunchControls::AttPressureTimeSave( bool b )
{
    *W10_09 = b;
    WriteElement(9);
}
bool AttestationLaunchControls::AttPressureTimeSave() const
{
    return *W10_09;
}
}//namespace data
}//namespace cpu
