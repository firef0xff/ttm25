#pragma once
#include <stdint.h>
#include "../../../mylib/COMPort/ComPort.h"
namespace cpu
{
class COMError : public std::runtime_error
{
public:
    COMError ( std::string const& err ):
        std::runtime_error( err )
    {}
};

class Metran
{
public:
    ~Metran();
    static Metran& Instance();

    void Read();

    float Pressure();
    uint8_t ErrorCode();
    uint8_t WarnCode();

private:
    Metran();
    Metran( Metran const& ) = delete;
    void operator = ( Metran const& ) = delete;

    void Init();

    uint16_t mData[3];
    std::unique_ptr< COMPort > mPort;
};

}
