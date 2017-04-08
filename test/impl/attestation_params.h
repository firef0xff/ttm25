#pragma once

#include "../test_params.h"

namespace test
{

class AttestationParams : public Parameters
{
public:
    static AttestationParams& Instance();

    ~AttestationParams();

    void Reset() override;
    virtual QString ToString() const;

    virtual QJsonObject Serialise() const;
    virtual bool Deserialize( QJsonObject const& obj );

    virtual void WriteToController() const;

    double PressureSpeed() const;
    bool PressureSpeed( QString const& );

    void UpdatePeriod( int );
    int UpdatePeriod() const;

    bool PrintAll() const;
    void DrawLogo(QPainter &, QRect &, const QPixmap &) const;
    bool Draw(QPainter &painter, QRect &free_rect , const QString &compare_width) const;
private:
    AttestationParams();
    AttestationParams( AttestationParams const& ) = delete;
    void operator = ( AttestationParams const& ) = delete;

    double mPressureSpeed;
    int mUpdatePeriod;
};

}//namespace test
