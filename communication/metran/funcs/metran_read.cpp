#include <cstring>
#include "metran_read.h"
#include "../../buffer_tools.h"

namespace metran
{

Read::Read(uint16_t reg, uint16_t count,
            uint16_t *resp, size_t resp_size ):
    Command(3),
    mRegNo(reg),
    mRegCount(count),
    mRespPkgSize(0),
    mResponce(resp),
    mRespSize(resp_size)
{}

size_t Read::WriteImpl( uint8_t* buf, size_t size )
{
    if (size < RequestSizeImpl() )
        return 0;

    uint8_t* head = buf;
    head = ToBuff( mRegNo, head );
    head = ToBuff( mRegCount, head );
    return head - buf;
}
size_t Read::ReadImpl( uint8_t const* buf, size_t size, bool& res )
{
    res = false;
    if (size < ResponseSizeImpl() )
        return 0;

    uint8_t const* head = buf;
    head = FromBuff( mRespPkgSize, head );
    memcpy( mResponce, head, mRespPkgSize );
    res = true;
    return head - buf;
}

size_t Read::RequestSizeImpl() const
{
    return sizeof( mRegNo ) + sizeof( mRegCount );
}
size_t Read::ResponseSizeImpl() const
{
    return sizeof(mRespPkgSize) + mRespSize*sizeof(*mResponce);
}

}
