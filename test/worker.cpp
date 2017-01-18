#include "worker.h"
#include "test_params.h"
#include "test.h"

Worker::Worker():
    mFill(false),
    mTest(false),
    mPause(false),
    mTerminateSignal(true)
{}
void Worker::run()
{
    mTerminateSignal = false;
    try
    {
        test::CURRENT_PARAMS->StendInit();
    }catch( std::exception const& e )
    {
        LogIt( e.what() );
        return;
    }
    test::Test* to_run = test::CURRENT_PARAMS->TestForExec();
    if (!to_run)
    {
        LogIt( "Нет тестов для запуска" );
        emit done();
        return;
    }

    LogIt( "Запущен тест: " + to_run->Name() );
    bool result = to_run->Run( std::bind( &Worker::LaunchIt, this, std::placeholders::_1 ),
                               std::bind( &Worker::LogIt, this, std::placeholders::_1 ),
                               mFill, mTest, mPause, mTerminateSignal );
    emit progress();
    if (result)
        LogIt( "Тест пройден" );
    else
        LogIt( "Тест не пройден" );
    LogIt( QString() );

    try
    {
        test::CURRENT_PARAMS->StendDeInit();
    }catch( std::exception const& e )
    {
        LogIt( e.what() );
        return;
    }
    emit done();
}
void Worker::stop()
{
    mPause = true;
}
void Worker::fill()
{
    mFill = true;
}
void Worker::test()
{
    mTest = true;
}
void Worker::terminate()
{
    mTerminateSignal = true;
}

void Worker::LogIt( QString const& str )
{
    emit to_log( str );
}

void Worker::LaunchIt( Functor func )
{
    emit to_exec( func );
}
