#include "metran_command.h"
#include "../buffer_tools.h"

namespace metran
{

Command::Command( uint8_t cmd ):
    mCmd(cmd)
{
}
size_t Command::Write( uint8_t* buf, size_t size )
{
    uint8_t* head = buf;
    head = ToBuff( mCmd, head );
    size_t writed = head - buf;
    return WriteImpl( head, size - writed ) + writed;
}
size_t Command::Read( uint8_t const* buf, size_t size, bool& res )
{
    uint8_t const* head = buf;
    uint8_t cmd = 0;
    head = FromBuff( cmd, head );
    size_t readed = head - buf;
    return ReadImpl( head, size - readed, res ) + readed;
}

size_t Command::RequestSize() const
{
    return sizeof(mCmd) + RequestSizeImpl();
}
size_t Command::ResponseSize() const
{
    return sizeof(mCmd) + ResponseSizeImpl();
}


}//namespace metran
