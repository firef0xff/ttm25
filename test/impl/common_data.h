#pragma once
#include "../test.h"
#include <QDateTime>
#include <condition_variable>
#include "cpu/cpu_memory.h"

namespace test
{
class TestCommonData : public Test
{
public:
    TestCommonData( QString const& name, uint8_t id );

protected:
    int TestingTime = 0;
    QTime StartTime;
    std::condition_variable mCondVar;

    QJsonObject Serialise() const;
    bool Deserialize( QJsonObject const& obj );

    void Start();
    bool CheckErrors();
    virtual uint8_t CommandID();
    virtual void UpdateData(){}
    cpu::data::LaunchControl& mCommand;
private:
    void ShowErrors( QString const& err );
};
}
