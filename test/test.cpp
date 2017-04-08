#include "test.h"

namespace test
{

Test::Test(QString const& name, uint8_t id)
    : mName(name),
      mId(id),
      mPrepareMarker(nullptr),
      mRunMarker(nullptr),
      mPauseMarker(nullptr),
      mTerminateMarker(nullptr),
      mDrawLine( 0 )
{
}

Test::~Test()
{
}



QString const& Test::Name() const
{
    return mName;
}
uint8_t const& Test::ID() const
{
    return mId;
}
bool Test::Run(LaunchFunction to_launch, LogFunction to_log, bool& prepare, bool& run, bool& pause, bool& terminate )
{
    Log = to_log;
    Launcher = to_launch;
    mPrepareMarker = &prepare;
    mRunMarker = &run;
    mPauseMarker = &pause;
    mTerminateMarker = &terminate;

    bool res = Run();

    Log = LogFunction();
    Launcher = LaunchFunction();
    mPrepareMarker = nullptr;
    mRunMarker = nullptr;
    mPauseMarker = nullptr;
    mTerminateMarker = nullptr;

    return res;
}

bool Test::IsStopped()
{
    return  mTerminateMarker && *mTerminateMarker;
}

void Test::ResetDrawLine()
{
    mDrawLine = 0;
}

bool AllocatePlace( QRect& place, int height, QRect& source )
{
    if ( source.height() < height)
        return false;

    place.setTop( source.top() );
    place.setLeft( source.left() );
    place.setHeight( height );
    place.setWidth( source.width() );

    source.setTop( place.bottom() );

    return true;
}
bool DrawLine ( uint32_t& printed, uint32_t& num, QRect& source, QFont font, DrawLineHandler do_draw, double space, int custom_height )
{
    bool res = false;
    if ( printed > num )
        res = true;
    else if ( printed < num )
        res = false;
    else
    {
        QFontMetrics metrix( font );
        int height = custom_height ? custom_height : metrix.height() * space;

        QRect place;
        if ( AllocatePlace( place, height, source ) )
        {
            do_draw( place );
            ++printed;
            res = true;
        }
        else
            res = false;
    }
    ++num;
    return res;
}
bool Test::DrawLine ( uint32_t& num, QRect& source, QFont font, DrawLineHandler do_draw, double space, int custom_height ) const
{
    return test::DrawLine( mDrawLine, num, source, font, do_draw, space, custom_height );
}


DrawHelper::DrawHelper( QPainter& p, QRect& r):
    painter(p),
    free_rect(r)
{

}
void DrawHelper::DrawRowCenter( QRect const& place, QFont const& font, QColor const& color, QString const& text )
{
    painter.save();
    QFontMetrics metrix( font );
    QPoint start_point( place.center().x() - metrix.width( text ) / 2, place.center().y() +metrix.height()/2);
    painter.setFont( font );
    painter.setPen( color );
    painter.drawText( start_point, text );
    painter.restore();
}
void DrawHelper::DrawRowLeft( QRect const& place,
                  QFont const& font,
                  QColor const& color1,
                  QString const& label,
                  QColor const& color2,
                  QString const& value )
{
    painter.save();
    QFontMetrics metrix( font );
    QPoint start_point( place.left() , place.center().y()+metrix.height()/2 );
    QPoint start_point2( place.left() + metrix.width(label), place.center().y() +metrix.height()/2);
    painter.setFont( font );
    painter.setPen( color1 );
    painter.drawText( start_point, label );
    painter.setPen( color2 );
    painter.drawText( start_point2, value );
    painter.restore();
}

}//namespace test
