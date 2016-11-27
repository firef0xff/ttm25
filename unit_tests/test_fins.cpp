#include "communication/fins/paskage.h"
#include "communication/fins/endpoint.h"
#include "communication/fins/funcs/memory_area_read.h"
#include <cstring>
#include <stdexcept>
#include <vector>
#include "communication/fins/udp_communicator.h"
#include "communication/fins/mem_type.h"
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
    class test_Command :public fins::MemoryAreaRead
    {
    public:
        test_Command(fins::MemoryAddr const& addr, uint16_t el_count):
            MemoryAreaRead( addr, el_count )
        {

        }
        virtual size_t ReadImpl( uint8_t const* buf, size_t size, bool& res ) override
        {
            if ( size%mAddr.mElementSize )
                throw std::runtime_error("unexpected end of data");
            mData.clear();
            size_t elements_count = size/mAddr.mElementSize;
            mData.reserve( elements_count );

            uint8_t const* head = buf;
            for( size_t i = 0; i < elements_count; ++ i )
            {
                mData.push_back( static_cast<bool>( *(head) ) );
                head += mAddr.mElementSize;
            }
            res = true;
            return size;
        }

    private:
        std::vector<bool> mData;
    };

    TestPaskage()
    {
        using namespace fins;

        EndPoint source( EndPoint::NA_LOCAL, 1, EndPoint::A2_COMPUTER );
        EndPoint dest( EndPoint::NA_LOCAL, 0, EndPoint::A2_CPU );

        fins::BIT_CIO mem( 3568, 10 );
        test_Command c( mem, 2 );
        Paskage p( dest, source, c, 15 );
        auto* data = p.Data();
        uint8_t request[] = {Command::REQUEST_WITH_RESPONCE,0,2,
                             EndPoint::NA_LOCAL,0,EndPoint::A2_CPU,
                             EndPoint::NA_LOCAL,1,EndPoint::A2_COMPUTER,15,
                             0x1,0x1,0x30,0x0d,0xF0,10,0,2};
        if (memcmp(data, request, sizeof(request)))
            throw std::runtime_error("wrong request");
        uint8_t responce[] = {Command::RESPONCE,0,2,
                              EndPoint::NA_LOCAL,1,EndPoint::A2_COMPUTER,
                              EndPoint::NA_LOCAL,0,EndPoint::A2_CPU,15,
                              0x1,0x1,0,0,1};


        UDP_Communicator com( "192.168.0.2" );
        com.send( p.Data(), p.Size() );
    }


} t1;

}//namespace

}//namespace test
