#pragma once
#include <stdint.h>
#include <stddef.h>

namespace fins
{

class Command
{
public:
    enum ICF : uint8_t  //уйдет в команду (параметром шаблона)
    {
        // пос (0b76543210)
        //6: 0 команда 1-ответ
        //0: 0 требуется ответ 1 ответ не требуется

        REQUEST_WITH_RESPONCE = 0b10000000,
        REQUEST_WITHOUT_RESPONCE = 0b10000001,
        RESPONCE = 0b11000001
    };
    enum
    {
        REQUEST_CODE = 2,   //место под код команды
        REQUEST_BODY = 2000,//место для тела команды
        RESPONCE_CODE = 2,  //место для кода ответа
        RESPONCE_BODY = 1998//место для тела ответа
    };

    Command( uint8_t mr, uint8_t sr );
    virtual uint8_t GetICF() = 0;
    size_t Write( uint8_t* buf, size_t size );
    size_t Read( uint8_t const* buf, size_t size, bool& res );

private:
    virtual size_t WriteImpl( uint8_t* buf, size_t size ) = 0;
    virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res ) = 0;

    uint8_t mMR = 0;
    uint8_t mSR = 0;

    uint8_t mErrClass = 0;
    uint8_t mErrCode = 0;
};

}//namespace fins

