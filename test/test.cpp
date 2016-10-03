#include "test.h"

namespace test
{

Test::Test(QString const& name, uint8_t id)
    : mName(name), mId(id), mStopMarker(nullptr),
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
bool Test::Run(LaunchFunction to_launch, LogFunction to_log, bool& stop )
{
    Log = to_log;
    Launcher = to_launch;
    mStopMarker = &stop;

    bool res = Run();

    Log = LogFunction();
    Launcher = LaunchFunction();
    mStopMarker = nullptr;

    return res;
}

bool Test::IsStopped()
{
    return  mStopMarker && *mStopMarker;
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
bool Test::DrawLine ( uint32_t& num, QRect& source, QFont font, DrawLineHandler do_draw, double space, int custom_height ) const
{
    bool res = false;
    if ( mDrawLine > num )
        res = true;
    else if ( mDrawLine < num )
        res = false;
    else
    {
        QFontMetrics metrix( font );
        int height = custom_height ? custom_height : metrix.height() * space;

        QRect place;
        if ( AllocatePlace( place, height, source ) )
        {
            do_draw( place );
            ++mDrawLine;
            res = true;
        }
        else
            res = false;
    }
    ++num;
    return res;
}

}//namespace test
