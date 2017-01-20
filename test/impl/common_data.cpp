#include "common_data.h"
#include <QJsonObject>
#include <QMessageBox>
#include <thread>
#include <chrono>

namespace test
{

QString ErrMsg()
{
    QString str_errs;
    return str_errs;
}



TestCommonData::TestCommonData( QString const& name, uint8_t id ):
    test::Test(  name, id ),
    mCommand( cpu::CpuMemory::Instance().LaunchControl ),
    mIsPrepare(false)
{}

QJsonObject TestCommonData::Serialise() const
{
    QJsonObject obj;
    obj.insert("TestingTime", TestingTime );
    obj.insert("IsPrepare", mIsPrepare);

    return obj;
}
bool TestCommonData::Deserialize( QJsonObject const& obj )
{
    TestingTime = obj.value("TestingTime").toInt();
    mIsPrepare = obj.value("IsPrepare").toBool();
    return true;
}

uint8_t TestCommonData::CommandID()
{
    return mId;
}
void TestCommonData::Start()
{
    mCommand.Reset();
    mCommand.Write();

    StartTime.start();
    mIsPrepare = false;

    while( !mCommand.Done() )
    {
        mCommand.Read();
        UpdateData();
        if ( IsStopped() )
        {
            mCommand.Terminated( true );
            mCommand.Write();
            Log( "Испытание прервано" );
            return;
        }

        if ( mPrepareMarker && *mPrepareMarker )
        {
            *mPrepareMarker = false;
            mCommand.Prepare(true);
            mIsPrepare = true;
        }

        if ( mRunMarker && *mRunMarker )
        {
            *mRunMarker = false;
            mCommand.Start(true);
            mIsPrepare = false;
        }

        if ( mPauseMarker && *mPauseMarker )
        {
            *mPauseMarker = false;
            mCommand.Stop(true);
        }

        mCommand.Write();
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
    }
    TestingTime = StartTime.elapsed()/1000;
    *mTerminateMarker = !CheckErrors();
}


bool TestCommonData::CheckErrors()
{
    QString str_errs = ErrMsg();

    if ( str_errs.isEmpty() )
        return true;

    std::mutex mutex;
    std::unique_lock< std::mutex > lock( mutex );
    Launcher( std::bind( &TestCommonData::ShowErrors, this, str_errs ) );

    mCondVar.wait( lock );
    return false;
}
void TestCommonData::ShowErrors( QString const& err )
{
    QMessageBox msg;
    msg.setWindowTitle( "Ошибки тестирования" );
    msg.setText( err.left(200) + "..." );
    msg.addButton( QMessageBox::Ok );
    msg.setModal( true );
    msg.exec();
    mCondVar.notify_one();
}

}
