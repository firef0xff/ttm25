#include "worker.h"
#include "test_params.h"
#include "test.h"

Worker::Worker():
    mStopSignal(false)
{}
void Worker::run()
{
    mStopSignal = false;
    try
    {
        test::CURRENT_PARAMS->StendInit();
    }catch( std::exception const& e )
    {
        LogIt( e.what() );
        return;
    }
    test::Test* to_run = test::CURRENT_PARAMS->TestForExec();

    LogIt( "Запущен тест: " + to_run->Name() );
    bool result = to_run->Run( std::bind( &Worker::LaunchIt, this, std::placeholders::_1 ),
                               std::bind( &Worker::LogIt, this, std::placeholders::_1 ),
                               mStopSignal );
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
}
void Worker::stop()
{
    mStopSignal = true;
    wait();
}

void Worker::LogIt( QString const& str )
{
    emit to_log( str );
}

void Worker::LaunchIt( Functor func )
{
    emit to_exec( func );
}
