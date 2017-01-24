#include "main_tests.h"
#include <QJsonObject>
#include <QJsonArray>
#include <GraphBuilder/graph_builder.h>
#include "../test_params.h"
#include <QTextDocument>
#include <QtGui/QAbstractTextDocumentLayout>
#include "work_params.h"
#include "defect_question.h"
#ifdef DEMO
#include "../../settings/settings.h"
#endif
namespace test
{

namespace
{

QJsonArray ToJson( M2_2006::DataSet const& data )
{
    QJsonArray arr;
    foreach ( M2_2006::Point const& d, data )
    {
        arr.insert( arr.end(), d.Serialise() );
    }
    return std::move( arr );
}
M2_2006::DataSet FromJson( QJsonArray const& arr )
{
    M2_2006::DataSet data;

    foreach (QJsonValue const& v, arr)
    {
        M2_2006::Point d;
        if ( d.Deserialize( v.toObject() ) )
            data.insert( data.end(), d );
    }
    return std::move( data );
}

ff0x::NoAxisGraphBuilder::LinePoints Process( M2_2006::DataSet const& src, QPointF& x_range, QPointF& y_range )
{
    ff0x::NoAxisGraphBuilder::LinePoints result;

    for ( int i = 0; i < src.size(); ++i )
    {
        double const& y = src[i].Y();
        double const& x = src[i].X();
        if ( !i )
        {
            x_range.setX( x );
            x_range.setY( x );
            y_range.setX( y );
            y_range.setY( y );
        }
        else
        {
            if ( x > x_range.x() )
                x_range.setX( x );
            if ( x < x_range.y() )
                x_range.setY( x );

            if ( y > y_range.x() )
                y_range.setX( y );
            if ( y < y_range.y() )
                y_range.setY( y );
        }

        result.push_back( QPointF( x, y ) );
    }
//    if ( 0 > x_range.x() )
//        x_range.setX( 0 );
//    if ( 0 < x_range.y() )
//        x_range.setY( 0 );

//    if ( 0 > y_range.x() )
//        y_range.setX( 0 );
//    if ( 0 < y_range.y() )
//        y_range.setY( 0 );


    return std::move( result );
}
}//namespace

M2_2006::Point::Point():
    mX(0),mY(0)
{
}
M2_2006::Point::Point( double x, double y ):
    mX(x),
    mY(y)
{}
double const& M2_2006::Point::X() const
{
    return mX;
}
double const& M2_2006::Point::Y() const
{
    return mY;
}

QJsonObject M2_2006::Point::Serialise() const
{
    QJsonObject obj;
    obj.insert("x", mX);
    obj.insert("y", mY);
    return std::move( obj );
}
bool M2_2006::Point::Deserialize( QJsonObject const& obj )
{
    mX = obj.value("x").toDouble();
    mY = obj.value("y").toDouble();
    return true;
}

class M2_2006::GrapfData
{
public:
    GrapfData( M2_2006 const* test, QString compare_width )
    {
        QPointF xa_range_e;
        QPointF ya_range_e;

        QPointF xb_range_e;
        QPointF yb_range_e;

        //поиск данных теста
        bool use_etalone = false;
        foreach (QJsonValue const& val, test::ReadFromFile(compare_width).value("Results").toArray())
        {
            auto obj = val.toObject();
            if ( obj.value("id").toInt() == test->mId )
            {
                M2_2006::DataSet data = FromJson( obj.value("data").toObject().value("PData").toArray() );
                dataA_e2 = Process( data, xa_range_e, ya_range_e );
                data = FromJson( obj.value("data").toObject().value("VData").toArray() );
                dataB_e2 = Process( data, xb_range_e, yb_range_e );
                use_etalone = true;

                xa_range_e = ff0x::MergeRanges( xa_range_e, xb_range_e, true );
            }
        }

        dataA = Process( test->mPData, x_range, y_range );
        QPointF xb_range;
        dataB = Process( test->mVData, xb_range, yb_range );

        x_range = ff0x::MergeRanges( x_range, xb_range, true );
        yb_range = ff0x::MergeRanges( yb_range, yb_range_e, use_etalone );

        x_range = ff0x::MergeRanges( x_range, xa_range_e, use_etalone );
        y_range = ff0x::MergeRanges( y_range, ya_range_e, use_etalone );
    }

