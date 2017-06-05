#include "attestation_params.h"
#include "../../cpu/cpu_memory.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <mutex>
#include "../test.h"
namespace test
{
namespace
{
std::unique_ptr<AttestationParams> INST;
std::mutex MUTEX;
}
AttestationParams& AttestationParams::Instance()
{
    if ( !INST )
    {
        std::lock_guard< std::mutex > lock( MUTEX );
        if ( !INST )
        {
            INST.reset( new AttestationParams() );
        }
    }
    return *INST;
}

AttestationParams::AttestationParams():
    mPressureSpeed(0)
{}
AttestationParams::~AttestationParams()
{}

void AttestationParams::Reset()
{
    Parameters::Reset();
    mPressureSpeed = 0;
}
QString AttestationParams::ToString() const
{
    return QString();
}

QJsonObject AttestationParams::Serialise() const
{
    auto obj = Parameters::Serialise();
    obj.insert("PressureSpeed", mPressureSpeed);
    obj.insert("UpdatePeriod", mUpdatePeriod);

    obj.insert("ProtoDate", mProtoDate);
    obj.insert("ProtoNo", mProtoNo);

    return obj;
}
bool AttestationParams::Deserialize( QJsonObject const& obj )
{
    Parameters::Deserialize( obj );
    bool res = obj.contains("PressureSpeed") && obj.contains("UpdatePeriod");
    mPressureSpeed = obj.value("PressureSpeed").toDouble();
    mUpdatePeriod = obj.value("UpdatePeriod").toInt();

    mProtoDate = obj.value("ProtoDate").toString();
    mProtoNo = obj.value("ProtoNo").toString();
    return res;
}

void AttestationParams::WriteToController() const
{
    auto& params = cpu::CpuMemory::Instance().Params;
    params.Reset();
    params.PressureSpeed( mPressureSpeed );
    params.Write();
}

double AttestationParams::PressureSpeed() const
{
    return mPressureSpeed;
}
bool AttestationParams::PressureSpeed( QString const& val )
{
    return ParseValue( mPressureSpeed, val );
}
void AttestationParams::UpdatePeriod( int val )
{
    mUpdatePeriod = val;
}
int AttestationParams::UpdatePeriod() const
{
    return mUpdatePeriod;
}

void AttestationParams::ProtoDate( QString const& val )
{
    mProtoDate = val;
}
QString AttestationParams::ProtoDate() const
{
    return mProtoDate;
}

void AttestationParams::ProtoNo( QString const& val )
{
    mProtoNo = val;
}
QString AttestationParams::ProtoNo() const
{
    return mProtoNo;
}

bool AttestationParams::PrintAll() const
{
    return false;
}

void AttestationParams::DrawLogo( QPainter &/*painter*/, QRect &/*work_area*/, const QPixmap & ) const
{}
bool AttestationParams::Draw(QPainter &painter, QRect &free_rect , const QString &/*compare_width*/) const
{
    if ( auto ptr = TestForExec() )
        if ( ptr->ID() != 0 )
            return true;

    bool res = false;
    uint32_t num = 0;
    uint32_t printed = 0;

    QFont header_font = painter.font();
    header_font.setFamily("Arial");
    header_font.setPointSize( 14 );
    header_font.setBold(true);
    auto& params = AttestationParams::Instance();

    DrawHelper drw( painter, free_rect );

    res = DrawLine( printed, num, free_rect, header_font,
    [ this, &painter, &drw, &header_font ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, header_font, Qt::black, "ПРИЛОЖЕНИЕ");
    }, 1.5 );

    header_font.setPointSize( 12 );
    res = DrawLine( printed, num, free_rect, header_font,
    [ this, &painter, &drw, &header_font, &params ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, header_font, Qt::black, "к протоколу No " + ProtoNo() + " от " + ProtoDate() + " переодической аттестации стенда СИШ-25" );
    }, 1.5 );
    res = DrawLine( printed, num, free_rect, header_font,
    []( QRect const& )
    {
    }, 1.5 );
    return res;
}

}//namespace test

