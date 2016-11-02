#pragma once
#include "../test.h"
#include <QDateTime>
#include <condition_variable>

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
    void Wait( bool& work, bool& done);
    bool CheckErrors();
    virtual uint8_t CommandID();
    virtual void UpdateData(){}
//    cpu::data::DB31& mCommand;
private:
    void ShowErrors( QString const& err );
}