    void SetXscale( double scale )
    {
        if (current_x_scale == scale)
            return;

        x_range.setX( x_range.x()/current_x_scale*scale );
        x_range.setY( x_range.y()/current_x_scale*scale );

        for ( auto it = dataA.begin(), end = dataA.end(); it != end; ++it )
        {
            QPointF& point = *it;
            point.setX( point.x()/current_x_scale*scale );
        }
        for ( auto it = dataA_e2.begin(), end = dataA_e2.end(); it != end; ++it )
        {
            QPointF& point = *it;
            point.setX( point.x()/current_x_scale*scale );
        }
        for ( auto it = dataB.begin(), end = dataB.end(); it != end; ++it )
        {
            QPointF& point = *it;
            point.setX( point.x()/current_x_scale*scale );
        }
        for ( auto it = dataB_e2.begin(), end = dataB_e2.end(); it != end; ++it )
        {
            QPointF& point = *it;
            point.setX( point.x()/current_x_scale*scale );
        }

        current_x_scale = scale;
    }
    void SetYscale( double scale )
    {
        if (current_y_scale == scale)
            return;

        y_range.setX( y_range.x()/current_y_scale*scale );
        y_range.setY( y_range.y()/current_y_scale*scale );

        for ( auto it = dataA.begin(), end = dataA.end(); it != end; ++it )
        {
            QPointF& point = *it;
            point.setY( point.y()/current_y_scale*scale );
        }
        for ( auto it = dataA_e2.begin(), end = dataA_e2.end(); it != end; ++it )
        {
            QPointF& point = *it;
            point.setY( point.y()/current_y_scale*scale );
        }
//        for ( auto it = dataB.begin(), end = dataB.end(); it != end; ++it )
//        {
//            QPointF& point = *it;
//            point.setY( point.y()/current_y_scale*scale );
//        }
//        for ( auto it = dataB_e2.begin(), end = dataB_e2.end(); it != end; ++it )
//        {
//            QPointF& point = *it;
//            point.setY( point.y()/current_y_scale*scale );
//        }

        current_y_scale = scale;
    }

    ff0x::GraphBuilder::LinePoints dataA;
    ff0x::GraphBuilder::LinePoints dataA_e2;

    ff0x::GraphBuilder::LinePoints dataB;
    ff0x::GraphBuilder::LinePoints dataB_e2;

    QPointF x_range;
    QPointF y_range;    
    QPointF yb_range;

private:
    double current_x_scale = 1.0;
    double current_y_scale = 1.0;
};

class M2_2006::ZGrapfData
{
public:
    ZGrapfData( M2_2006 const* test, QString compare_width )
    {
        QPointF xa_range_e;
        QPointF ya_range_e;

        //поиск данных теста
        bool use_etalone = false;
        foreach (QJsonValue const& val, test::ReadFromFile(compare_width).value("Results").toArray())
        {
            auto obj = val.toObject();
            if ( obj.value("id").toInt() == test->mId )
            {
                M2_2006::DataSet data = FromJson( obj.value("data").toObject().value("ZData").toArray() );
                data_e2 = Process( data, xa_range_e, ya_range_e );
                use_etalone = true;
            }
        }

        data = Process( test->mZData, x_range, y_range );

        x_range = ff0x::MergeRanges( x_range, xa_range_e, use_etalone );
        y_range = ff0x::MergeRanges( y_range, ya_range_e, use_etalone );
    }

    void SetXscale( double scale )
    {
        if (current_x_scale == scale)
            return;

        x_range.setX( x_range.x()/current_x_scale*scale );
        x_range.setY( x_range.y()/current_x_scale*scale );

        for ( auto it = data.begin(), end = data.end(); it != end; ++it )
        {
            QPointF& point = *it;
            point.setX( point.x()/current_x_scale*scale );
        }
        for ( auto it = data_e2.begin(), end = data_e2.end(); it != end; ++it )
        {
            QPointF& point = *it;
            point.setX( point.x()/current_x_scale*scale );
        }

        current_x_scale = scale;
    }

