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

    void PumpOnOff( bool );
    bool PumpOnOff() const;

    void RegulatingOpen( bool );
    bool RegulatingOpen() const;

    void RegulatingClose( bool );
    bool RegulatingClose() const;

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
    bool* H0_00;    //насос вкл/выкл
    bool* H0_01;    //резерв

    bool* H0_02;    //регулирующий закрыть
    bool* H0_03;    //регулирующий открыть

    bool* H0_04;    //сброс включить
    bool* H0_05;    //сброс отключить

    bool* H0_06;    //подача включить
    bool* H0_07;    //подача отключить

    bool* H0_08;    //атмосфера включить
    bool* H0_09;    //атмосфера отключить

    bool* H0_10;    //Вакуум включить
    bool* H0_11;    //Вакуум отключить

private:
    friend class cpu::CpuMemory;
    Controls();
    Controls( const Controls& ) = delete;
    void operator = ( const Controls& ) = delete;

    fins::Elements mData;
    fins::BIT_HR mAddr;
    std::recursive_mutex mMutex;
};

}//namespace data
}//namespace cpu
