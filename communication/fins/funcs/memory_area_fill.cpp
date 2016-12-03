#include "memory_area_fill.h"

namespace fins
{

MemoryAreaFill::MemoryAreaFill( const MemoryAddr &addr, Element const& data, int16_t count ):
    fins::Command(0x1,0x3),
    mAddr( addr ),
    mElement( data ),
    mCount( count )

{
}
uint8_t MemoryAreaFill::GetICF()
{
    return Command::REQUEST_WITH_RESPONCE;
}
size_t MemoryAreaFill::WriteImpl( uint8_t* buf, size_t size )
{
    if ( size < RequestSizeImpl() )
        return size;

    uint8_t* head = buf;
    *(head++) = mAddr.mMemType;
    auto addr = reinterpret_cast<uint8_t const*>(&mAddr.mAddr);
    *(head++) = addr[1];
    *(head++) = addr[0];
    *(head++) = mAddr.mBit;
    auto count = reinterpret_cast<uint8_t const*>(&mCount);
    *(head++) = count[1];
    *(head++) = count[0];
    head = mElement.Write( head, mElement.Size() );

    return head - buf;
}
size_t MemoryAreaFill::ReadImpl( uint8_t const* /*buf*/, size_t size, bool& res )
{
    res = true;
    return size;
}
size_t MemoryAreaFill::RequestSizeImpl() const
{
    static const size_t base_size = sizeof( mAddr.mMemType ) +
                                    sizeof( mAddr.mAddr ) +
                                    sizeof( mAddr.mBit ) +
                                    sizeof( mCount );

    return base_size + mElement.Size();
}
size_t MemoryAreaFill::ResponseSizeImpl() const
{
    return 0;
}

}//namespace fins

