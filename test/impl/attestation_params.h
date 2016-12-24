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

private:
    AttestationParams();
    AttestationParams( AttestationParams const& ) = delete;
    void operator = ( AttestationParams const& ) = delete;
};

}//namespace test
