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

class AttestationLaunchControls : public InOut
{
public:
    void Write();
    void Read();
    void Reset();

    void AttPressureStart( bool b );
    bool AttPressureStart() const;

    void AttPressureStop( bool b );
    bool AttPressureStop() const;

    void AttPressureSave( bool b );
    bool AttPressureSave() const;

    void AttTimeStart( bool b );
    bool AttTimeStart() const;

    void AttTimeStop( bool b );
    bool AttTimeStop() const;

    void AttPressureSpeedStart( bool b );
    bool AttPressureSpeedStart() const;

    void AttPressureSpeedStop( bool b );
    bool AttPressureSpeedStop() const;

    std::unique_lock<std::recursive_mutex> Locker();
protected:
    bool* W10_00;    //Старт по времени
    bool* W10_01;    //Стоп по времени

    bool* W10_02;    //Старт по давлению
    bool* W10_03;    //Стоп по давлению
    bool* W10_04;    //Запомнить по давлению

    bool* W10_05;    //Старт по нарастанию давления
    bool* W10_06;    //Стоп по нарастанию давления

private:
    friend class cpu::CpuMemory;
    AttestationLaunchControls();
    AttestationLaunchControls( const AttestationLaunchControls& ) = delete;
    void operator = ( const AttestationLaunchControls& ) = delete;

    fins::Elements mData;
    fins::BIT_WR mAddr;
    std::recursive_mutex mMutex;
};

}//namespace data
}//namespace cpu
