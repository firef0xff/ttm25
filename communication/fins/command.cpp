#include "command.h"

namespace fins
{

Command::Command( uint8_t mr, uint8_t sr ):
    mMR( mr ),
    mSR( sr )
{
}

size_t Command::Write( uint8_t* buf, size_t size )
{
    uint8_t* head = buf;
    *(head++) = mMR;
    *(head++) = mSR;
    size_t writed = head - buf;
    return WriteImpl( head, size - writed ) + writed;
}
size_t Command::Read( uint8_t const* buf, size_t size, bool& res )
{
    uint8_t const* head = buf;
    if( *(head++) != mMR )
    {
        res = false;
        return 0;
    }
    if( *(head++) != mSR )
    {
        res = false;
        return 0;
    }
    mErrClass = *(head++);
    mErrCode = *(head++);
    size_t readed = head - buf;
    return ReadImpl( head, size - readed, res ) + readed;
}

}//namespace fins
