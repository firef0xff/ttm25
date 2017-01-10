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
    void run();
    void stop();
    void resume_fill();
    void resume_test();
    void terminate();
private:
    mutable bool mFill;
    mutable bool mTest;
    mutable bool mTerminateSignal;
//    bool mEtalone;
    void LogIt( QString const& str );
    void LaunchIt( Functor  func );
signals:
    void to_log( QString const& );
    void to_exec( Functor );
    void progress();

};
Q_DECLARE_METATYPE(Functor)
