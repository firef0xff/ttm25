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
    bool Model( QString const& );
    QString const& Size() const;
    bool Size( QString const& );
    QString const& Customer() const;
    bool Customer( QString const& );
    qint32 OrderNo() const;
    bool OrderNo( QString const& );
    QString TireNo() const;
    bool TireNo( QString const& );
    double BreakPressure() const;
    bool BreakPressure( QString const& );
    qint32 ConstPressureTime() const;
    bool ConstPressureTime( QString const& );

    double Frequency() const;
    bool Frequency( QString const& );
    double Pressure() const;
    bool Pressure( QString const& );
    double Expenditure() const;
    bool Expenditure( QString const& );
    double PressureSpeed() const;
    bool PressureSpeed( QString const& );
    double Volume() const;
    bool Volume( QString const& );
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
