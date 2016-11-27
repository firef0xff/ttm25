#pragma once
#include "../command.h"
#include "../memoryaddr.h"

namespace fins
{



class MemoryAreaRead : public Command
{
public:
    MemoryAreaRead(const MemoryAddr &addr, uint16_t el_count);

    virtual uint8_t GetICF() override;
    virtual size_t WriteImpl( uint8_t* buf, size_t size ) override;
    virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res ) override;

protected:
    MemoryAddr const& mAddr;
    uint16_t mElCount;
};

}//namespace fins
