#include "command.h"

namespace acr
{

Command::Command( uint8_t cmd ):
    mCmd(cmd)
{
}
size_t Command::Write( uint8_t* buf, size_t size )
{
    uint8_t* head = buf;
    *(head++) = 0xAA;
    *(head++) = mCmd;
    size_t writed = head - buf;
    return WriteImpl( head, size - writed ) + writed;
}
size_t Command::Read( uint8_t const* buf, size_t size, bool& res )
{
    uint8_t const* head = buf;
    size_t readed = head - buf;
    return ReadImpl( head, size - readed, res ) + readed;
}

}//namespace acr
