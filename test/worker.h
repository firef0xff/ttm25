#pragma once

#include <QThread>
#include <memory>
#include <functional>

typedef std::function< void() > Functor;
class Worker : public QThread
{
    Q_OBJECT
public:
    Worker();
    ~Worker();
    void run();
    void stop();
    void fill();
    void test();
    void terminate();
private:
    mutable bool mFill;
    mutable bool mTest;
    mutable bool mPause;
    mutable bool mTerminateSignal;
//    bool mEtalone;
    void LogIt( QString const& str );
    void LaunchIt( Functor  func );
signals:
    void to_log( QString const& );
    void to_exec( Functor );
    void progress();
    void done();

};
Q_DECLARE_METATYPE(Functor)
