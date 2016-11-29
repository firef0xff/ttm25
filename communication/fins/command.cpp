#include "command.h"
#include <stdexcept>
#include <sstream>

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

    if ( mErrClass || mErrCode )
    {
        std::stringstream s;
        s << "Error while execute command "<< mMR << mSR <<
             " error code: " << mErrCode << " class: " << mErrClass;
        throw std::runtime_error( s.str() );
    }
    size_t readed = head - buf;
    return ReadImpl( head, size - readed, res ) + readed;
}

size_t Command::RequestSize() const
{
    return sizeof(mMR) + sizeof(mSR) + RequestSizeImpl();
}
size_t Command::ResponseSize() const
{
    return sizeof(mErrCode) + sizeof(mErrClass) + ResponseSizeImpl();
}
}//namespace fins
