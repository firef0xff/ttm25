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

    void Start();
    virtual bool Success() const override;

    virtual void Reset() = 0;
    virtual void UpdateData() = 0;
    virtual void SetStartBit( bool b ) = 0;
    virtual void SetStopBit( bool b ) = 0;
    virtual bool StopBit() = 0;

    virtual QJsonObject Serialise() const override;
    virtual bool Deserialize( QJsonObject const& obj ) override;
    virtual bool Draw( QPainter& painter, QRect &free_rect, QString  const& compare_width ) const override;

    virtual void PaintGraph(QPainter& painter, QFont const& font, const QRect &rect,
                    double skale = 0.98 ) const;

    virtual void ResetDrawLine() override;

protected:
    virtual bool DrawHeader( uint32_t& num, QPainter& painter, QRect &free_rect ) const;
    virtual bool DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const = 0;
    virtual bool DrawFoter( uint32_t& num, QPainter& painter, QRect &free_rect ) const;
    virtual bool DrawGraph( uint32_t& num, QPainter& painter, QRect &free_rect ) const;

    bool mSuccess;
    cpu::data::AttestationLaunchControls& mControls;

    mutable int PrintedRows = 0;
    mutable int PrintedPage = 0;
};

class AttPressure : public Attestaion
{
public:
    class GrapfData;
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
    virtual bool StopBit() override;

    void Reset();
    DataSet const& GetData() const;
    DataSet& GetData();

    virtual QJsonObject Serialise() const override;
    virtual bool Deserialize( QJsonObject const& obj ) override;

    virtual void PaintGraph(QPainter& painter, QFont const& font, const QRect &rect,
                    double skale) const override;
private:
    virtual bool DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const;

    DataSet mData;
    int32_t mCurrenPos;
    bool mWait;
    mutable std::unique_ptr<GrapfData> mGrapfs;
};

class AttTime : public Attestaion
{
public:
    class GrapfData;
    struct Data
    {
        Data();

        QJsonObject Serialise() const;
        bool Deserialize( QJsonObject const& obj );

        double mCpuTime;
        double mResult;
        bool mCurrent;
    };
    typedef QVector<Data> DataSet;
    AttTime();
    virtual bool Run() override;

    void Reset();
    DataSet const& GetData() const;
    DataSet& GetData();
    virtual void UpdateData();
    virtual void SetStartBit( bool b );
    virtual void SetStopBit( bool b );
    virtual bool StopBit() override;

    virtual QJsonObject Serialise() const override;
    virtual bool Deserialize( QJsonObject const& obj ) override;

    virtual void PaintGraph(QPainter& painter, QFont const& font, const QRect &rect,
                    double skale) const override;
private:
    virtual bool DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const;

    DataSet mData;
    int32_t mCurrenPos;
    bool mWait;
    mutable std::unique_ptr<GrapfData> mGrapfs;
};

class AttPressureSpeed : public Attestaion
{
public:
    class GrapfData;
    struct Data
    {
        Data();

        QJsonObject Serialise() const;
        bool Deserialize( QJsonObject const& obj );

        double mCpuTime;
        double mResult;
    };
    typedef QVector<Data> DataSet;

    AttPressureSpeed();
    virtual bool Run() override;

    DataSet const& GetData() const;
    DataSet& GetData();

    void Reset();
    virtual void UpdateData();
    virtual void SetStartBit( bool b );
    virtual void SetStopBit( bool b );
    virtual bool StopBit() override;

    virtual QJsonObject Serialise() const override;
    virtual bool Deserialize( QJsonObject const& obj ) override;

    virtual void PaintGraph(QPainter& painter, QFont const& font, const QRect &rect,
                    double skale) const override;
private:
    virtual bool DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const;

    DataSet mData;
    int32_t mCurrenTime;
    mutable std::unique_ptr<GrapfData> mGrapfs;
};
}//namespace test
