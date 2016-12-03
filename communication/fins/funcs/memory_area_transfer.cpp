#include "memory_area_transfer.h"

namespace fins
{

MemoryAreaTransfer::MemoryAreaTransfer( MemoryAddr const& from, MemoryAddr const& to, int16_t count ):
    fins::Command(0x1,0x5),
    mAddrFrom( from ),
    mAddrTo( to ),
    mCount( count )

{
}
uint8_t MemoryAreaTransfer::GetICF()
{
    return Command::REQUEST_WITH_RESPONCE;
}
size_t MemoryAreaTransfer::WriteImpl( uint8_t* buf, size_t size )
{
    if ( size < RequestSizeImpl() )
        return size;

    uint8_t* head = buf;
    *(head++) = mAddrFrom.mMemType;
    auto addr = reinterpret_cast<uint8_t const*>(&mAddrFrom.mAddr);
    *(head++) = addr[1];
    *(head++) = addr[0];
    *(head++) = mAddrFrom.mBit;
    *(head++) = mAddrTo.mMemType;
    addr = reinterpret_cast<uint8_t const*>(&mAddrTo.mAddr);
    *(head++) = addr[1];
    *(head++) = addr[0];
    *(head++) = mAddrTo.mBit;
    auto count = reinterpret_cast<uint8_t const*>(&mCount);
    *(head++) = count[1];
    *(head++) = count[0];

    return head - buf;
}
size_t MemoryAreaTransfer::ReadImpl( uint8_t const* /*buf*/, size_t size, bool& res )
{
    res = true;
    return size;
}
size_t MemoryAreaTransfer::RequestSizeImpl() const
{
    static const size_t base_size = 2*(sizeof( mAddrFrom.mMemType ) +
                                    sizeof( mAddrFrom.mAddr ) +
                                    sizeof( mAddrFrom.mBit )) +
                                    sizeof( mCount );

    return base_size;
}
size_t MemoryAreaTransfer::ResponseSizeImpl() const
{
    return 0;
}

}//namespace fins

