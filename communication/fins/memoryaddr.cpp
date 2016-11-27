#include "memoryaddr.h"
#include <stdexcept>

namespace fins
{

MemoryAddr::MemoryAddr( uint8_t mem_type, uint16_t max_mem_addr, uint8_t max_bit, uint8_t element_size,
                        uint16_t mem_addr, uint8_t bit):
    mMemType    ( mem_type ),
    mMaxAddr    ( max_mem_addr ),
    mMaxBit     ( max_bit ),
    mElementSize( element_size ),
    mAddr       ( mem_addr ),
    mBit        ( bit )
{
    if ( mAddr > mMaxAddr || ( mAddr == mMaxAddr && mBit > mMaxBit ) )
        throw std::runtime_error( "wrong cpu addr" );
}

}//namespace fins
