#pragma once

#include "../test_params.h"

namespace test
{

class WorkParams : public Parameters
{
public:
    static WorkParams& Instance();

    ~WorkParams();

    void Reset() override;
    virtual QString ToString() const;

    virtual QJsonObject Serialise() const;
    virtual bool Deserialize( QJsonObject const& obj );

    virtual void WriteToController() const;

    QString const& Model() const;
    void Model( QString const& );

    QString const& Size() const;
    void Size( QString const& );

    QString const& Customer() const;
    void Customer( QString const& );

    qint32 OrderNo() const;
    void OrderNo( QString const& );

    QString TireNo() const;
    void TireNo( QString const& );

    double BreakPressure() const;
    void BreakPressure( QString const& );

    qint32 ConstPressureTime() const;
    void ConstPressureTime( QString const& );

private:
    WorkParams();
    WorkParams( WorkParams const& ) = delete;
    void operator = ( WorkParams const& ) = delete;

    //шапка
    QString mModel;
    QString mSize;
    QString mCustomer;
    qint32 mOrderNo;

    QString mTireNo;

    double mBreakPressure;
    qint32 mConstPressureTime;
};

}//namespace test
