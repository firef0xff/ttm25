#pragma once
#include "common_data.h"
#include <memory>

namespace test
{

class InsidePressure: public TestCommonData
{
public:
    InsidePressure( QString const& name, uint8_t id );
    ~InsidePressure();

    class GrapfData;
    class Point
    {
    public:
        Point( double x, double y );

        QJsonObject Serialise() const;
        bool Deserialize( QJsonObject const& obj );

        double const& X() const;
        double const& Y() const;
    private:
        double mX;
        double mY;
    };
    typedef QVector<Point> DataSet;

    bool Run();
    bool Success() const;

    QJsonObject Serialise() const;
    bool Deserialize( QJsonObject const& obj );

    bool Draw( QPainter& painter, QRect &free_rect, QString  const& compare_width ) const;

private:
    friend class GrapfData;
    bool mSuccess = false;
    DataSet mData;
    mutable std::unique_ptr<GrapfData> mGrapfs;
};


}
