#include "multiple_memory_area_read.h"

namespace fins
{

MultipleMemoryAreaRead::MultipleMemoryAreaRead( const Addrs &addr, Elements& data ):
    fins::Command(0x1,0x4),
    mAddr( addr ),
    mElements( data )
{

}

uint8_t MultipleMemoryAreaRead::GetICF()
{
    return Command::REQUEST_WITH_RESPONCE;
}
size_t MultipleMemoryAreaRead::WriteImpl( uint8_t* buf, size_t size )
{
    if ( size < RequestSizeImpl() )
        return size;

    uint8_t* head = buf;
    for ( auto it = mAddr.begin(), end = mAddr.end(); it != end; ++it )
    {
        MemoryAddr const& lnk = **(it);
        *(head++) = lnk.mMemType;
        auto addr = reinterpret_cast<uint8_t const*>(&lnk.mAddr);
        *(head++) = addr[1];
        *(head++) = addr[0];
        *(head++) = lnk.mBit;
    }

    return head - buf;
}
size_t MultipleMemoryAreaRead::ReadImpl( uint8_t const* buf, size_t size, bool& res )
{
    if ( size < ResponseSizeImpl() )
        throw std::runtime_error("unexpected end of data");

    uint8_t const* head = buf;
    auto it = mElements.begin(), end = mElements.end();
    auto it2 = mAddr.begin(), end2 = mAddr.end();
    for ( ;it != end && it2 != end2; ++it, ++it2 )
    {
        Element& element = **it;
        MemoryAddr const& addr = **it2;
        uint8_t mem_type = *(head++);
        if ( mem_type != addr.mMemType )
            throw std::runtime_error("unexpected memory type");
        head = element.Read( head, element.Size() );
    }
    res = true;
    return size;
}

size_t MultipleMemoryAreaRead::RequestSizeImpl() const
{
    size_t size = 0;

    if ( !mAddr.empty() )
    {
        MemoryAddr const& addr = **(mAddr.begin());
        size = sizeof( addr.mMemType ) +
               sizeof( addr.mAddr ) +
               sizeof( addr.mBit );
        size *= mAddr.size();
    }

    return size;
}
size_t MultipleMemoryAreaRead::ResponseSizeImpl() const
{
    if ( mElements.empty() || mAddr.empty() )
        return 0;

    size_t size = 0;
    MemoryAddr const& addr = **(mAddr.begin());
    for ( auto it = mElements.begin(), end = mElements.end(); it != end; ++it )
    {
        Element const& lnk= **it;
        size += lnk.Size() + sizeof(addr.mMemType);
    }
    return size;
}

}//namespace fins

