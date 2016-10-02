#include "paskage.h"
#include "endpoint.h"
#include "command.h"

namespace fins
{

Paskage::Paskage(EndPoint const& dest, EndPoint const& current, Command& req , uint8_t sid):
    mSID( sid ),
    mDestination( dest ),
    mSource( current ),
    mRequest( req )
{
    Build();
}

void Paskage::Build()
{
    uint8_t* head = mOutBuf;
    *(head++) = mRequest.GetICF(); //записать icf команды
    *(head++) = mRSV;//записать биты RSV GST
    *(head++) = mGST;
    head += mDestination.Write(head, HEADER_SIZE - ( head - mOutBuf ) ); //записать EndPoint получателя
    head += mSource.Write(head, HEADER_SIZE - ( head - mOutBuf ) ); //записать EndPoint отправителя
    *(head++) = mSID;//записать SID
    if ( head - mOutBuf == HEADER_SIZE )
        head += mRequest.Write( head, BODY_SIZE ); //записать код команды //записать тело команды
    mSize = head - mOutBuf;
}

bool Paskage::SetResponce( uint8_t const* buf, size_t size )
{
    if ( size < HEADER_SIZE )
        return false;

    uint8_t const* head = buf;
    uint8_t ics = *(head++);

    //если захочется сделать возможность вызова функций с контроллера
    //на компе нужно будет разрешить и обработать остальные варианты
    //на данном этапе достаточно возможностей чистого клиента
    if ( ics != Command::RESPONCE )
        return false;

    if ( *(head++) != mRSV )
        return false;
    if ( *(head++) != mGST )
        return false;

    bool res = false;
    head += mSource.Check( head, HEADER_SIZE - ( head - mOutBuf ), res );
    if (!res)
        return false;
    head += mDestination.Check( head, HEADER_SIZE - ( head - mOutBuf ), res );
    if (!res)
        return false;
    if (*(head++) != mSID )
        return false;

    head += mRequest.Read( head, size - HEADER_SIZE, res );

    return res;
}

uint8_t* Paskage::Data()
{
    return mOutBuf;
}
size_t Paskage::Size()
{
    return mSize;
}

bool Paskage::NeedAnsver()
{
    return mRequest.GetICF() == Command::REQUEST_WITH_RESPONCE;
}

}//namespace fins
