#include "metran_write.h"
#include "../../buffer_tools.h"
#include <cstring>

namespace metran
{

Write::Write(uint16_t reg, uint16_t count, uint16_t *data, size_t data_size):
    Command(16),
    mRegNo(reg),
    mRegCount(count),
    mData(reinterpret_cast<uint8_t*>(data)),
    mDataSize( data_size * sizeof(*data) )
{}


size_t Write::WriteImpl( uint8_t* buf, size_t size )
{
    if (size < RequestSizeImpl() )
        return 0;

    uint8_t* head = buf;
    head = ToBuff( mRegNo, head );
    head = ToBuff( mRegCount, head );
    head = ToBuff( mDataSize, head );
    memcpy( head, mData, mDataSize );
    head += mDataSize;
    return head - buf;
}
size_t Write::ReadImpl( uint8_t const* buf, size_t size, bool& res )
{
    res = false;
    if (size < ResponseSizeImpl() )
        return 0;

    uint8_t const* head = buf;
    uint16_t reg_no = 0;
    uint16_t reg_count = 0;
    head = FromBuff( reg_no, head );
    head = FromBuff( reg_count, head );
    res = (reg_no == mRegNo) && ( mRegCount == reg_count );
    return head - buf;
}

size_t Write::RequestSizeImpl() const
{
    return sizeof( mRegNo ) + sizeof( mRegCount ) +
           sizeof( mDataSize ) + mDataSize;
}
size_t Write::ResponseSizeImpl() const
{
    return sizeof( mRegNo ) + sizeof( mRegCount );
}
}
