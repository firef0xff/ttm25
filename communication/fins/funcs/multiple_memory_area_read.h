#pragma once
#include "../command.h"
#include "../memoryaddr.h"
#include "../data_element.h"
#include <vector>
namespace fins
{



class MultipleMemoryAreaRead : public Command
{
public:
    typedef std::vector< MemoryAddr const*> Addrs;
    MultipleMemoryAreaRead(const Addrs &addr, Elements& data);

    virtual uint8_t GetICF() override;
    virtual size_t WriteImpl( uint8_t* buf, size_t size ) override;
    virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res ) override;

protected:
    virtual size_t RequestSizeImpl() const;
    virtual size_t ResponseSizeImpl() const;

    Addrs const& mAddr;
    Elements& mElements;
};

}//namespace fins
