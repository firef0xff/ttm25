#pragma once
#include "memoryaddr.h"

namespace fins
{
template < uint8_t TYPE, uint16_t MAX_ADDR, uint8_t MAX_BIT, uint8_t EL_SIZE >
class TemplateAddr :public MemoryAddr
{
public:
    TemplateAddr( uint16_t mem_addr, uint8_t bit ):
        MemoryAddr( TYPE, MAX_ADDR, MAX_BIT, EL_SIZE, mem_addr, bit )
    {}
};


typedef TemplateAddr<0x30, 6143, 15, 1 > BIT_CIO;
typedef TemplateAddr<0x31, 511,  15, 1 > BIT_WR;
typedef TemplateAddr<0x32, 511,  15, 1 > BIT_HR;
typedef TemplateAddr<0x33, 959,  15, 1 > BIT_AR;
typedef TemplateAddr<0x02, 32767,15, 1 > BIT_DM;
typedef TemplateAddr<0x20, 32767,15, 1 > BIT_EM0;
typedef TemplateAddr<0x21, 32767,15, 1 > BIT_EM1;
typedef TemplateAddr<0x22, 32767,15, 1 > BIT_EM2;
typedef TemplateAddr<0x23, 32767,15, 1 > BIT_EM3;
typedef TemplateAddr<0x24, 32767,15, 1 > BIT_EM4;
typedef TemplateAddr<0x25, 32767,15, 1 > BIT_EM5;
typedef TemplateAddr<0x26, 32767,15, 1 > BIT_EM6;
typedef TemplateAddr<0x27, 32767,15, 1 > BIT_EM7;
typedef TemplateAddr<0x28, 32767,15, 1 > BIT_EM8;
typedef TemplateAddr<0x29, 32767,15, 1 > BIT_EM9;
typedef TemplateAddr<0x2a, 32767,15, 1 > BIT_EMA;
typedef TemplateAddr<0x2b, 32767,15, 1 > BIT_EMB;
typedef TemplateAddr<0x2c, 32767,15, 1 > BIT_EMC;

typedef TemplateAddr<0x70, 6143, 15, 1 > BIT_CIO_P;
typedef TemplateAddr<0x71, 511,  15, 1 > BIT_WR_P;
typedef TemplateAddr<0x72, 511,  15, 1 > BIT_HR_P;


typedef TemplateAddr<0xb0, 6143, 0, 2 > WORD_CIO;
typedef TemplateAddr<0xb1, 511,  0, 2 > WORD_WR;
typedef TemplateAddr<0xb2, 511,  0, 2 > WORD_HR;
typedef TemplateAddr<0xb3, 959,  0, 2 > WORD_AR;
typedef TemplateAddr<0x82, 32767,0, 2 > WORD_DM;
typedef TemplateAddr<0xa0, 32767,0, 2 > WORD_EM0;
typedef TemplateAddr<0xa1, 32767,0, 2 > WORD_EM1;
typedef TemplateAddr<0xa2, 32767,0, 2 > WORD_EM2;
typedef TemplateAddr<0xa3, 32767,0, 2 > WORD_EM3;
typedef TemplateAddr<0xa4, 32767,0, 2 > WORD_EM4;
typedef TemplateAddr<0xa5, 32767,0, 2 > WORD_EM5;
typedef TemplateAddr<0xa6, 32767,0, 2 > WORD_EM6;
typedef TemplateAddr<0xa7, 32767,0, 2 > WORD_EM7;
typedef TemplateAddr<0xa8, 32767,0, 2 > WORD_EM8;
typedef TemplateAddr<0xa9, 32767,0, 2 > WORD_EM9;
typedef TemplateAddr<0xaa, 32767,0, 2 > WORD_EMA;
typedef TemplateAddr<0xab, 32767,0, 2 > WORD_EMB;
typedef TemplateAddr<0xac, 32767,0, 2 > WORD_EMC;
typedef TemplateAddr<0x98, 32767,0, 2 > WORD_EMCUR;
typedef TemplateAddr<0xbc,     0,0, 2 > WORD_EMCUR_NO;


typedef TemplateAddr<0xf0, 6143, 0, 4 > WORD_CIO_P;
typedef TemplateAddr<0xf1, 511,  0, 4 > WORD_WR_P;
typedef TemplateAddr<0xf2, 511,  0, 4 > WORD_HR_P;
}
