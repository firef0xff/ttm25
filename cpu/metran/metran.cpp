#include "metran.h"
#include "communication/swap_endian.h"
#include "communication/metran/funcs/metran_read.h"
#include "communication/metran/metran_paskage.h"
#include "settings/settings.h"
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
    mPort.reset(new COMPort( Addr.toStdString(), std::move( TimeOuts ), COMPort::CommMasks::ev_rxchar, 4096, 2048 )  );

    //корректировка настроек порта DCB

    std::unique_ptr< DCB> settings = mPort->Get_port_Settings();
    settings->BaudRate =COMPort::BaudRates::BR_9600;
    settings->ByteSize = 8;
    settings->Parity = COMPort::Paritys::EVENparity;
    settings->StopBits = COMPort::StopBits::ONE;
    settings->fAbortOnError = FALSE;
    settings->fDtrControl = DTR_CONTROL_ENABLE;
    settings->fRtsControl = RTS_CONTROL_TOGGLE;
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
    uint8_t const count = sizeof(mData)/sizeof(*mData);    //количество читаемых регистров

    uint16_t data[count] = {0};

    metran::Read cmd( reg, count, data );
    metran::Paskage pkg( addr, cmd );


    const int len = 1096;
    char buff[len] = {0};
    char* p_buff = &buff[0];
    size_t ans_len = pkg.ResponseSize();
    size_t remain_len = len;

    port.Clear_Com_Buff(	COMPort::Purge_flags::TXABORT|
                            COMPort::Purge_flags::RXABORT|
                            COMPort::Purge_flags::TXCLEAR|
                            COMPort::Purge_flags::RXCLEAR);
    port.Write( pkg.Data(), pkg.Size() );

    size_t total_readed = 0;
    do
    {
        size_t readed = port.Read( reinterpret_cast<BYTE*>( p_buff ), remain_len );
        total_readed += readed;
        remain_len -= readed;
        p_buff += readed;
    }
    while( total_readed < ans_len );
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
