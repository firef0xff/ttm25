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
private:
    mutable bool mStopSignal;
//    bool mEtalone;
    void LogIt( QString const& str );
    void LaunchIt( Functor  func );
signals:
    void to_log( QString const& );
    void to_exec( Functor );
    void progress();

};
Q_DECLARE_METATYPE(Functor)
