#include "endpoint.h"

namespace fins
{

EndPoint::EndPoint ( uint8_t na, uint8_t a1, uint8_t a2 ):
    mNA( na ),
    mA1( a1 ),
    mA2( a2 )
{}

size_t EndPoint::Write( uint8_t* buf, size_t size ) const
{
    if ( size < 3ul )
        return 0;
    buf[0] = mNA;
    buf[1] = mA1;
    buf[3] = mA2;
    return 3ul;
}
size_t EndPoint::Check( uint8_t const* buf, size_t size, bool& res ) const
{
    if ( size < 3ul )
        return 0;
    res =  buf[0] == mNA
        && buf[1] == mA1
        && buf[3] == mA2;
    return 3ul;
}

}//namespace fins