    ff0x::GraphBuilder::LinePoints data;
    ff0x::GraphBuilder::LinePoints data_e2;

    QPointF x_range;
    QPointF y_range;

private:
    double current_x_scale = 1.0;
};

M2_2006::M2_2006( QString method_name, int32_t id):
    TestCommonData( method_name, id ),
    mBreakPressure(0.0),
    mState()
{
#ifdef DEMO
    auto& params = WorkParams::Instance();
    params.Date( QDateTime::currentDateTime() );
    params.Size( "265/75 R15" );
    params.Model( "PS-400" );
    params.Customer( "ОАО 'Роги и лапти'" );
    params.OrderNo("10");
    params.TireNo("4416 Я45545");
    params.Pressure("56.7");
    params.BreakPressure(true);
    params.ConstPressureTime("90");
    params.User( app::Settings::Instance().User() );

    for ( auto x = 0; x <100; ++x )
    {
        mPData.append(Point( x, (10+x*x/50) ));
        mVData.append(Point( x, (5+x*x/30) ));
    }

#endif
}
M2_2006::~M2_2006()
{}

bool M2_2006::Run()
{
    {
        std::unique_lock<std::mutex> l( mMutex );
        mSuccess = false;
        mPData.clear();
        mVData.clear();
        mZData.clear();
        mGrapfs.reset();
        mLastTime = cpu::CpuMemory::Instance().Sensors.Time();
        mBreakPressure = 0;
        mState = "";
    }

    Start();
    if ( IsStopped() )
        return false;

    std::mutex mutex;
    std::unique_lock< std::mutex > lock( mutex );
    Launcher( std::bind( &M2_2006::Question, this ) );

    mCondVar.wait( lock );

    return Success();
}
bool M2_2006::Success() const
{
    return mSuccess;
}
void M2_2006::UpdateData()
{
    auto& mem = cpu::CpuMemory::Instance().Sensors;
    if ( mLastTime != static_cast<int32_t>( mem.Time() ) )
    {
        std::unique_lock<std::mutex> l( mMutex );
        mLastTime = static_cast<int32_t>( mem.Time() );
        if (mBreakPressure < mem.Pressure())
            mBreakPressure = mem.Pressure();

        if (!mIsPrepare)
        {
            mPData.push_back( Point( mLastTime, mem.Pressure() ) );
            mVData.push_back( Point( mLastTime, mem.Volume() ) );
        }
        else
        {
            mZData.push_back( Point( mLastTime, mem.Volume() ) );
        }
    }
}

QJsonObject M2_2006::Serialise() const
{
    std::unique_lock<std::mutex> l( mMutex );
    QJsonObject obj = TestCommonData::Serialise();
    obj.insert("PData", ToJson( mPData ) );
    obj.insert("VData", ToJson( mVData ) );
    obj.insert("ZData", ToJson( mZData ) );
    obj.insert("BreakPressure", mBreakPressure);
    obj.insert("State", mState);

    return obj;
}
bool M2_2006::Deserialize( QJsonObject const& obj )
{
    std::unique_lock<std::mutex> l( mMutex );
    mPData = FromJson( obj.value("PData").toArray() );
    mVData = FromJson( obj.value("VData").toArray() );
    mZData = FromJson( obj.value("ZData").toArray() );
    mBreakPressure = obj.value("BreakPressure").toDouble();
    mState = obj.value("State").toString();

    TestCommonData::Deserialize( obj );
    return true;
}

bool M2_2006::Draw( QPainter& painter, QRect &free_rect, QString  const& compare_width ) const
{
    bool res = false;
    uint32_t num = 0;

    res = DrawHeader(num, painter,free_rect);
    res = DrawBody(num, painter,free_rect);
    res = DrawGraph(num, painter,free_rect, compare_width);
    res = DrawFoter(num, painter,free_rect);
    res = DrawZGraph(num, painter,free_rect, compare_width);
    if (res )
        free_rect.setHeight( 0 );
    return res;
}

bool M2_2006::DrawHeader( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont header_font = painter.font();
    header_font.setFamily("Arial");
    header_font.setPointSize( 14 );
    header_font.setBold(true);
    auto& params = WorkParams::Instance();

    DrawHelper drw( painter, free_rect );

    bool res = DrawLine( num, free_rect, header_font,
    [ this, &painter, &drw, &header_font ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, header_font, Qt::black, "ОБЩЕСТВО С ОГРАНИЧЕННОЙ ОТВЕТСТВЕННОСТЬЮ");
    }, 1.5 );
    res = DrawLine( num, free_rect, header_font,
    [ this, &painter, &drw, &header_font ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, header_font, Qt::black, "ШИННЫЙ ИСПЫТАТЕЛЬНЫЙ ЦЕНТР «ВЕРШИНА»");
    }, 1.5 );
    res = DrawLine( num, free_rect, header_font,
    []( QRect const& )
    {
    }, 1.5 );
    res = DrawLine( num, free_rect, header_font,
    [ this, &painter, &drw, &header_font, &params ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, header_font, Qt::black, "Протокол No _____ от " + params.Date().toString("dd.MM.yyyy") );
    }, 1.5 );
    res = DrawLine( num, free_rect, header_font,
    [ this, &painter, &drw, &header_font, &params ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, header_font, Qt::black, "Испытание шины " + params.Size() + " " + params.Model() );
    }, 1.5 );
    res = DrawLine( num, free_rect, header_font,
    [ this, &painter, &drw, &header_font, &params ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, header_font, Qt::black, "Заказчик " + params.Customer() );
    }, 1.5 );
    res = DrawLine( num, free_rect, header_font,
    [ this, &painter, &drw, &header_font, &params ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, header_font, Qt::black, "Заказ №" + ToString( params.OrderNo() ) );
    }, 1.5 );

    return res;
}
bool M2_2006::DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );

    auto& params = WorkParams::Instance();

    bool res = true;
    {
        QString header = "<html>"
                "<head>"
                  "<meta charset='utf-8'>"
                  "<style type='text/css'>"
                       "td { text-align: center;}"
                       "th { font-weight: normal;}"
                       "table {border-collapse: collapse; border-style: solid;}"
                 "</style>"
                "</head>"
                "<body>"
                "<table valign='middle' width='100%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                  "<th rowspan=\"4\">Дата<br>изготовления,<br>порядковый<br>номер изделия</th>"
                  "<th colspan=\"3\">"+TableTitle()+"</th>"
                  "<th rowspan=\"2\">Испытательное<br>оборудование</th>"
                "</tr>"
                "<tr>"
                  "<td colspan=\"3\">"+Name()+"</td>"
                "</tr>"
                "<tr>"
                  "<td colspan=\"2\">Разрушающее давление,<br>МПа (кгс/см2)</td>"
                  "<td rowspan=\"2\">Состояние шины после<br>испытания<br><br>(вид дефекта)</td>"
                  "<td rowspan=\"3\">Стенд СИШ-25</td>"
                "</tr>"
                "<tr>"
                  "<td>Факт</td>"
                  "<td>Норматив</td>"
                "</tr>";

        QString footer = "</table>"
                "</body>"
                "</html>";

        QString table = header;
        auto MakeRow = [ this, &params ]() -> QString
        {
            QString row =   "<tr>";
            row +=              "<td>"+params.TireNo()+"</td>"
                                "<td>"+ToString(mBreakPressure)+"</td>"
                                "<td>"+ToString(params.Pressure())+"</td>"
                                "<td>"+mState+"</td>";
            row +=          "</tr>";
            return row;
        };

        QTextDocument doc;
        doc.setUndoRedoEnabled( false );
        doc.setTextWidth( free_rect.width() );
        doc.setUseDesignMetrics( true );
        doc.setDefaultTextOption ( QTextOption (Qt::AlignHCenter )  );
        table += MakeRow();
        table += footer;

        doc.setHtml( table );
        auto h = doc.documentLayout()->documentSize().height();

        res = DrawLine( num, free_rect, text_font,
        [ this, &painter, &doc, &text_font ]( QRect const& rect )
        {
            painter.save();
            QRectF r( 0, 0, rect.width(), rect.height() );
            painter.translate( rect.topLeft() );
            doc.drawContents( &painter, r);
            painter.restore();
        }, 1, h );
        res = DrawLine( num, free_rect, text_font,
        []( QRect const& )
        {
        }, 3 );
    }

    return res;
}
bool M2_2006::DrawFoter( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont header_font = painter.font();
    header_font.setFamily("Arial");
    QFont text_font = header_font;
    text_font.setPointSize( 12 );
    text_font.setUnderline(true);
    QFont underline_font = text_font;
    underline_font.setPointSize( 6 );
    underline_font.setUnderline(false);

    auto& params = WorkParams::Instance();
    DrawHelper drw( painter, free_rect );

    bool res = DrawLine( num, free_rect, text_font,
    []( QRect const& )
    {
    }, 3 );

    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font, &params ]( QRect const& rect )
    {
        int add = 0;
        auto const& user = params.User();
        auto size = user.size();
        if ( size < 10)
        add = 10 - size;
        drw.DrawRowLeft( rect, text_font, Qt::black, "Инженер ОСИ ООО «ШИЦ«Вершина»"+QString(2,'\t')+QString(10,'_')+QString(1,'\t')+user+QString(add,'_'));
    }, 1.5 );
    res = DrawLine( num, free_rect, underline_font,
    [ this, &painter, &drw, &underline_font,&free_rect ]( QRect const& rect )
    {
        drw.DrawRowLeft( rect, underline_font, Qt::black, QString(5,'\t') + QString(15,' ') + "подпись" + QString(2,'\t') + "(расшифровка подписи)");
        free_rect.setHeight(0);
    }, 1.5 );

    return res;
}

