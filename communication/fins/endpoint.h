#pragma once
#include <stdint.h>
#include <stddef.h>

namespace fins
{

class EndPoint
{
public:
    enum NA : uint8_t //адрес сети назначения
    {
        NA_LOCAL = 0 //локальная сеть
        //1-127 - удаленная сеть
    };
    enum A1 : uint8_t //адрес узла назначения
    {
        A1_LOCAL = 0, //внутри плк
        A1_COMMON = 0xff //широковещательный
        //1-32 - адрес в сети Controller Link
        //1-126 - адрес в сети Ethernet
    };
    enum A2 :uint8_t //адрес модуля назначения
    {
        A2_CPU = 0x0,         //0 - Модуль cpu
        A2_COMPUTER = 0x1,    //1 - Компьютер
        A2_ETHERNET = 0xfe, //- Contoller Linc или Ethernet, подключенный в сеть
        A2_BOARD = 0xe1     // - встраиваемая плата
        //0x10 - 0x1f -модуль шины CPU (номер модуля + 0x10)
        //0x20 - специальный модуль вывода (номер модуля + 0x20)
    };

    EndPoint ( uint8_t na, uint8_t a1, uint8_t a2 );

    size_t Write( uint8_t* buf, size_t size ) const;
    size_t Check( const uint8_t *buf, size_t size, bool& res ) const;

private:
    uint8_t mNA = NA_LOCAL;
    uint8_t mA1 = A1_LOCAL;
    uint8_t mA2 = A2_CPU;
};
}//namespace fins
