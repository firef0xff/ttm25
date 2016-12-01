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
    if ( size < RequestSizeImpl() )
        return size;

    uint8_t* head = buf;
    *(head++) = mAddr.mMemType;
    auto addr = reinterpret_cast<uint8_t const*>(&mAddr.mAddr);
    *(head++) = addr[1];
    *(head++) = addr[0];
    *(head++) = mAddr.mBit;
    uint16_t el_count = 0;
    if ( !mElements.empty() )
        el_count = ( mElements.begin()->get()->Size() * mElements.size() ) / mAddr.mElementSize;
    auto count = reinterpret_cast<uint8_t const*>(&el_count);
    *(head++) = count[1];
    *(head++) = count[0];
    return head - buf;
}
size_t MemoryAreaRead::ReadImpl( uint8_t const* buf, size_t size, bool& res )
{
    if ( size < ResponseSizeImpl() )
        throw std::runtime_error("unexpected end of data");

    uint8_t const* head = buf;
    for ( auto it = mElements.begin(), end = mElements.end(); it != end; ++it )
    {
        Element& element = **it;
        head = element.Read( head, element.Size() );
    }
    res = true;
    return size;
}

size_t MemoryAreaRead::RequestSizeImpl() const
{
    static const size_t base_size = sizeof( mAddr.mMemType ) +
                                    sizeof( mAddr.mAddr ) +
                                    sizeof( mAddr.mBit ) +
                                    sizeof( static_cast<uint16_t>( mElements.size() ) );
    return base_size;
}
size_t MemoryAreaRead::ResponseSizeImpl() const
{
    if ( mElements.empty() )
        return 0;

    return mElements.begin()->get()->Size() * mElements.size();
}

}//namespace fins

