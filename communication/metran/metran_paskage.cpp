#include "metran_paskage.h"
#include "../buffer_tools.h"

namespace metran
{

Paskage::Paskage( uint8_t endpoint, Command& cmd ):
    mEndpoint( endpoint ),
    mRequest( cmd )
{
    Build();
}

//вернет false в случае если пакет не удалось распознать либо ответ не соответствует запросу
bool SetResponce( uint8_t const* buf, size_t size );

uint8_t* Paskage::Data()
{
    return mOutBuf;
}
size_t Paskage::Size()
{
    return mSize;
}

size_t Paskage::RequestSize() const
{
    return HEADER_SIZE + mRequest.RequestSize();
}
size_t Paskage::ResponseSize() const
{
    return HEADER_SIZE + mRequest.ResponseSize();
}

void Paskage::Build()
{
    uint8_t* head = mOutBuf;
    head = ToBuff( mEndpoint, head );
    size_t writed = head - mOutBuf;
    head += mRequest.Write( head, PASKAGE_SIZE - writed );
    head = ToBuff( mCrc, head );
    mSize = head - mOutBuf;
}

}
