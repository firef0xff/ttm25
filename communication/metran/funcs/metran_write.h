#pragma once
#include "../metran_command.h"
namespace metran
{
class Write : public Command
{
public:
    Write(uint16_t reg, uint16_t count, uint16_t* resp, size_t resp_size);

private:
    virtual size_t WriteImpl( uint8_t* buf, size_t size ) override;
    virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res ) override;

    virtual size_t RequestSizeImpl() const override;
    virtual size_t ResponseSizeImpl() const override;

    uint16_t mRegNo;
    uint16_t mRegCount;

    uint8_t* mData;
    uint8_t mDataSize;
};

}
