#pragma once
#include <QString>
#include <QVariant>
#include <functional>
#include <stdint.h>
#include <QPainter>
#include <functional>

namespace test
{

bool AllocatePlace(QRect& place, int height, QRect& source );
typedef std::function< void ( QRect const& ) > DrawLineHandler;
bool DrawLine ( uint32_t& printed, uint32_t& num, QRect& source, QFont font, DrawLineHandler do_draw, double space = 1, int custom_height = 0 );

class Test
{
public:
    typedef std::function< void( QString const& ) > LogFunction;
    typedef std::function< void() > Handler;
    typedef std::function< void( Handler ) > LaunchFunction;
    Test( QString const& name, uint8_t id );
    virtual ~Test();
    bool Run( LaunchFunction, LogFunction, bool&, bool&, bool&, bool& );
    virtual bool Run() = 0;
    virtual bool Success() const = 0;

    QString const& Name() const;
    uint8_t const& ID() const;

    virtual QJsonObject Serialise() const = 0;
    virtual bool Deserialize( QJsonObject const& obj ) = 0;

    // true если больше нечего выводить ecли false то предоставленного куска мало
    virtual bool Draw( QPainter& painter, QRect &free_rect, QString  const& compare_width ) const = 0;

    virtual void ResetDrawLine();  


protected:
    bool IsStopped();
    LogFunction Log;
    LaunchFunction Launcher;
    QString mName;


    bool DrawLine ( uint32_t &num, QRect& source, QFont font, DrawLineHandler do_draw, double space = 1, int custom_height = 0 ) const;

protected:
    uint8_t mId;
    bool* mPrepareMarker;
    bool* mRunMarker;
    bool* mPauseMarker;
    bool* mTerminateMarker;
private:
    Test( Test const& );
    void operator = ( Test const& );

    mutable uint32_t mDrawLine; //отметка на которой остановилась печать
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
}//namespace test

Q_DECLARE_METATYPE( test::Test* )
