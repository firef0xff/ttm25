#pragma once
#include <stdint.h>
#include <stddef.h>
#include <atomic>
namespace fins
{

class Command;
class EndPoint;

/*
class Connection
{
public:
    Connection( EndPoint const& dest, EndPoint const& source );

    void Execute( Command& cmd );

private:
    EndPoint mDestination;
    EndPoint mSource;
};*/

/*класс для построения пакета данных
 *его цель - собрать/разобрать бинарный пакет, но не отправлять его куда либо
 */
class Paskage
{
public:
    enum // размеры элементов пакета
    {
        HEADER_SIZE = 10,   //размер заголовка
        BODY_SIZE = 2002    //размер тела пакета
    };
    Paskage( EndPoint const& dest, EndPoint const& source, Command& req );

    //вернет false в случае если пакет не удалось распознать либо ответ не соответствует запросу
    bool SetResponce( uint8_t const* buf, size_t size );

    uint8_t* Data();   //указатель на буфер данных
    size_t Size();     // размер пакета данных

    bool NeedAnsver(); //true если этот запрос требует ответа

    size_t RequestSize() const;
    size_t ResponseSize() const;
    uint8_t SID() const;
private:
    void Build();


    const uint8_t mRSV = 0; //резерв (всегда 00)
    const uint8_t mGST = 2; //счетчик шлюзов, всегда 2
    const uint8_t mSID = 0; // 0x0 - 0xff - Идентификатор сервиса (будет идентификатором подключения)

    EndPoint const& mDestination;
    EndPoint const& mSource;

    Command& mRequest;

    uint8_t mOutBuf[HEADER_SIZE + BODY_SIZE] = {0};
    size_t  mSize = 0;

    static std::atomic<uint8_t> SID_COUNTER;
};

}//namespace fins
