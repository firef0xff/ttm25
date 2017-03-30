#include "metran.h"
#include "communication/swap_endian.h"
#include "communication/metran/funcs/metran_read.h"
#include "communication/metran/metran_paskage.h"
#include "settings/settings.h"
#include <chrono>
namespace cpu
{

Metran::Metran()
{
}

Metran::~Metran()
{
}

Metran& Metran::Instance()
{
    static Metran m;
    return m;
}

void Metran::Init()
{
    //подготовка  таймаутов
    std::unique_ptr< COMMTIMEOUTS > TimeOuts ( new COMMTIMEOUTS() );
    TimeOuts->ReadIntervalTimeout = 0xFFFFFFFF;
    TimeOuts->ReadTotalTimeoutMultiplier = 0;
    TimeOuts->ReadTotalTimeoutConstant = 0;
    TimeOuts->WriteTotalTimeoutMultiplier = 0;
    TimeOuts->WriteTotalTimeoutConstant =0;

    QString Addr = app::Settings::Instance().ComAddr();
    mPort.reset(new COMPort( Addr.toStdString(), std::move( TimeOuts ), COMPort::CommMasks::ev_rxchar, 4096, 4096 )  );

    //корректировка настроек порта DCB

    std::unique_ptr< DCB> settings = mPort->Get_port_Settings();
    settings->BaudRate =COMPort::BaudRates::BR_9600;
    settings->ByteSize = 8;
    settings->Parity = COMPort::Paritys::NOparity;
    settings->StopBits = COMPort::StopBits::ONE;
    settings->fAbortOnError = FALSE;
    settings->fDtrControl = DTR_CONTROL_DISABLE;
    settings->fRtsControl = RTS_CONTROL_DISABLE;
    settings->fBinary = TRUE;
    settings->fParity = COMPort::fParitys::OFF;
    settings->fInX = FALSE;
    settings->fOutX = FALSE;
    settings->XonChar = (BYTE)0x11;
    settings->XoffChar = (BYTE)0x13;
    settings->fErrorChar = FALSE;
    settings->fNull = FALSE;
    settings->fOutxCtsFlow = FALSE;
    settings->fOutxDsrFlow = FALSE;
    settings->XonLim = 1024;
    settings->XoffLim = 1024;
    settings->fDsrSensitivity=FALSE;
    mPort->Set_DCB_Settings( std::move( settings ) );
}

void Metran::Read()
{
    if ( !mPort )
        Init();
    if ( !mPort )
        return;

    COMPort& port = *mPort;

    uint8_t const addr = app::Settings::Instance().MetranAddr(); //адрес устройства

    uint16_t const reg = 0x000F;            //первый регистр для чтения
    uint8_t const count = sizeof(mData);    //количество читаемых регистров

    uint16_t data[count] = {0};

    metran::Read cmd( reg, count, data );
    metran::Paskage pkg( addr, cmd );


    const int len = 1096;
    char buff[len] = {0};
    char* p_buff = &buff[0];
    size_t remain_len = pkg.ResponseSize();
    bool end = false;

//    port.DTR_On();
//    port->Clear_Com_Buff(	COMPort::Purge_flags::TXABORT|
//                            COMPort::Purge_flags::RXABORT|
//                            COMPort::Purge_flags::TXCLEAR|
//                            COMPort::Purge_flags::RXCLEAR);
//    port.Write( pkg.Data(), pkg.Size() );
//    port.DTR_oFF();
    auto start = std::chrono::system_clock::now();
    do
    {
        size_t readed = port.Read( reinterpret_cast<BYTE*>( p_buff ), remain_len );
        remain_len -= std::min( readed, remain_len );
        p_buff += readed;

//        if ( (std::chrono::system_clock::now() - start) > std::chrono::seconds(10) )
//            throw COMError("Read timeout");
    }
    while( !end && remain_len );    
    pkg.SetResponce( reinterpret_cast<uint8_t*>( buff ), len - remain_len );
}

float Metran::Pressure()
{
    return swap_endian( reinterpret_cast< float const& >( mData[1] ) );
}
uint8_t Metran::ErrorCode()
{
    return reinterpret_cast< uint8_t* >( mData )[0];
}
uint8_t Metran::WarnCode()
{
    return reinterpret_cast< uint8_t* >( mData )[1];
}

}
