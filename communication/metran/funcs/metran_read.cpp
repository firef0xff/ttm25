#include <cstring>
#include "metran_read.h"
#include "../../buffer_tools.h"
#include "../../swap_endian.h"

namespace metran
{

Read::Read(uint16_t reg, uint16_t count,
            uint16_t *resp):
    Command(3),
    mRegNo(reg),
    mRegCount(count),
    mResponce(resp)
{}

size_t Read::WriteImpl( uint8_t* buf, size_t size )
{
    if (size < RequestSizeImpl() )
        return 0;

    uint8_t* head = buf;
    head = ToBuff( swap_endian( mRegNo ), head );
    head = ToBuff( swap_endian( mRegCount ), head );
    return head - buf;
}
size_t Read::ReadImpl( uint8_t const* buf, size_t size, bool& res )
{
    res = false;
    if (size < ResponseSizeImpl() )
        return 0;

    uint8_t const* head = buf;
    uint8_t resp_size = 0;
    head = FromBuff( resp_size, head );
    memcpy( mResponce, head, resp_size );
    head += resp_size;
    res = true;
    return head - buf;
}

size_t Read::RequestSizeImpl() const
{
    return sizeof( mRegNo ) + sizeof( mRegCount );
}
size_t Read::ResponseSizeImpl() const
{
    return mRegCount*sizeof(*mResponce) + sizeof(uint8_t);
}

}
