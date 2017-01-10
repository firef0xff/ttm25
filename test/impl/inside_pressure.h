#pragma once
#include "common_data.h"
#include <memory>
#include <functional>

namespace test
{

class M2_2006: public TestCommonData
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

    M2_2006( CallBack f, QString method_name = "Методика №2-2006", int32_t id = 1 );
    ~M2_2006();

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

protected:
    virtual bool DrawHeader( uint32_t& num, QPainter& painter, QRect &free_rect ) const;
    virtual bool DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const;
    virtual bool DrawFoter( uint32_t& num, QPainter& painter, QRect &free_rect ) const;

    virtual QString TableTitle() const;

    bool DrawGraph( uint32_t& num, QPainter& painter, QRect &free_rect, QString  const& compare_width ) const;

    friend class GrapfData;
    bool mSuccess = false;
    DataSet mPData;
    DataSet mVData;
    CallBack mOnDataUpdate;
    mutable std::unique_ptr<GrapfData> mGrapfs;
    float mLastTime;
    double mBreakPressure;
#warning TODO mState
    qint32 mState;
};

class EK_OON_106: public M2_2006
{
public:
    EK_OON_106(CallBack f);
    void UpdateData() override;
    QJsonObject Serialise() const;
    bool Deserialize( QJsonObject const& obj );

    virtual bool DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const override;
    virtual QString TableTitle() const override;
private:
    int32_t mConstPressureTime;
};


class M24_82: public M2_2006
{
public:
    M24_82(CallBack f);
    virtual QString TableTitle() const override;
};

class MI5C_2006: public M2_2006
{
public:
    MI5C_2006(CallBack f);
    virtual QString TableTitle() const override;
};

class DrawHelper
{
public:
    DrawHelper( QPainter& , QRect& );
    void DrawRowCenter( QRect const& place, QFont const& font, QColor const& color, QString const& text );
    void DrawRowLeft( QRect const& place,
                      QFont const& font,
                      QColor const& color1,
                      QString const& label,
                      QColor const& color2 = Qt::black,
                      QString const& value = "" );
private:
    QPainter& painter;
    QRect &free_rect;
};

}
