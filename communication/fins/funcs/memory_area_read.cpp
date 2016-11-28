#include "memory_area_read.h"


namespace fins
{

MemoryAreaRead::MemoryAreaRead( MemoryAddr const& addr, Elements& data ):
    fins::Command(0x1,0x1),
    mAddr( addr ),
    mElements( data )
{

}

uint8_t MemoryAreaRead::GetICF()
{
    return Command::REQUEST_WITH_RESPONCE;
}
size_t MemoryAreaRead::WriteImpl( uint8_t* buf, size_t size )
{
    if ( size < 48 )
        return size;

    uint8_t* head = buf;
    *(head++) = mAddr.mMemType;
    auto addr = reinterpret_cast<uint8_t const*>(&mAddr.mAddr);
    *(head++) = addr[1];
    *(head++) = addr[0];
    *(head++) = mAddr.mBit;
    uint16_t el_count = mElements.size();
    auto count = reinterpret_cast<uint8_t const*>(&el_count);
    *(head++) = count[1];
    *(head++) = count[0];
    return head - buf;
}
size_t MemoryAreaRead::ReadImpl( uint8_t const* buf, size_t size, bool& res )
{

}

}//namespace fins

