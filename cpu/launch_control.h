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

class LaunchControl : public InOut
{
public:

    void Write();
    void Read();
    void Reset();

    void Prepare( bool );
    bool Prepare() const;

    void Start( bool );
    bool Start() const;

    void Stop( bool );
    bool Stop() const;

    void Terminated( bool );
    bool Terminated() const;

    void Done( bool );
    bool Done() const;

    std::unique_lock<std::recursive_mutex> Locker();
protected:
    bool* W3_00;    //Старт наполнения
    bool* W3_01;    //Старт нарастания давления

    bool* W3_02;    //Стоп наполнение/нарастания
    bool* W3_03;    //Аварийный сброс

    bool* W3_04;    //индикатор останова

private:
    friend class cpu::CpuMemory;
    LaunchControl();
    LaunchControl( const LaunchControl& ) = delete;
    void operator = ( const LaunchControl& ) = delete;

    fins::Elements mData;
    fins::BIT_WR mAddr;
    std::recursive_mutex mMutex;
};

}//namespace data
}//namespace cpu
