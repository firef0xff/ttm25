#include "communication/fins/paskage.h"
#include "communication/fins/endpoint.h"
#include "communication/fins/command.h"
#include <cstring>
#include <stdexcept>
#include <vector>

namespace test
{

namespace
{
//класс который знает
//код области
//диапазон адресов
//размер элемента и умеет его читать писать

class DataInfo
{
public:
    DataInfo()
    {}
    virtual uint8_t MemCode() = 0;
    virtual uint8_t ElementSize() = 0;
    virtual size_t ReadElement( uint8_t const* buf, size_t size ) = 0;
    virtual size_t WriteElement( uint8_t const* buf, size_t size ) = 0;
};

class TestPaskage
{
public:
    class test_Command :public fins::Command
    {
    public:
        test_Command():
            fins::Command(0x1,0x1)
        {
            mMemCode = 30;
            mAddrWord = 3568;
            mAddrBit = 10;
            mCount = 1;
        }
        virtual uint8_t GetICF() override
        {
            return Command::REQUEST_WITH_RESPONCE;
        }
        virtual size_t WriteImpl( uint8_t* buf, size_t size ) override
        {
            if ( size < 48 )
                return size;

            uint8_t* head = buf;
            *(head++) = mMemCode;
            auto addr = reinterpret_cast<uint8_t*>(&mAddrWord);
            *(head++) = addr[1];
            *(head++) = addr[0];
            *(head++) = mAddrBit;
            *(head++) = mCount;
            return head - buf;
        }
        virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res ) override
        {
            if ( size%mElementSize )
                throw std::runtime_error("unexpected end of data");
            mData.clear();
            size_t elements_count = size/mElementSize;
            mData.reserve( elements_count );

            uint8_t const* head = buf;
            for( size_t i = 0; i < elements_count; ++ i )
            {
                mData.push_back( static_cast<bool>( *(head) ) );
                head += mElementSize;
            }
            res = true;
            return size;
        }

    private:
        uint8_t  mMemCode = 0;
        uint16_t mAddrWord = 0;
        uint8_t  mAddrBit = 0;
        uint16_t mCount = 0;

        uint8_t mElementSize = 1;
        std::vector<bool> mData;
    };

    TestPaskage()
    {
        using namespace fins;

        EndPoint source( EndPoint::NA_LOCAL, 55, EndPoint::A2_COMPUTER );
        EndPoint dest( EndPoint::NA_LOCAL, 26, EndPoint::A2_CPU );

        test_Command c;
        Paskage p( dest, source, c, 15 );
        auto* data = p.Data();
        uint8_t request[] = {Command::REQUEST_WITH_RESPONCE,0,2,
                             EndPoint::NA_LOCAL,26,EndPoint::A2_CPU,
                             EndPoint::NA_LOCAL,55,EndPoint::A2_COMPUTER,15,
                             0x1,0x1,30,0x0d,0xF0,10,1};
        if (memcmp(data, request, sizeof(request)))
            throw std::runtime_error("wrong request");
        uint8_t responce[] = {Command::RESPONCE,0,2,
                              EndPoint::NA_LOCAL,55,EndPoint::A2_COMPUTER,
                              EndPoint::NA_LOCAL,26,EndPoint::A2_CPU,15,
                              0x1,0x1,0,0,1};

    }


} t1;

}//namespace

}//namespace test
