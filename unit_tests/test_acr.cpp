#include "communication/acr/current_values.h"
#include "cstring"
#include <stdexcept>

namespace test
{

class TestAcr
{
public:
    TestAcr()
    {
        uint8_t request[] = {0xAA,0x02};
        uint8_t responce[] = {0x9B,0x61,0xB9,0x3F,
                              0xFE,0xC5,0xAF,0x42,
                              0x2D,0x04,0x00,0x00,
                              0x02,
                              0x4E,0x00,0x00,0x00,
                              0x00,
                              0x6B,0xB9};


        acr::CurrentValues cv;
        uint8_t out[10] = {0};
        cv.Write( out, 10 );
        if (memcmp(out, request, sizeof(request)))
            throw std::runtime_error("wrong request");

        bool res = false;
        cv.Read( responce, sizeof(responce), res );
        if (!res)
            throw std::runtime_error("wrong responce length");

        if ( static_cast<int32_t>(cv.V*1000) != 1448 )
            throw std::runtime_error("wrong cv.V");
        if ( static_cast<int32_t>(cv.Q*1000) != 87886 )
            throw std::runtime_error("wrong cv.Q");
        if (cv.U != 1069)
            throw std::runtime_error("wrong cv.U");
        if (cv.Pu != 2)
            throw std::runtime_error("wrong cv.Pu");
        if (cv.t != 78)
            throw std::runtime_error("wrong cv.t");
        if (cv.ERR != 0)
            throw std::runtime_error("wrong cv.ERR");
        uint16_t s = 0xb96b;
        if (cv.CRC != s )
            throw std::runtime_error("wrong cv.CRC");
    }
}t2;
}
