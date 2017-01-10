#pragma once
#include "controls.h"
#include "sensors.h"
#include "params.h"
#include "launch_control.h"

namespace cpu
{

class CpuMemory
{
public:
    ~CpuMemory();
    static CpuMemory& Instance();

    data::Controls Controls;    //Управление стендом
    data::Sensors Sensors;      //Датчики стенда
    data::Params Params;        //параметры проведения испытаний
    data::LaunchControl LaunchControl; //Управление запуском тестов
private:
    CpuMemory();
    CpuMemory( CpuMemory const& ) = delete;
    CpuMemory& operator = ( CpuMemory const& ) = delete;
};

}//namespace cpu
