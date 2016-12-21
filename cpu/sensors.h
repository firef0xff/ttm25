#pragma once
#include "data_block_base.h"
#include "../communication/fins/data_element.h"
#include "../communication/fins/mem_type.h"
#include <inttypes.h>
#include <vector>

namespace cpu
{

class CpuMemory;

namespace data
{

class Sensors :public In
{
public:
    void Read();
    void Reset();

    float Pressure() const;
    float Expenditure() const;
    float Volume() const;
    float Time() const;
protected:
    float* D50; //фактическое давление
    float* D52; //мгновенный расход
    float* D54; //объем
    float* D56; //текущее время

private:
    friend class cpu::CpuMemory;
    Sensors();
    Sensors( const Sensors& ) = delete;
    void operator = ( const Sensors& ) = delete;

    fins::Elements mData;
    fins::WORD_DM mAddr;
};

}//namespace data
}//namespace cpu
