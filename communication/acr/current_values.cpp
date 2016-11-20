#include "current_values.h"
#include "../swap_endian.h"

namespace acr
{

CurrentValues::CurrentValues()
    :Command(2)
{
}

size_t CurrentValues::WriteImpl( uint8_t* /*buf*/, size_t /*size*/ )
{
    return 0;
}
size_t CurrentValues::ReadImpl( uint8_t const* buf, size_t size, bool& res )
{
    res = false;
    if ( size < 20 )
        return 0;

    uint8_t const* head = buf;
    V = *reinterpret_cast<float const*>( head );
    head += sizeof( V );
    Q = *reinterpret_cast<float const*>( head );
    head += sizeof( Q );
    U = *reinterpret_cast<int32_t const*>( head );
    head += sizeof( U );
    Pu = *reinterpret_cast<int8_t const*>( head );
    head += sizeof( Pu );
    t = *reinterpret_cast<int32_t const*>( head );
    head += sizeof( t );
    ERR = *reinterpret_cast<int8_t const*>( head );
    head += sizeof( ERR );
    CRC = *reinterpret_cast<int16_t const*>( head );
    head += sizeof( CRC );
    res = true;
}


}//namespace acr

