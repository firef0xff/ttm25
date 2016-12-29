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

    double Frequency() const;
    void Frequency( QString const& );
    double Pressure() const;
    void Pressure( QString const& );
    double Expenditure() const;
    void Expenditure( QString const& );
    double PressureSpeed() const;
    void PressureSpeed( QString const& );
    double Volume() const;
    void Volume( QString const& );
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

    //контроллер
    double mFrequency;
    double mPressure;
    double mExpenditure;
    double mPressureSpeed;
    double mVolume;
};

}//namespace test
