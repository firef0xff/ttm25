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

class ErrCodes :public InOut
{
public:
    void Read();
    void Write();
    void Reset();

    bool IsFaucetClose();
    bool IsLowWaterLavel();
    bool IsHightPressureSpeed();
    bool IsLowPressure();
    bool IsHightPressure();
    bool IsLowPumpFrequency();
protected:

    bool* W12_00; //- кран закрыт
    bool* W12_01; //- низкий уровень воды в баке
    bool* W12_02; //- скорость нарастания давления выше допустимого
    bool* W12_03; //- давление удержания ниже заданного
    bool* W12_04; // давление удержания выше заданного
    bool* W12_05; //- частота привода насоса ниже допустимого

private:
    friend class cpu::CpuMemory;
    ErrCodes();
    ErrCodes( const ErrCodes& ) = delete;
    void operator = ( const ErrCodes& ) = delete;

    fins::Elements mData;
    fins::BIT_WR mAddr;
};

}//namespace data
}//namespace cpu
