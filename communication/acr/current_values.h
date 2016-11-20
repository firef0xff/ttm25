#pragma once
#include "command.h"

namespace acr
{

class CurrentValues : public Command
{
public:
    CurrentValues();

    float V = 0.0;      //V, м/c 4Б (float)
    float Q = 0.0;      //Q,м3/ч 4Б (float)
    uint32_t U = 0;      //U, м3  4Б (long int)
    uint8_t Pu = 0;      //Pu     1Б: lg(KU)+3, от 0 до 5
    uint32_t t = 0;      //t, мин 4Б (long int)
    uint8_t  ERR = 0;    //ERR    1Б
    uint16_t CRC = 0;    //CRC    2Б, младшим байтом вперед

private:
    virtual size_t WriteImpl( uint8_t* buf, size_t size );
    virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res );

};

}//namespace acr
