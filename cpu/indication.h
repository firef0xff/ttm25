#pragma once
#include "data_block_base.h"
#include "../communication/fins/data_element.h"
#include "../communication/fins/mem_type.h"
#include <inttypes.h>
#include <vector>
#include <mutex>

namespace cpu
{

class CpuMemory;

namespace data
{

class Indication :public In
{
public:
    void Read();
    void Reset();

    bool PBU() const;
    bool MUP() const;

    bool NVK() const;
    bool UVN() const;

    bool UVM() const;
    bool ZVN() const;

    bool KPO() const;
    bool KPZ() const;

    bool XO() const;
    bool XZ() const;
protected:
    bool* W5_00;    //ПБУ
    bool* W5_01;    //МУП

    bool* W5_02;    //НВК
    bool* W5_03;    //УВН

    bool* W5_04;    //УВМ
    bool* W5_05;    //ЗВН

    bool* W5_06;    //КПО
    bool* W5_07;    //КПЗ

    bool* W5_08;    //ХО
    bool* W5_09;    //ХЗ

private:
    friend class cpu::CpuMemory;
    Indication();
    Indication( const Indication& ) = delete;
    void operator = ( const Indication& ) = delete;

    fins::Elements mData;
    fins::BIT_WR mAddr;
};

}//namespace data
}//namespace cpu
