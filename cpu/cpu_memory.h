#pragma once
#include "controls.h"
#include "sensors.h"

namespace cpu
{

class CpuMemory
{
public:
    ~CpuMemory();
    static CpuMemory& Instance();

    data::Controls Controls;    //Управление стендом
    data::Sensors Sensors;      //Датчики стенда

private:
    CpuMemory();
    CpuMemory( CpuMemory const& ) = delete;
    CpuMemory& operator = ( CpuMemory const& ) = delete;
};

}//namespace cpu