QString M2_2006::TableTitle() const
{
    return "Определение прочности пневматических шин при разрушении внутренним давлением.";
}
bool M2_2006::DrawGraph( uint32_t& num, QPainter& painter, QRect &free_rect, QString  const& compare_width ) const
{

    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );


    DrawHelper drw( painter, free_rect );

    bool res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, text_font, Qt::black, "Процесс испытания покрышки");
    }, 1.5 );

    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &text_font, &compare_width, &free_rect ]( QRect const& rect )
    {
        PaintGraph( painter, text_font, rect, compare_width );        
    }, 1, 480  );



    return res;
}
bool M2_2006::DrawZGraph( uint32_t& num, QPainter& painter, QRect &free_rect, QString  const& compare_width ) const
{

    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );

    DrawHelper drw( painter, free_rect );

    bool res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowCenter( rect, text_font, Qt::black, "Процесс заполенния покрышки");
    }, 1.5 );


    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &text_font, &compare_width, &free_rect ]( QRect const& rect )
    {
        PaintZGraph( painter, text_font, rect, compare_width );
    }, 1, 480  );



    return res;
}
void M2_2006::PaintGraph( QPainter& painter, QFont const& font, QRect const &rect,
                                 QString  const& compare_width,
                                 double skale,
                                 M2_2006::PressureUnits pu,
                                 M2_2006::TimeUnits tu ) const
{
    std::unique_lock<std::mutex> l( mMutex );
    mGrapfs.reset( new GrapfData( this, compare_width ) );

    painter.save();

    double x_scale = 1.0/60.0;
    QString x_msg = "Время, мин.";
    double y_scale = 1/10.19716212978;
    QString y_msg = "Давление, кгс\\см2";

    if ( pu == puMPA )
    {
        y_scale = 1;
        y_msg = "Давление, МПа";
    }
    if ( tu == tuSec )
    {
        x_scale = 1.0;
        x_msg = "Время, сек.";
    }

    mGrapfs->SetXscale( x_scale );
    mGrapfs->SetYscale( y_scale );

    QFont f = font;
    f.setPointSize( 12 );
    int w = (rect.width())*skale;
    int h = (rect.height())*skale/2;

    ff0x::GraphBuilder builder ( w, h, ff0x::GraphBuilder::PlusPlus, f );
    ff0x::BasicGraphBuilder::GraphDataLine lines;
    lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->dataA, ff0x::BasicGraphBuilder::LabelInfo( "Давление", Qt::blue ) ) );
    if ( !mGrapfs->dataA_e2.empty() )
        lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->dataA_e2, ff0x::BasicGraphBuilder::LabelInfo( "Предыдущий результат: давление", Qt::gray ) ) );

    QRect p1(rect.left(), rect.top(), w, h );
    {
        QPointF x_range;
        QPointF y_range;
        QPointF xi_range(mGrapfs->x_range.x(), 0);
        QPointF yi_range(mGrapfs->y_range.x(), 0);
        double x_step = 0;
        double y_step = 0;
        ff0x::DataLength( xi_range,x_range, x_step );
        ff0x::DataLength( yi_range,y_range, y_step );

        painter.drawPixmap( p1, builder.Draw( lines, x_range.x()+x_step, y_range.x()+y_step, x_step, y_step, x_msg, y_msg, true ) );
    }

    ff0x::BasicGraphBuilder::GraphDataLine lines2;
    lines2.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->dataB, ff0x::BasicGraphBuilder::LabelInfo( "Объем, литр", Qt::red ) ) );
    if ( !mGrapfs->dataB_e2.empty() )
        lines2.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->dataB_e2, ff0x::BasicGraphBuilder::LabelInfo( "Предыдущий результат: объем", Qt::lightGray ) ) );

    QRect p2(rect.left(), rect.top()+h, w, h );
    {
        QPointF x_range;
        QPointF y_range;
        QPointF xi_range(mGrapfs->x_range.x(), 0);
        QPointF yi_range(mGrapfs->yb_range.x(), 0);
        double x_step = 0;
        double y_step = 0;
        ff0x::DataLength( xi_range,x_range, x_step );
        ff0x::DataLength( yi_range,y_range, y_step );

        painter.drawPixmap( p2, builder.Draw( lines2, x_range.x()+x_step, y_range.x()+y_step, x_step, y_step, x_msg, "Объем, литр", true ) );
    }

    painter.restore();
}
void M2_2006::PaintZGraph(QPainter& painter, QFont const& font, QRect const &rect,
                                 QString  const& compare_width,
                                 double skale,
                                 M2_2006::TimeUnits tu ) const
{
    std::unique_lock<std::mutex> l( mMutex );
    mZGrapfs.reset( new ZGrapfData( this, compare_width ) );

    painter.save();

    double x_scale = 1.0/60.0;
    QString x_msg = "Время, мин.";

    if ( tu == tuSec )
    {
        x_scale = 1.0;
        x_msg = "Время, сек.";
    }

    mZGrapfs->SetXscale( x_scale );

    QFont f = font;
    f.setPointSize( 12 );
    int w = (rect.width())*skale;
    int h = (rect.height())*skale;

    ff0x::GraphBuilder builder ( w, h, ff0x::GraphBuilder::PlusPlus, f );
    ff0x::BasicGraphBuilder::GraphDataLine lines;
    lines.push_back( ff0x::BasicGraphBuilder::Line(mZGrapfs->data, ff0x::BasicGraphBuilder::LabelInfo( "Объем, литр", Qt::darkGreen ) ) );
    if ( !mZGrapfs->data_e2.empty() )
        lines.push_back( ff0x::BasicGraphBuilder::Line(mZGrapfs->data_e2, ff0x::BasicGraphBuilder::LabelInfo( "Предыдущий результат: объем", Qt::gray ) ) );

    QRect p1(rect.left(), rect.top(), w, h );
    {
        QPointF x_range;
        QPointF y_range;
        QPointF xi_range(mZGrapfs->x_range.x(), 0);
        QPointF yi_range(mZGrapfs->y_range.x(), 0);
        double x_step = 0;
        double y_step = 0;
        ff0x::DataLength( xi_range,x_range, x_step );
        ff0x::DataLength( yi_range,y_range, y_step );

        painter.drawPixmap( p1, builder.Draw( lines, x_range.x()+x_step, y_range.x()+y_step, x_step, y_step, x_msg, "Объем, литр", true ) );
    }

    painter.restore();
}

