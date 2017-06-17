#include "metran/metran_paskage.h"
#include "metran/funcs/metran_read.h"
#include "swap_endian.h"

#include "cstring"
#include <stdexcept>

#include <QByteArray>

namespace test
{

class TestMetran
{
public:
    TestMetran()
    {
        uint8_t rq[] = { 0x01,0x03,0x00,0x0F,0x00,0x03,0x35,0xC8 };

        uint8_t rs1[] = { 0x01,0x03,0x06,0x00,0x00,0x39,0x3F,0xA4,0x38,0x66,0xF7 }; //0,0001827636
        uint8_t rs2[] = { 0x01,0x03,0x06,0x00,0x00,0x3B,0x07,0xE3,0x3A,0x54,0xB3 }; //0,00207348

        uint8_t const addr = 0x01;    //адрес устройства
        uint16_t const reg = 0x000F;  //первый регистр для чтения
        uint8_t const count = 0x3;    //количество читаемых регистров
        uint16_t const crc = 0x35C8;  //контрольная сумма

        uint16_t data[count] = {0};

        metran::Read cmd( reg, count, data );
        metran::Paskage pkg( addr, cmd );

        uint8_t* pkg_buf = pkg.Data();

        if (memcmp(pkg_buf, rq, sizeof(rq)))
            throw std::runtime_error("wrong request");

        pkg.SetResponce(rs1, sizeof(rs1) );

        uint16_t d3[] = {0x38,0xA4,0x3F,0x39};
        float const *res3 = reinterpret_cast< float const* >( &d3[0] );
        float res = swap_endian( reinterpret_cast< float const& >( data[1] ) );
        float val = 0.0001827636;
        return;
    }
};
#ifdef TESTS
TestMetran t2;
#endif
}
