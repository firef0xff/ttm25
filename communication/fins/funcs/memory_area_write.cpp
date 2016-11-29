#include "memory_area_write.h"

namespace fins
{

MemoryAreaWrite::MemoryAreaWrite( const MemoryAddr &addr, Elements const& data ):
    fins::Command(0x1,0x2),
    mAddr( addr ),
    mElements( data )
{

}
uint8_t MemoryAreaWrite::GetICF()
{
    return Command::REQUEST_WITH_RESPONCE;
}
size_t MemoryAreaWrite::WriteImpl( uint8_t* buf, size_t size )
{
    if ( size < 48 + mAddr.mElementSize * mElements.size() )
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

    for ( auto it = mElements.begin(), end = mElements.end(); it != end; ++it )
    {
        Element const& element = **it;
        head = element.Write( head, mAddr.mElementSize );
    }
    return head - buf;
}
size_t MemoryAreaWrite::ReadImpl( uint8_t const* /*buf*/, size_t size, bool& res )
{
    res = true;
    return size;
}
size_t MemoryAreaWrite::RequestSizeImpl() const
{
    return sizeof( mAddr.mMemType ) +
           sizeof( mAddr.mAddr ) +
           sizeof( mAddr.mBit ) +
           sizeof( static_cast<uint16_t>( mElements.size() ) ) +
           mAddr.mElementSize * mElements.size();
}
size_t MemoryAreaWrite::ResponseSizeImpl() const
{
    return 0;
}

}//namespace fins

