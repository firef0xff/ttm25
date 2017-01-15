#pragma once
#include "test/test.h"
#include "cpu/cpu_memory.h"
#include <QVector>

namespace test
{

class Attestaion  : public Test
{
public:
    Attestaion ( QString const& name, uint8_t id );

    void Start( bool const& flag );
    virtual bool Success() const override;

    virtual void Reset() = 0;
    virtual void UpdateData() = 0;
    virtual void SetStartBit( bool b ) = 0;
    virtual void SetStopBit( bool b ) = 0;

    virtual QJsonObject Serialise() const override;
    virtual bool Deserialize( QJsonObject const& obj ) override;
    virtual bool Draw( QPainter& painter, QRect &free_rect, QString  const& compare_width ) const override;


protected:
    bool mSuccess;
    cpu::data::AttestationLaunchControls& mControls;

};

class AttPressure : public Attestaion
{
public:
    struct Data
    {
        Data();

        QJsonObject Serialise() const;
        bool Deserialize( QJsonObject const& obj );

        double mTask;
        double mResult;
        double mFact;
        bool mCurrent;
    };
    typedef QVector<Data> DataSet;


    AttPressure();
    virtual bool Run() override;
    virtual void UpdateData() override;
    virtual void SetStartBit( bool b ) override;
    virtual void SetStopBit( bool b ) override;

    void Reset();
    DataSet const& GetData() const;
    DataSet& GetData();

    virtual QJsonObject Serialise() const override;
    virtual bool Deserialize( QJsonObject const& obj ) override;
private:
    DataSet mData;
    int32_t mCurrenPos;
    bool mWait;
};

class AttTime : public Attestaion
{
public:
    AttTime();
    virtual bool Run() override;

    void Reset(){};
    virtual void UpdateData(){};
    virtual void SetStartBit( bool b ){};
    virtual void SetStopBit( bool b ){};

    virtual QJsonObject Serialise() const override;
    virtual bool Deserialize( QJsonObject const& obj ) override;
};

class AttPressureSpeed : public Attestaion
{
public:
    AttPressureSpeed();
    virtual bool Run() override;

    void Reset(){};
    virtual void UpdateData(){};
    virtual void SetStartBit( bool b ){};
    virtual void SetStopBit( bool b ){};

    virtual QJsonObject Serialise() const override;
    virtual bool Deserialize( QJsonObject const& obj ) override;
};
}//namespace test
