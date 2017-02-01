#include "indication.h"
#include "communication/fins/funcs/memory_area_read.h"
#include "net_connection.h"

namespace cpu
{
namespace data
{

Indication::Indication():
    mAddr(5,0)
{
    Reset();
}

void Indication::Read()
{
    fins::MemoryAreaRead cmd( mAddr, mData );
    NetConnection::Execute( cmd );
}

void Indication::Reset()
{
    mData.clear();
    fins::AddElement< fins::BOOL >( mData, &W5_00, false );
    fins::AddElement< fins::BOOL >( mData, &W5_01, false );

    fins::AddElement< fins::BOOL >( mData, &W5_02, false );
    fins::AddElement< fins::BOOL >( mData, &W5_03, false );

    fins::AddElement< fins::BOOL >( mData, &W5_04, false );
    fins::AddElement< fins::BOOL >( mData, &W5_05, false );

    fins::AddElement< fins::BOOL >( mData, &W5_06, false );
    fins::AddElement< fins::BOOL >( mData, &W5_07, false );

    fins::AddElement< fins::BOOL >( mData, &W5_08, false );
    fins::AddElement< fins::BOOL >( mData, &W5_09, false );
}

bool Indication::PBU() const
{
    return *W5_00;
}
bool Indication::MUP() const
{
    return *W5_01;
}

bool Indication::NVK() const
{
    return *W5_02;
}
bool Indication::UVN() const
{
    return *W5_03;
}

bool Indication::UVM() const
{
    return *W5_04;
}
bool Indication::ZVN() const
{
    return *W5_05;
}

bool Indication::KPO() const
{
    return *W5_06;
}
bool Indication::KPZ() const
{
    return *W5_07;
}

bool Indication::XO() const
{
    return *W5_08;
}
bool Indication::XZ() const
{
    return *W5_09;
}

} //namespace deta

} //namespace cpu