void M2_2006::Question()
{
    DefectQuestion msg;
    msg.setModal( true );
    msg.exec();
    mState = msg.Status();
    mCondVar.notify_one();
}


EK_OON_106::EK_OON_106():
    M2_2006( "Правило ЕЭК ООН №106", 2 ),
    mConstPressureTime( 0 )
{}
void EK_OON_106::UpdateData()
{
    M2_2006::UpdateData();
}
QJsonObject EK_OON_106::Serialise() const
{
    auto obj = M2_2006::Serialise();
    obj.insert("ConstPressureTime",mConstPressureTime);
    return obj;
}
bool EK_OON_106::Deserialize( QJsonObject const& obj )
{
    mConstPressureTime = obj.value("ConstPressureTime").toInt();
    return M2_2006::Deserialize( obj );
}

bool EK_OON_106::DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );

    auto& params = WorkParams::Instance();

    bool res = true;
    {
        QString header = "<html>"
                "<head>"
                  "<meta charset='utf-8'>"
                  "<style type='text/css'>"
                       "td { text-align: center;}"
                       "th { font-weight: normal;}"
                       "table {border-collapse: collapse; border-style: solid;}"
                 "</style>"
                "</head>"
                "<body>"
                "<table valign='middle' width='100%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                  "<th rowspan=\"4\">Дата<br>изготовления,<br>порядковый<br>номер изделия</th>"
                  "<th colspan=\"5\">"+TableTitle()+"</th>"
                  "<th rowspan=\"2\">Испытательное<br>оборудование</th>"
                "</tr>"
                "<tr>"
                  "<td colspan=\"5\">"+Name()+"</td>"
                "</tr>"
                "<tr>"
                  "<td colspan=\"2\">Разрушающее<br>давление,<br>МПа (кгс/см2)</td>"
                  "<td colspan=\"2\">Поддержание<br>давления на<br>неизменном уровне<br>в шине, мин</td>"
                  "<td rowspan=\"2\">Состояние шины после<br>испытания<br><br>(вид дефекта)</td>"
                  "<td rowspan=\"3\">Стенд СИШ-25</td>"
                "</tr>"
                "<tr>"
                  "<td>Факт</td>"
                  "<td>Норматив</td>"
                  "<td>Факт</td>"
                  "<td>Норматив</td>"
                "</tr>";

        QString footer = "</table>"
                "</body>"
                "</html>";

        QString table = header;
        auto MakeRow = [ this, &params ]() -> QString
        {
            QString row =   "<tr>";
            row +=              "<td>"+params.TireNo()+"</td>"
                                "<td>"+ToString(mBreakPressure)+"</td>"
                                "<td>"+ToString(params.Pressure())+"</td>"
                                "<td>"+ToString(mConstPressureTime)+"</td>"
                                "<td>"+ToString(params.ConstPressureTime())+"</td>"
                                "<td>"+mState+"</td>";
            row +=          "</tr>";
            return row;
        };

        QTextDocument doc;
        doc.setUndoRedoEnabled( false );
        doc.setTextWidth( free_rect.width() );
        doc.setUseDesignMetrics( true );
        doc.setDefaultTextOption ( QTextOption (Qt::AlignHCenter )  );
        table += MakeRow();
        table += footer;

        doc.setHtml( table );
        auto h = doc.documentLayout()->documentSize().height();

        res = DrawLine( num, free_rect, text_font,
        [ this, &painter, &doc, &text_font ]( QRect const& rect )
        {
            painter.save();
            QRectF r( 0, 0, rect.width(), rect.height() );
            painter.translate( rect.topLeft() );
            doc.drawContents( &painter, r);
            painter.restore();
        }, 1, h );
        res = DrawLine( num, free_rect, text_font,
        []( QRect const& )
        {
        }, 3 );
    }

    return res;
}
QString EK_OON_106::TableTitle() const
{
    return "Оценка устойчивости шин к разрыву";
}


M24_82::M24_82():
    M2_2006( "Методика № М 24-82", 3 )
{

}
QString M24_82::TableTitle() const
{
    return "Определение прочности тормозной камеры внутренним гидравлическим давлением.";
}


MI5C_2006::MI5C_2006():
    M2_2006( "Методика № МИ5С-2006", 4 )
{

}
QString MI5C_2006::TableTitle() const
{
    return "Определение прочности авиационных шин внутренним гидравлическим давлением.";
}
}
