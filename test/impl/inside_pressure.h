#pragma once
#include "common_data.h"
#include <memory>
#include <functional>

namespace test
{

class InsidePressure: public TestCommonData
{    
public:
    typedef std::function<void()> CallBack;

    enum PressureUnits
    {
        puMPA = 0,
        puKGS = 1
    };

    enum TimeUnits
    {
        tuMin = 0,
        tuSec = 1
    };

    InsidePressure( CallBack f );
    ~InsidePressure();

    class GrapfData;
    class Point
    {
    public:
        Point();
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
    void UpdateData() override;


    QJsonObject Serialise() const;
    bool Deserialize( QJsonObject const& obj );

    bool Draw( QPainter& painter, QRect &free_rect, QString  const& compare_width ) const;
    void PaintGraph(QPainter& painter, QFont const& font, const QRect &rect,
                    QString  const& compare_width,
                    double skale = 0.98,
                    PressureUnits pu = puKGS,
                    TimeUnits tu = tuMin ) const;

private:

    bool DrawAuto( uint32_t& num, QPainter& painter, QRect &free_rect ) const;
    bool DrawAvia( uint32_t& num, QPainter& painter, QRect &free_rect ) const;
    bool DrawGraph( uint32_t& num, QPainter& painter, QRect &free_rect, QString  const& compare_width ) const;

    friend class GrapfData;
    bool mSuccess = false;
    DataSet mData;
    CallBack mOnDataUpdate;
    mutable std::unique_ptr<GrapfData> mGrapfs;

};


}
