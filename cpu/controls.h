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

class Controls :public InOut
{
public:

    void Write();
    void Read();
    void Reset();

    void UnknownOnOff( bool );
    bool UnknownOnOff() const;

    void RegulatingOnOff( bool );
    bool RegulatingOnOff() const;

    void ResetOnOff( bool );
    bool ResetOnOff() const;

    void FeedOnOff( bool );
    bool FeedOnOff() const;

    void AirOnOff( bool );
    bool AirOnOff() const;

    void VacuumOnOff( bool );
    bool VacuumOnOff() const;

    std::unique_lock<std::recursive_mutex> Locker();
protected:
    bool* W2_00;
    bool* W2_01;

    bool* W2_02;    //регулирующий закрыть
    bool* W2_03;    //регулирующий открыть

    bool* W2_04;    //сброс включить
    bool* W2_05;    //сброс отключить

    bool* W2_06;    //подача включить
    bool* W2_07;    //подача отключить

    bool* W2_08;    //атмосфера включить
    bool* W2_09;    //атмосфера отключить

    bool* W2_10;    //Вакуум включить
    bool* W2_11;    //Вакуум отключить

private:
    friend class cpu::CpuMemory;
    Controls();
    Controls( const Controls& ) = delete;
    void operator = ( const Controls& ) = delete;

    fins::Elements mData;
    fins::BIT_WR mAddr;
    std::recursive_mutex mMutex;
};

}//namespace data
}//namespace cpu
