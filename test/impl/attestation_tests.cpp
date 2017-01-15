#include "attestation_tests.h"
#include <thread>
#include <QJsonObject>
#include <QJsonArray>
namespace test
{

Attestaion::Attestaion ( QString const& name, uint8_t id ):
    Test(name, id),
    mSuccess(false),
    mControls(cpu::CpuMemory::Instance().AttestationLaunchControls)
{}

void Attestaion::Start( bool const& flag )
{
    mControls.Reset();
    mControls.Write();

    while( !flag )
    {
        mControls.Read();
        UpdateData();
        if ( IsStopped() )
        {
            SetStopBit( true );
            Log( "Испытание прервано" );
            return;
        }

        if ( mPrepareMarker && *mPrepareMarker )
        {
            *mPrepareMarker = false;
            SetStartBit( true );
        }

        if ( mRunMarker && *mRunMarker )
        {//кнопка записать
            *mRunMarker = false;
        }

        if ( mPauseMarker && *mPauseMarker )
        {//пока не используется
            *mPauseMarker = false;
        }

        mControls.Write();
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
    }
}
bool Attestaion::Success() const
{
    return mSuccess;
}

QJsonObject Attestaion::Serialise() const
{
    QJsonObject obj;
    obj.insert("Success",mSuccess);
    return obj;
}
bool Attestaion::Deserialize( QJsonObject const& obj )
{
    mSuccess = obj.value("Success").toBool();
    return true;
}
bool Attestaion::Draw( QPainter& painter, QRect &free_rect, QString  const& compare_width ) const
{
    return true;
}


//------------------------------------------------------~

namespace
{
QJsonArray ToJson( AttPressure::DataSet const& data )
{
    QJsonArray arr;
    foreach ( AttPressure::Data const& d, data )
    {
        arr.insert( arr.end(), d.Serialise() );
    }
    return std::move( arr );
}
AttPressure::DataSet FromJson( QJsonArray const& arr )
{
    AttPressure::DataSet data;

    foreach (QJsonValue const& v, arr)
    {
        AttPressure::Data d;
        if ( d.Deserialize( v.toObject() ) )
            data.insert( data.end(), d );
    }
    return std::move( data );
}
}

AttPressure::Data::Data():
    mTask(0),
    mResult(0),
    mFact(0),
    mCurrent(false)
{}

QJsonObject AttPressure::Data::Serialise() const
{
    QJsonObject obj;
    obj.insert("Task",  mTask);
    obj.insert("Result",mResult);
    obj.insert("Fact",  mFact);
    obj.insert("Current",  mCurrent);
    return obj;
}
bool AttPressure::Data::Deserialize( QJsonObject const& obj )
{
    mTask   = obj.value("Task").toDouble();
    mResult = obj.value("Result").toDouble();
    mFact   = obj.value("Fact").toDouble();
    mCurrent   = obj.value("Current").toDouble();
    return true;
}


AttPressure::AttPressure():
    Attestaion("Аттестация по давлению", 0)
{
    Reset();
}

bool AttPressure::Run()
{
    Start( mControls.AttPressureSave() );
    if ( IsStopped() )
        return false;

    return Success();
    return true;
}
void AttPressure::UpdateData()
{
    auto const& mem = cpu::CpuMemory::Instance().Sensors;
    if (mCurrenPos<mData.size())
    {
        auto& dt = mData[mCurrenPos];
        dt.mResult = mem.Pressure();
        if (mControls.AttPressureSave() && !mWait)
        {
            for ( auto it = mData.begin(), end = mData.end(); it != end; ++it )
            {
                it->mCurrent = false;
            }
            dt.mCurrent = true;
            mWait = true;
            ++mCurrenPos;
        }
    }
    if ( mRunMarker && *mRunMarker )
    {
        *mRunMarker = false;
        mWait = false;
        if (mControls.AttPressureSave())
            mControls.AttPressureSave(false);
    }

}
void AttPressure::SetStartBit( bool b )
{
    mControls.AttPressureStart( b );
}
void AttPressure::SetStopBit( bool b )
{
    mControls.AttPressureStop( b );
}

void AttPressure::Reset()
{
    mCurrenPos = 0;
    mWait=false;
    mData.clear();
    for ( int i = 5; i <= 100; i += 5  )
    {
        Data d;
        d.mTask = i;
        if ( i == 5 )
            d.mCurrent = true;
        mData.push_back( std::move( d ) );
    }
}
AttPressure::DataSet const& AttPressure::GetData() const
{
    return mData;
}
AttPressure::DataSet& AttPressure::GetData()
{
    return mData;
}

QJsonObject AttPressure::Serialise() const
{
    QJsonObject obj = Attestaion::Serialise();
    obj.insert("CurrenPos",mCurrenPos);
    obj.insert("Data", ToJson(mData));
    obj.insert("Wait",mWait);
    return obj;
}
bool AttPressure::Deserialize( QJsonObject const& obj )
{
    mData = FromJson( obj.value("Data").toArray() );
    mCurrenPos = obj.value("CurrenPos").toInt();
    mWait = obj.value("Wait").toBool();
    Attestaion::Deserialize( obj );
    return true;
}

//------------------------------------------------------~
AttTime::AttTime():
    Attestaion("Аттестация по времени", 1)
{}
bool AttTime::Run()
{
    return true;
}
QJsonObject AttTime::Serialise() const
{
    return QJsonObject();
}
bool AttTime::Deserialize( QJsonObject const& obj )
{
    return true;
}

//------------------------------------------------------~
AttPressureSpeed::AttPressureSpeed():
    Attestaion("Аттестация по времени нарастания давления", 2)
{}
bool AttPressureSpeed::Run()
{
    return true;
}
QJsonObject AttPressureSpeed::Serialise() const
{
    return QJsonObject();
}
bool AttPressureSpeed::Deserialize( QJsonObject const& obj )
{
    return true;
}


}
