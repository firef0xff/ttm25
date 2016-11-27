#pragma once
#include <stdint.h>

namespace fins
{

class MemoryAddr
{
public:
    MemoryAddr( uint8_t mem_type, uint16_t max_mem_addr, uint8_t max_bit, uint8_t element_size,
                uint16_t mem_addr, uint8_t bit);

    const uint8_t     mMemType;
    const uint16_t    mMaxAddr;
    const uint8_t     mMaxBit;
    const uint8_t     mElementSize;
    const uint16_t    mAddr;
    const uint8_t     mBit;
};

}//namespace fins
