#include "communication/fins/paskage.h"
#include "communication/fins/endpoint.h"
#include "communication/fins/command.h"

namespace test
{

namespace
{

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
            return Command::REQUEST_WITHOUT_RESPONCE;
        }
        virtual size_t Write( uint8_t* buf, size_t size ) override
        {
            if ( size < 48 )
                return size;

            uint8_t* head = buf;
            *(head++) = mMemCode;
            auto addr = reinterpret_cast<uint8_t*>(&mAddrWord);
            *(head++) = addr[0];
            *(head++) = addr[1];
            *(head++) = mAddrBit;
            *(head++) = mCount;
            return head - buf;
        }
        virtual size_t Read( uint8_t const* /*buf*/, size_t size, bool& res ) override
        {
            res = true;
            return size;
        }

    private:
        uint8_t  mMemCode = 0;
        uint16_t mAddrWord = 0;
        uint8_t  mAddrBit = 0;
        uint16_t mCount = 0;
    };

    TestPaskage()
    {
        using namespace fins;

        EndPoint source( EndPoint::NA_LOCAL, 55, EndPoint::A2_COMPUTER );
        EndPoint dest( EndPoint::NA_LOCAL, 26, EndPoint::A2_CPU );

        test_Command c;
        Paskage p( dest, source, c, 15 );
        auto* data = p.Data();
    }


} t1;

}//namespace

}//namespace test
