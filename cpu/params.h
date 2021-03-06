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

class Params :public Out
{
public:
    void Write();
    void Reset();

    void Frequency( float );
    void Pressure( float );
    void Expenditure( float );
    void PressureSpeed( float );
    void Volume( float );
    void ConstPressureTime( float ) ;
    void BreakPressure( bool );
protected:
    float* D00; //задание частоты
    float* D02; //задание давления
    float* D04; //задание расхода
    float* D06; //задание нарастания давления
    float* D08; //теоретический объем наполнения
    float* D10; //поддержка постоянного давления
    bool*  W3_15; //Испытание до разрыва
private:
    friend class cpu::CpuMemory;
    Params();
    Params( const Params& ) = delete;
    void operator = ( const Params& ) = delete;

    fins::Elements mData;
    fins::WORD_DM mAddr;

    fins::Elements mbData;
    fins::BIT_WR mbAddr;
};

}//namespace data
}//namespace cpu
