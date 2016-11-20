#pragma once
#include <stdint.h>
#include <stddef.h>

namespace acr
{

class Command
{
public:
    Command( uint8_t cmd );
    size_t Write( uint8_t* buf, size_t size );
    size_t Read( uint8_t const* buf, size_t size, bool& res );
private:
    virtual size_t WriteImpl( uint8_t* buf, size_t size ) = 0;
    virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res ) = 0;

    uint8_t mCmd = 0;
};

}
