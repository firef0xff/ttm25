#include "communication/fins/paskage.h"
#include "communication/fins/endpoint.h"
#include "communication/fins/funcs/memory_area_read.h"
#include "communication/fins/funcs/memory_area_write.h"
#include <cstring>
#include <stdexcept>
#include <vector>
#include "communication/fins/communicator.h"
#include "communication/fins/mem_type.h"
namespace test
{

namespace
{
using namespace fins;

class TestPaskage
{
public:   
    class test_Command :public fins::MemoryAreaRead
    {
    public:
        test_Command(fins::MemoryAddr const& addr, fins::Elements& el):
            MemoryAreaRead( addr, el )
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
        TestBinaryBuild();
        TestReadWrite();
    }

    void TestBinaryBuild()
    {
        EndPoint source( EndPoint::NA_LOCAL, 1, EndPoint::A2_COMPUTER );
        EndPoint dest( EndPoint::NA_LOCAL, 0, EndPoint::A2_CPU );

        fins::BIT_CIO mem( 3568, 10 );
        Elements els;
        els.push_back( BOOL::Create(true) );
        els.push_back( BOOL::Create(false) );

        test_Command c( mem, els );
        Paskage p( dest, source, c );
        auto* data = p.Data();
        uint8_t request[] = {Command::REQUEST_WITH_RESPONCE,0,2,
                             EndPoint::NA_LOCAL,0,EndPoint::A2_CPU,
                             EndPoint::NA_LOCAL,1,EndPoint::A2_COMPUTER,p.SID(),
                             0x1,0x1,0x30,0x0d,0xF0,10,0,2};
        if (memcmp(data, request, sizeof(request)))
            throw std::runtime_error("wrong request");
        uint8_t responce[] = {Command::RESPONCE,0,2,
                              EndPoint::NA_LOCAL,1,EndPoint::A2_COMPUTER,
                              EndPoint::NA_LOCAL,0,EndPoint::A2_CPU,p.SID(),
                              0x1,0x1,0,0,1};

    }

    void TestReadWrite()
    {
        EndPoint source( EndPoint::NA_LOCAL, 1, EndPoint::A2_CPU );
        EndPoint dest( EndPoint::NA_LOCAL, 1, EndPoint::A2_CPU );
        fins::WORD_CIO mem( 3568, 0 );

        Elements els;
        els.push_back( LREAL::Create(71000404.24800812l) );
        els.push_back( INT64::Create(51120404.24800812l) );

        MemoryAreaWrite w_cmd( mem, els );
        Paskage write( dest, source, w_cmd );

        Elements els2;
        els2.push_back( LREAL::Create(0) );
        els2.push_back( LREAL::Create(0) );
        MemoryAreaRead r_cmd( mem, els2 );
        Paskage read( dest, source, r_cmd );

        Communicator com( "192.168.0.2" );
        com.slotSendToServer( write );
        com.slotSendToServer( read );

        for ( auto it = els.begin(),
                   it2 = els2.begin(),
                   end = els.end(),
                   end2 = els2.end(); it != end && it2 != end2; ++it, ++it2 )
        {
            INT64 const& e1 = static_cast< INT64 const& >( **it );
            INT64 const& e2 = static_cast< INT64 const& >( **it2 );

            if ( e1.Data() != e2.Data() )
                throw std::runtime_error("e1 != e2 ");

        }
        return;
    }

} t1;

}//namespace

}//namespace test
