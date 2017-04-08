#pragma once
#include "metran_command.h"

namespace metran
{
class Paskage
{
public:
    enum // размеры элементов пакета
    {
        HEADER_SIZE = sizeof(uint8_t) + sizeof(uint16_t),  //размер заголовка
        BODY_SIZE = 1024*4,
        PASKAGE_SIZE = HEADER_SIZE + BODY_SIZE
    };

    Paskage( uint8_t endpoint, Command& cmd );

    //вернет false в случае если пакет не удалось распознать либо ответ не соответствует запросу
    bool SetResponce( uint8_t const* buf, size_t size );

    uint8_t* Data();   //указатель на буфер данных
    size_t Size();     // размер пакета данных

    size_t RequestSize() const;
    size_t ResponseSize() const;
private:
    void Build();

    uint8_t mEndpoint = 0;
    Command& mRequest;
    uint16_t mCrc = 0;

    uint8_t mOutBuf[HEADER_SIZE + BODY_SIZE] = {0};
    size_t  mSize = 0;
};

}
