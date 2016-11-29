#pragma once
#include "../command.h"
#include "../memoryaddr.h"
#include "../data_element.h"

namespace fins
{

class MemoryAreaWrite : public Command
{
public:
    MemoryAreaWrite( const MemoryAddr &addr, Elements const& data );

    virtual uint8_t GetICF() override;
    virtual size_t WriteImpl( uint8_t* buf, size_t size ) override;
    virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res ) override;

private:
    virtual size_t RequestSizeImpl() const;
    virtual size_t ResponseSizeImpl() const;

    MemoryAddr const& mAddr;
    Elements const& mElements;
};

}//namespace fins
