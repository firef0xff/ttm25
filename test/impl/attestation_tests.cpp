#include "attestation_tests.h"
#include "attestation_params.h"
#include <thread>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <GraphBuilder/graph_builder.h>
#include <cpu/metran/metran.h>

namespace test
{

Attestaion::Attestaion ( QString const& name, uint8_t id ):
    TestCommonData(name, id),
    mSuccess(false),
    mControls(cpu::CpuMemory::Instance().AttestationLaunchControls)
{}

void Attestaion::Start()
{
    mControls.Reset();
    mControls.Write();
    StartTime.start();

    while( !StopBit() )
    {
        mControls.Read();
        UpdateData();
        if ( IsStopped() || !CheckErrors() )
        {
            SetStopBit( true );
            Log( "Испытание прервано" );
            return;
        }

        if ( mPrepareMarker && *mPrepareMarker )
        {
            *mPrepareMarker = false;
            SetStartBit( true );
        }

        if ( mRunMarker && *mRunMarker )
        {//кнопка записать
            *mRunMarker = false;
        }

        if ( mPauseMarker && *mPauseMarker )
        {//пока не используется
            *mPauseMarker = false;
        }

        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
    }
    TestingTime = StartTime.elapsed()/1000;
    return;
}
bool Attestaion::Success() const
{
    return mSuccess;
}

QJsonObject Attestaion::Serialise() const
{
    QJsonObject obj;
    obj.insert("Success",mSuccess);
    return obj;
}
bool Attestaion::Deserialize( QJsonObject const& obj )
{
    mSuccess = obj.value("Success").toBool();
    return true;
}

bool Attestaion::Draw( QPainter& painter, QRect &free_rect, QString  const& /*compare_width*/ ) const
{
    bool res = false;
    uint32_t num = 0;

    res = DrawHeader(num, painter,free_rect);
    res = DrawGraph( num, painter, free_rect );
    res = DrawBody(num, painter,free_rect);
//    res = DrawFoter(num, painter,free_rect);

    if (res )
        free_rect.setHeight( 0 );
    return res;
}

bool Attestaion::DrawHeader( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont header_font = painter.font();
    header_font.setFamily("Arial");
    header_font.setPointSize( 14 );
    header_font.setBold(true);
    auto& params = AttestationParams::Instance();

    DrawHelper drw( painter, free_rect );

    auto rows = Name().split('\n');

    bool res = false;
    for ( auto it = rows.begin(), end = rows.end(); it != end; ++it )
    {
        QString str = *it;
        if ( it == rows.begin() )
            str = QString::number( ID() + 1 ) + ". " + str;
        res = DrawLine( num, free_rect, header_font,
        [ this, &painter, &drw, &header_font, &params, &str ]( QRect const& rect )
        {
            drw.DrawRowCenter( rect, header_font, Qt::black, str );
        }, 1.5 );
    }
    res = DrawLine( num, free_rect, header_font,
    []( QRect const& )
    {
    }, 1.5 );
    return res;
}
bool Attestaion::DrawFoter( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont header_font = painter.font();
    header_font.setFamily("Arial");
    QFont text_font = header_font;
    text_font.setPointSize( 12 );
    text_font.setUnderline(true);
    QFont underline_font = text_font;
    underline_font.setPointSize( 6 );
    underline_font.setUnderline(false);

    auto& params = AttestationParams::Instance();
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

bool Attestaion::DrawGraph( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{

    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );


    DrawHelper drw( painter, free_rect );

    bool res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &text_font, &free_rect ]( QRect const& rect )
    {
        PaintGraph( painter, text_font, rect );
    }, 1, 480  );

    return res;
}

void Attestaion::ResetDrawLine()
{
    PrintedRows = 0;
    PrintedPage = 0;
    Test::ResetDrawLine();
}

void Attestaion::PaintGraph(QPainter&, QFont const&, const QRect&, double ) const
{}
//------------------------------------------------------~

namespace
{
QJsonArray ToJson( AttPressure::DataSet const& data )
{
    QJsonArray arr;
    foreach ( AttPressure::Data const& d, data )
    {
        arr.insert( arr.end(), d.Serialise() );
    }
    return std::move( arr );
}
void FromJson( AttPressure::DataSet& data, QJsonArray const& arr )
{
    data.clear();
    foreach (QJsonValue const& v, arr)
    {
        AttPressure::Data d;
        if ( d.Deserialize( v.toObject() ) )
            data.insert( data.end(), d );
    }
}
}

AttPressure::Data::Data():
    mTask(0),
    mResult(0),
    mFact(0),
    mCurrent(false)
{}

QJsonObject AttPressure::Data::Serialise() const
{
    QJsonObject obj;
    obj.insert("Task",  mTask);
    obj.insert("Result",mResult);
    obj.insert("Fact",  mFact);
    obj.insert("Current",  mCurrent);
    return obj;
}
bool AttPressure::Data::Deserialize( QJsonObject const& obj )
{
    mTask   = obj.value("Task").toDouble();
    mResult = obj.value("Result").toDouble();
    mFact   = obj.value("Fact").toDouble();
    mCurrent   = obj.value("Current").toDouble();
    return true;
}

double AttPressure::Data::Error() const
{
    if ( !mFact )
        return 0.0;

    auto delta = abs(mFact - mResult);
    return round(delta/mFact*10000)/100; //вывод погрешности в %
}


AttPressure::AttPressure():
    Attestaion("Определение относительной погрешности достижения\nзаданного внутреннего давления в шине", 1)
{
    Reset();
}

bool AttPressure::Run()
{
    Start();
    cpu::Metran::Instance().Disconnect();
    if ( IsStopped() )
        return false;

    return Success();
}
void AttPressure::UpdateData()
{
    auto const& mem = cpu::CpuMemory::Instance().Sensors;
    if (mCurrenPos<mData.size())
    {
        auto& dt = mData[mCurrenPos];
        dt.mCurrent = true;
        dt.mResult = mem.Pressure();
        if (mControls.AttPressureSave() )
        {
            for ( auto it = mData.begin(), end = mData.end(); it != end; ++it )
            {
                it->mCurrent = false;
            }
            //в этой точке опросить метран
            try
            {
                auto& mt = cpu::Metran::Instance();
                mt.Read();
                dt.mFact = mt.Pressure();
            }
            catch (std::exception const& e)
            {
                QString err = QString("Испытание прервано: ") + e.what();
                Log( err  );
                SetStopBit( true );
                ShowError( err );
            }
            //продолжить тест            
            mControls.AttPressureSave(false);
            ++mCurrenPos;
        }
    }
}
void AttPressure::SetStartBit( bool b )
{
    mControls.AttPressureStart( b );
}
void AttPressure::SetStopBit( bool b )
{
    mControls.AttPressureStop( b );
}
bool AttPressure::StopBit()
{
    return mControls.AttPressureStop();
}

void AttPressure::Reset()
{
    mCurrenPos = 0;
    mData.clear();
//    for ( int i = 10; i <= 100; i += 10  )
//    {
//        Data d;
//        d.mTask = round(100*i*1/10.19716212978)/100.0;
//        if ( i == 10 )
//            d.mCurrent = true;
//        mData.push_back( std::move( d ) );
//    }
    for ( int i = 1; i <= 10; i += 1  )
    {
        Data d;
        d.mTask = i;
        if ( i == 1 )
            d.mCurrent = true;
        mData.push_back( std::move( d ) );
    }
}
AttPressure::DataSet const& AttPressure::GetData() const
{
    return mData;
}
AttPressure::DataSet& AttPressure::GetData()
{
    return mData;
}

QJsonObject AttPressure::Serialise() const
{
    QJsonObject obj = Attestaion::Serialise();
    obj.insert("CurrenPos",mCurrenPos);
    obj.insert("Data", ToJson(mData));
    return obj;
}
bool AttPressure::Deserialize( QJsonObject const& obj )
{
    FromJson( mData, obj.value("Data").toArray() );
    mCurrenPos = obj.value("CurrenPos").toInt();
    Attestaion::Deserialize( obj );
    return true;
}

bool AttPressure::DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );

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
                "<table valign='middle' width='70%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                  "<th>Заданное<br>давление, МПа</th>"
                  "<th>Достигнутое<br>значение, МПа</th>"
                  "<th>Контрольное<br>значение, МПа</th>"
                  "<th>Погрешность,<br> %</th>"
                "</tr>";

        QString footer = "</table>"
                "</body>"
                "</html>";

        QString table = header;
        auto MakeRow = [ this ]( Data const& dt) -> QString
        {
            QString row =   "<tr>";
            row +=              "<td>"+ToString(dt.mTask)+"</td>"
                                "<td>"+ToString(dt.mResult)+"</td>"
                                "<td>"+ToString(dt.mFact)+"</td>"
                                "<td>"+ToString(dt.Error())+"</td>";
            row +=          "</tr>";
            return row;
        };

        QTextDocument doc;
        doc.setUndoRedoEnabled( false );
        doc.setTextWidth( free_rect.width() );
        doc.setUseDesignMetrics( true );
        doc.setDefaultTextOption ( QTextOption (Qt::AlignHCenter )  );

        int rows_prapared = 0;
        for ( auto i = PrintedRows; i < mData.size(); ++i ) //PrintedRows
        {
            QString row = MakeRow( mData[i] );
            QString tmp = table + row + footer;
            doc.setHtml( tmp );
            auto h = doc.documentLayout()->documentSize().height();
            if ( h > free_rect.height() )
                break;
            ++rows_prapared;
            table += row;
        }
        if ( PrintedRows && rows_prapared )
            num += PrintedPage;
        else if( PrintedPage )
            num += PrintedPage - 1;
        PrintedRows += rows_prapared;
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
            ++PrintedPage;
        }, 1, h );
    }

    DrawHelper drw( painter, free_rect );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowLeft( rect, text_font, Qt::black, "Допускаемая относительная погрешность достижения заданного внутреннего давления" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowLeft( rect, text_font, Qt::black, "в шине от ВПД \u00B1 3,0%"  );
    }, 1.5 );
    return res;
}

namespace
{
QVector<ff0x::GraphBuilder::LinePoints> Process( AttPressure::DataSet const& src, QPointF& x_range, QPointF& y_range )
{
    QVector<ff0x::GraphBuilder::LinePoints> result;
    result.resize(3);

    for ( int i = 0; i < src.size(); ++i )
    {
        double const& y1 = src[i].mTask;
        double const& y2 = src[i].mResult;
        double const& y3 = src[i].mFact;

        double x = i + 1;

        double max_y = std::max( y1, std::max( y2, y3 ) );
        double min_y = std::min( y1, std::min( y2, y3 ) );
        if ( !i )
        {
            x_range.setX( x );
            x_range.setY( x );
            y_range.setX( max_y );
            y_range.setY( min_y );
        }
        else
        {
            if ( x > x_range.x() )
                x_range.setX( x );
            if ( x < x_range.y() )
                x_range.setY( x );

            if ( max_y > y_range.x() )
                y_range.setX( max_y );
            if ( min_y < y_range.y() )
                y_range.setY( min_y );
        }

        result[0].push_back( QPointF( x, y1 ) );
        result[1].push_back( QPointF( x, y2 ) );
        result[2].push_back( QPointF( x, y3 ) );
    }

    return std::move( result );
}

}

class AttPressure::GrapfData
{
public:
    GrapfData( AttPressure const* test )
    {
        data = Process( test->mData, x_range, y_range );
    }

    QVector<ff0x::GraphBuilder::LinePoints> data;


    QPointF x_range;
    QPointF y_range;
};

void AttPressure::PaintGraph(QPainter& painter, QFont const& font, const QRect &rect, double skale) const
{
    mGrapfs.reset( new GrapfData( this ) );

    painter.save();

    QString x_msg = "Номер измерения";
    QString y_msg = "Давление, МПа";


    QFont f = font;
    f.setPointSize( 12 );
    int w = (rect.width())*skale;
    int h = (rect.height())*skale;

    ff0x::GraphBuilder builder ( w, h, ff0x::GraphBuilder::PlusPlus, f );
    ff0x::BasicGraphBuilder::GraphDataLine lines;
    lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->data[0], ff0x::BasicGraphBuilder::LabelInfo( "Задание", Qt::darkRed ) ) );
    lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->data[1], ff0x::BasicGraphBuilder::LabelInfo( "Результат", Qt::darkGreen ) ) );
    lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->data[2], ff0x::BasicGraphBuilder::LabelInfo( "Контрольное значение", Qt::darkBlue ) ) );

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

    painter.restore();
}
//------------------------------------------------------~
namespace
{
QJsonArray ToJson( AttTime::DataSet const& data )
{
    QJsonArray arr;
    foreach ( AttTime::Data const& d, data )
    {
        arr.insert( arr.end(), d.Serialise() );
    }
    return std::move( arr );
}
void FromJson( AttTime::DataSet& data, QJsonArray const& arr )
{
    data.clear();

    foreach (QJsonValue const& v, arr)
    {
        AttTime::Data d;
        if ( d.Deserialize( v.toObject() ) )
            data.insert( data.end(), d );
    }
}
}

AttTime::Data::Data():
    mCpuTime(0),
    mResult(0),
    mCurrent(false)
{}

QJsonObject AttTime::Data::Serialise() const
{
    QJsonObject obj;
    obj.insert("CpuTime",  mCpuTime);
    obj.insert("Result",mResult);
    obj.insert("Current",  mCurrent);
    return obj;
}
bool AttTime::Data::Deserialize( QJsonObject const& obj )
{
    mCpuTime   = obj.value("CpuTime").toDouble();
    mResult = obj.value("Result").toDouble();
    mCurrent   = obj.value("Current").toDouble();
    return true;
}


AttTime::AttTime():
    Attestaion("Аттестация машинного времени", 0)
{
    Reset();
}
bool AttTime::Run()
{
    Start();
    if ( IsStopped() )
        return false;

    return Success();
}
void AttTime::Reset()
{
    mCurrenPos = 0;
    mData.clear();
    mWait = false;
    Data d;
    d.mCurrent = true;
    mData.push_back( std::move(d) );
}

void AttTime::UpdateData()
{
    auto const& mem = cpu::CpuMemory::Instance().Sensors;
    if (mCurrenPos<mData.size())
    {
        auto& dt = mData[mCurrenPos];
        dt.mCpuTime = round( mem.Time() *100 )/100.0;
        if ( mRunMarker && *mRunMarker )
        {
            *mRunMarker = false;
            if ( mWait )
            {
                for ( auto it = mData.begin(), end = mData.end(); it != end; ++it )
                {
                    it->mCurrent = false;
                }
                dt.mCurrent = true;
            }
            else
            {
                ++mCurrenPos;
                mData.push_back(Data());
            }
            mWait = !mWait;
        }
        if (IsStopped())
        {
            if ( mData.size() > 1 )
                mData.resize( mData.size() - 1 );
            if ( mCurrenPos )
                --mCurrenPos;
        }
    }
}

AttTime::DataSet const& AttTime::GetData() const
{
    return mData;
}
AttTime::DataSet& AttTime::GetData()
{
    return mData;
}


QJsonObject AttTime::Serialise() const
{
    QJsonObject obj = Attestaion::Serialise();
    obj.insert("CurrenPos",mCurrenPos);
    obj.insert("Data", ToJson(mData));
    obj.insert("Wait", mWait);
    return obj;
}
bool AttTime::Deserialize( QJsonObject const& obj )
{
    FromJson( mData, obj.value("Data").toArray() );
    mCurrenPos = obj.value("CurrenPos").toInt();
    mWait = obj.value("Wait").toBool();
    Attestaion::Deserialize( obj );
    return true;
}

void AttTime::SetStartBit( bool b )
{
    mControls.AttTimeStart( b );
}
void AttTime::SetStopBit( bool b )
{
    mControls.AttTimeStop( b );
}
bool AttTime::StopBit()
{
    return mControls.AttTimeStop();
}
bool AttTime::DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );

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
                "<table valign='middle' width='50%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                  "<th>Время контроллера, сек</th>"
                  "<th>Секундомер, сек</th>"
                "</tr>";

        QString footer = "</table>"
                "</body>"
                "</html>";

        QString table = header;
        auto MakeRow = [ this ]( Data const& dt) -> QString
        {
            QString row =   "<tr>";
            row +=              "<td>"+ToString(dt.mCpuTime)+"</td>"
                                "<td>"+ToString(dt.mResult)+"</td>";
            row +=          "</tr>";
            return row;
        };

        QTextDocument doc;
        doc.setUndoRedoEnabled( false );
        doc.setTextWidth( free_rect.width() );
        doc.setUseDesignMetrics( true );
        doc.setDefaultTextOption ( QTextOption (Qt::AlignHCenter )  );

        int rows_prapared = 0;
        for ( auto i = PrintedRows; i < mData.size(); ++i ) //PrintedRows
        {
            QString row = MakeRow( mData[i] );
            QString tmp = table + row + footer;
            doc.setHtml( tmp );
            auto h = doc.documentLayout()->documentSize().height();
            if ( h > free_rect.height() )
                break;
            ++rows_prapared;
            table += row;
        }
        if ( PrintedRows && rows_prapared )
            num += PrintedPage;
        else if( PrintedPage )
            num += PrintedPage - 1;
        PrintedRows += rows_prapared;
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
            ++PrintedPage;
        }, 1, h );
    }

    return res;
}


namespace
{
QVector<ff0x::GraphBuilder::LinePoints> Process( AttTime::DataSet const& src, QPointF& x_range, QPointF& y_range )
{
    QVector<ff0x::GraphBuilder::LinePoints> result;
    result.resize(2);

    for ( int i = 0; i < src.size(); ++i )
    {
        double const& y1 = src[i].mCpuTime;
        double const& y2 = src[i].mResult;

        double x = i + 1;

        double max_y = std::max( y1, y2 );
        double min_y = std::min( y1, y2 );
        if ( !i )
        {
            x_range.setX( x );
            x_range.setY( x );
            y_range.setX( max_y );
            y_range.setY( min_y );
        }
        else
        {
            if ( x > x_range.x() )
                x_range.setX( x );
            if ( x < x_range.y() )
                x_range.setY( x );

            if ( max_y > y_range.x() )
                y_range.setX( max_y );
            if ( min_y < y_range.y() )
                y_range.setY( min_y );
        }

        result[0].push_back( QPointF( x, y1 ) );
        result[1].push_back( QPointF( x, y2 ) );
    }

    return std::move( result );
}

}

class AttTime::GrapfData
{
public:
    GrapfData( AttTime const* test )
    {
        data = Process( test->mData, x_range, y_range );
    }

    QVector<ff0x::GraphBuilder::LinePoints> data;


    QPointF x_range;
    QPointF y_range;
};

void AttTime::PaintGraph(QPainter& painter, QFont const& font, const QRect &rect, double skale) const
{
    mGrapfs.reset( new GrapfData( this ) );

    painter.save();

    QString x_msg = "Номер измерения";
    QString y_msg = "Время, сек";


    QFont f = font;
    f.setPointSize( 12 );
    int w = (rect.width())*skale;
    int h = (rect.height())*skale;

    ff0x::GraphBuilder builder ( w, h, ff0x::GraphBuilder::PlusPlus, f );
    ff0x::BasicGraphBuilder::GraphDataLine lines;
    lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->data[0], ff0x::BasicGraphBuilder::LabelInfo( "Результат", Qt::darkGreen ) ) );
    lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->data[1], ff0x::BasicGraphBuilder::LabelInfo( "Контрольное значение", Qt::darkBlue ) ) );

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

    painter.restore();
}
//------------------------------------------------------~
namespace
{
QJsonArray ToJson( AttPressureSpeed::DataSet const& data )
{
    QJsonArray arr;
    foreach ( AttPressureSpeed::Data const& d, data )
    {
        arr.insert( arr.end(), d.Serialise() );
    }
    return std::move( arr );
}
void FromJson( AttPressureSpeed::DataSet& data, QJsonArray const& arr )
{
    data.clear();

    foreach (QJsonValue const& v, arr)
    {
        AttPressureSpeed::Data d;
        if ( d.Deserialize( v.toObject() ) )
            data.insert( data.end(), d );
    }
}
}

AttPressureSpeed::Data::Data():
    mCpuTime(0),
    mResult(0)
{}

QJsonObject AttPressureSpeed::Data::Serialise() const
{
    QJsonObject obj;
    obj.insert("CpuTime",  mCpuTime);
    obj.insert("Result",mResult);
    return obj;
}
bool AttPressureSpeed::Data::Deserialize( QJsonObject const& obj )
{
    mCpuTime   = obj.value("CpuTime").toDouble();
    mResult = obj.value("Result").toDouble();
    return true;
}
double AttPressureSpeed::Data::Speed( Data const& prev ) const
{
    double dP = mResult - prev.mResult;
    double dT = mCpuTime - prev.mCpuTime;

    double speed = dP/dT; //в МПа/сек;

    return round( speed * 60.0 * 100 )/100; //в МПа/мин
}


AttPressureSpeed::AttPressureSpeed():
    Attestaion("Определение средней скорости нарастания давления", 2)
{
    Reset();
}
bool AttPressureSpeed::Run()
{
    Start();
    if ( IsStopped() )
        return false;

    return Success();
}
void AttPressureSpeed::Reset()
{        
    mData.clear();
}
void AttPressureSpeed::UpdateData()
{
    auto const& mem = cpu::CpuMemory::Instance().Sensors;
    int time = mem.Time();
    int ofset = AttestationParams::Instance().UpdatePeriod();

    double key = AttestationParams::Instance().PressureSpeed();

    if ( mData[key].empty() )
        mData[key].push_back( Data() );

    auto last_time = mData[key][mData.size()-1].mCpuTime;
    if ( time - last_time > ofset )
    {
        mCurrenTime = last_time;
        return;
    }
    if ( time < mCurrenTime )
        mCurrenTime = time;
    if ( time < mCurrenTime + ofset )
        return;

    mCurrenTime = time;

    Data dt;
    dt.mCpuTime = round( mem.Time()*100)/100.0;
    dt.mResult = mem.Pressure();
    mData[key].push_back(dt);
}

QJsonObject AttPressureSpeed::Serialise() const
{
    QJsonObject obj = Attestaion::Serialise();
    obj.insert("CurrenPos",mCurrenTime);
    QJsonObject data;
    for( auto it = mData.begin(), end = mData.end(); it != end; ++it )
        data.insert( ToString(it->first), ToJson(it->second));
    obj.insert("Data", data);
    return obj;
}
bool AttPressureSpeed::Deserialize( QJsonObject const& obj )
{
    mData.clear();
    QJsonObject data = obj.value("Data").toObject();
    for ( auto it = data.begin(), end = data.end(); it != end; ++it )
    {
        double key = 0.0;
        ParseValue( key, it.key() );
        FromJson( mData[key], it.value().toArray() );
    }

    mCurrenTime = obj.value("CurrenPos").toInt();
    Attestaion::Deserialize( obj );
    return true;
}

void AttPressureSpeed::SetStartBit( bool b )
{
    mControls.AttPressureSpeedStart( b );
}
void AttPressureSpeed::SetStopBit( bool b )
{
    mControls.AttPressureSpeedStop( b );
}
bool AttPressureSpeed::StopBit()
{
    return mControls.AttPressureSpeedStop();
}
AttPressureSpeed::DataSet const& AttPressureSpeed::GetData(double key) const
{
    return mData[ key ];
}
AttPressureSpeed::DataSet& AttPressureSpeed::GetData( double key )
{
    return mData[ key ];
}

bool AttPressureSpeed::DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );

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
                "<table valign='middle' width='50%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                  "<th>Время, сек</th>"
                  "<th>Давление, МПа</th>"
                  "<th>Скорость, МПа/мин</th>"
                "</tr>";

        QString footer = "</table>"
                "</body>"
                "</html>";

        QString table = header;
        auto MakeRow = [ this ]( DataSet const& data, int i ) -> QString
        {
            auto dt = data[i];
            double speed = i == 0 ? 0 : dt.Speed(data[i-1]);
            QString row =   "<tr>";
            row +=              "<td>"+ToString(dt.mCpuTime)+"</td>"
                                "<td>"+ToString(dt.mResult)+"</td>"
                                "<td>"+ToString(speed)+"</td>";
            row +=          "</tr>";
            return row;
        };

        QTextDocument doc;
        doc.setUndoRedoEnabled( false );
        doc.setTextWidth( free_rect.width() );
        doc.setUseDesignMetrics( true );
        doc.setDefaultTextOption ( QTextOption (Qt::AlignHCenter )  );

        int rows_prapared = 0;        
        DataSet all_data;
        for ( auto it = mData.begin(), end = mData.end(); it != end; ++it )
        {
            all_data += it->second;
        }

        for ( auto i = PrintedRows; i < all_data.size(); ++i ) //PrintedRows
        {
            QString row = MakeRow( all_data, i );
            QString tmp = table + row + footer;
            doc.setHtml( tmp );
            auto h = doc.documentLayout()->documentSize().height();
            if ( h > free_rect.height() )
                break;
            ++rows_prapared;
            table += row;
        }
        if ( PrintedRows && rows_prapared )
            num += PrintedPage;
        else if( PrintedPage )
            num += PrintedPage - 1;
        PrintedRows += rows_prapared;
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
            ++PrintedPage;
        }, 1, h );
    }

    DrawHelper drw( painter, free_rect );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowLeft( rect, text_font, Qt::black, "Скорости нарастания давления в шине:" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowLeft( rect, text_font, Qt::black, "- в режиме 1 не более 0.2 МПа/мин"  );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowLeft( rect, text_font, Qt::black, "- в режиме 2 не более 0.5 МПа/мин"  );
    }, 1.5 );
    return res;
}

namespace
{
QVector<ff0x::GraphBuilder::LinePoints> Process( AttPressureSpeed::DataSet const& src, QPointF& x_range, QPointF& y_range )
{
    QVector<ff0x::GraphBuilder::LinePoints> result;
    result.resize(2);

    double speed = AttestationParams::Instance().PressureSpeed();
    for ( int i = 0; i < src.size(); ++i )
    {
        double x = src[i].mCpuTime;

        double const& y1 = src[i].mResult;
        double const& y2 = x * speed/60.0;

        double max_y = std::max( y1, y2 );
        double min_y = std::min( y1, y2 );
        if ( !i )
        {
            x_range.setX( x );
            x_range.setY( x );
            y_range.setX( max_y );
            y_range.setY( min_y );
        }
        else
        {
            if ( x > x_range.x() )
                x_range.setX( x );
            if ( x < x_range.y() )
                x_range.setY( x );

            if ( max_y > y_range.x() )
                y_range.setX( max_y );
            if ( min_y < y_range.y() )
                y_range.setY( min_y );
        }

        result[0].push_back( QPointF( x, y1 ) );
        result[1].push_back( QPointF( x, y2 ) );
    }

    return std::move( result );
}

}

class AttPressureSpeed::GrapfData
{
public:
    GrapfData( AttPressureSpeed const* test )
    {
        for ( auto it = test->mData.begin(), end = test->mData.end(); it != end; ++it )
        {
            QPointF x_r, y_r;
            data.push_back( Process( it->second, x_r, y_r ) );
            ff0x::MergeRanges( x_range, x_r, true );
            ff0x::MergeRanges( y_range, y_r, true );
        }
    }

    typedef QVector<ff0x::GraphBuilder::LinePoints> Data;
    typedef std::vector< Data > DataSet;
    DataSet data;


    QPointF x_range;
    QPointF y_range;
};

void AttPressureSpeed::PaintGraph(QPainter& painter, QFont const& font, const QRect &rect, double skale) const
{
    mGrapfs.reset( new GrapfData( this ) );

    painter.save();

    QString x_msg = "Время, сек";
    QString y_msg = "Давление, МПа";


    QFont f = font;
    f.setPointSize( 12 );
    int w = (rect.width())*skale;
    int h = (rect.height())*skale;

    ff0x::GraphBuilder builder ( w, h, ff0x::GraphBuilder::PlusPlus, f );
    ff0x::BasicGraphBuilder::GraphDataLine lines;
    for ( auto it = mGrapfs->data.begin(), end = mGrapfs->data.end(); it != end; ++it  )
    {
        lines.push_back( ff0x::BasicGraphBuilder::Line( (*it)[0], ff0x::BasicGraphBuilder::LabelInfo( "Результат", Qt::darkBlue ) ) );
        lines.push_back( ff0x::BasicGraphBuilder::Line( (*it)[1], ff0x::BasicGraphBuilder::LabelInfo( "Контрольное значение", Qt::darkRed ) ) );
    }

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

    painter.restore();
}
//------------------------------------------------------~
namespace
{
QJsonArray ToJson( AttPressureTime::DataSet const& data )
{
    QJsonArray arr;
    foreach ( AttPressureTime::Data const& d, data )
    {
        arr.insert( arr.end(), d.Serialise() );
    }
    return std::move( arr );
}
void FromJson( AttPressureTime::DataSet& data, QJsonArray const& arr )
{
    data.clear();

    foreach (QJsonValue const& v, arr)
    {
        AttPressureTime::Data d;
        if ( d.Deserialize( v.toObject() ) )
            data.insert( data.end(), d );
    }
}

QJsonObject ToJson( AttPressureTime::StepDataSet const& data )
{
    QJsonObject obj;
    for( auto it = data.begin(), end = data.end(); it != end; ++it )
    {
        auto const& item = *it;
        obj.insert( QString::number( item.first ), item.second );
    }
    return std::move( obj );
}
void FromJson( AttPressureTime::StepDataSet& data, QJsonObject const& obj )
{
    data.clear();
    for ( auto it = obj.begin(), end = obj.end(); it != end; ++it )
        data.insert( std::make_pair( it.key().toInt(), it.value().toDouble() ) );
}

QJsonArray ToJson( AttPressureTime::Steps const& data )
{
    QJsonArray arr;
    foreach ( AttPressureTime::StepDataSet const& d, data )
    {
        arr.insert( arr.end(), ToJson( d ) );
    }
    return std::move( arr );
}
void FromJson( AttPressureTime::Steps& data, QJsonArray const& arr )
{
    data.clear();

    foreach (QJsonValue const& v, arr)
    {
        AttPressureTime::StepDataSet d;
        FromJson( d, v.toObject() );
        data.insert( data.end(), d );
    }
}

}

AttPressureTime::Data::Data():
    mPmax(0),
    mPtask(0),
    mPvpd(0),
    mCurrent(false)
{}

QJsonObject AttPressureTime::Data::Serialise() const
{
    QJsonObject obj;
    obj.insert("Pmax",  mPmax);
    obj.insert("Ptask", mPtask);
    obj.insert("Pvpd",  mPvpd);
    return obj;
}
bool AttPressureTime::Data::Deserialize( QJsonObject const& obj )
{
    mPmax   = obj.value("Pmax").toDouble();
    mPtask   = obj.value("Ptask").toDouble();
    mPvpd   = obj.value("Pvpd").toDouble();

    return true;
}

double AttPressureTime::Data::Error() const
{
    if ( !mPmax )
        return 0.0;
    return round((mPmax - mPtask)/mPvpd*10000)/100; //вывод погрешности в %
}

AttPressureTime::AttPressureTime():
    Attestaion("Определение приведенной погрешности для каждого\nдиапазона поддержания заданного давления в шине\nв течении определенного времени", 3)
{
    Reset();
}
bool AttPressureTime::Run()
{
    mSaveGraphData = false;
    Start();
    if ( IsStopped() )
        return false;

    return Success();
}
void AttPressureTime::Reset()
{
    mCurrenPos = 0;
    mSaveGraphData = false;
    mData.clear();
    Data d;
    d.mPtask = 3;
    d.mPvpd = 4;
    mData.push_back( std::move( d ) );
    d.mPtask = 5;
    d.mPvpd = 6;
    mData.push_back( std::move( d ) );
    d.mPtask = 9;
    d.mPvpd = 10;
    mData.push_back( std::move( d ) );
    mSteps.clear();
    mSteps.resize(3);
}
void AttPressureTime::UpdateData()
{
    auto const& mem = cpu::CpuMemory::Instance().Sensors;
    if (mCurrenPos<mData.size())
    {
        auto& dt = mData[mCurrenPos];
        dt.mCurrent = true;
        auto cur_press = mem.Pressure();
        if ( !mSaveGraphData && cur_press >= dt.mPtask - dt.mPtask*0.02 )
        {
            mSaveGraphData = true;
            mTime.start();
        }

        if ( mSaveGraphData )
        {
            int32_t sec = mTime.elapsed()/1000;
            mSteps[mCurrenPos][sec] = cur_press;
        }

        if ( dt.mPmax < cur_press )
            dt.mPmax = cur_press;

        if ( mControls.AttPressureTimeSave() )
        {
            for ( auto it = mData.begin(), end = mData.end(); it != end; ++it )
            {
                it->mCurrent = false;
            }
            //продолжить тест
            ++mCurrenPos;
            mSaveGraphData = false;
            mControls.AttPressureTimeSave(false);
        }
    }
}

QJsonObject AttPressureTime::Serialise() const
{
    QJsonObject obj = Attestaion::Serialise();
    obj.insert("CurrenPos",mCurrenPos);
    obj.insert("Data", ToJson(mData));
    obj.insert("Steps", ToJson(mSteps));
    return obj;
}
bool AttPressureTime::Deserialize( QJsonObject const& obj )
{
    FromJson( mData, obj.value("Data").toArray() );
    FromJson( mSteps, obj.value("Steps").toArray() );
    mCurrenPos = obj.value("CurrenPos").toInt();
    Attestaion::Deserialize( obj );
    return true;
}

void AttPressureTime::SetStartBit( bool b )
{
    mControls.AttPressureTimeStart( b );
}
void AttPressureTime::SetStopBit( bool b )
{
    mControls.AttPressureTimeStop( b );
}
bool AttPressureTime::StopBit()
{
    return mControls.AttPressureTimeStop();
}
AttPressureTime::DataSet const& AttPressureTime::GetData() const
{
    return mData;
}
AttPressureTime::DataSet& AttPressureTime::GetData()
{
    return mData;
}

bool AttPressureTime::DrawBody( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont text_font = painter.font();
    text_font.setFamily("Arial");
    text_font.setPointSize( 12 );

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
                "<table valign='middle' width='80%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                  "<th>Удерживаемое<br>давление, МПа</th>"
                  "<th>Давление ВПД,<br>МПа</th>"
                  "<th>Максимальное<br>давление, МПа</th>"
                  "<th>Приведенная <br>погрешность, %</th>"
                "</tr>";

        QString footer = "</table>"
                "</body>"
                "</html>";

        QString table = header;
        auto MakeRow = [ this ]( Data const& dt) -> QString
        {
            QString row =   "<tr>";
            row +=              "<td>"+ToString(dt.mPtask)+"</td>"
                                "<td>"+ToString(dt.mPvpd)+"</td>"
                                "<td>"+ToString(dt.mPmax)+"</td>"
                                "<td>"+ToString(dt.Error())+"</td>";
            row +=          "</tr>";
            return row;
        };

        QTextDocument doc;
        doc.setUndoRedoEnabled( false );
        doc.setTextWidth( free_rect.width() );
        doc.setUseDesignMetrics( true );
        doc.setDefaultTextOption ( QTextOption (Qt::AlignHCenter )  );

        int rows_prapared = 0;
        for ( auto i = PrintedRows; i < mData.size(); ++i ) //PrintedRows
        {
            QString row = MakeRow( mData[i] );
            QString tmp = table + row + footer;
            doc.setHtml( tmp );
            auto h = doc.documentLayout()->documentSize().height();
            if ( h > free_rect.height() )
                break;
            ++rows_prapared;
            table += row;
        }
        if ( PrintedRows && rows_prapared )
            num += PrintedPage;
        else if( PrintedPage )
            num += PrintedPage - 1;
        PrintedRows += rows_prapared;
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
            ++PrintedPage;
        }, 1, h );
    }

    DrawHelper drw( painter, free_rect );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowLeft( rect, text_font, Qt::black, "Допускаемая приведенная погрешность поддержания заданного внутреннего давления" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &drw, &text_font ]( QRect const& rect )
    {
        drw.DrawRowLeft( rect, text_font, Qt::black, "в шине от ВПД в течение 2-х минут \u00B1 3,0%"  );
    }, 1.5 );

    return res;
}

namespace
{
QVector<ff0x::GraphBuilder::LinePoints> Process( AttPressureTime::Steps const& src, QPointF& x_range, QPointF& y_range )
{
    QVector<ff0x::GraphBuilder::LinePoints> result;
    result.resize(src.size());

    for ( int i = 0; i < src.size(); ++i )
    {
        for ( auto it = src[i].begin(), end = src[i].end(); it != end; ++it )
        {
            double const& x = it->first;
            double const& y = it->second;

            if ( !i && it == src[i].begin() )
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

            result[i].push_back( QPointF( x, y ) );
        }

    }

    return std::move( result );
}

}

class AttPressureTime::GrapfData
{
public:
    GrapfData( AttPressureTime const* test )
    {
        data = Process( test->mSteps, x_range, y_range );
    }

    QVector<ff0x::GraphBuilder::LinePoints> data;


    QPointF x_range;
    QPointF y_range;
};

void AttPressureTime::PaintGraph(QPainter& painter, QFont const& font, const QRect &rect, double skale) const
{
    mGrapfs.reset( new GrapfData( this ) );

    painter.save();

    QString x_msg = "Время выдержки, сек";
    QString y_msg = "Давление, МПа";


    QFont f = font;
    f.setPointSize( 12 );
    int w = (rect.width())*skale;
    int h = (rect.height())*skale;

    ff0x::GraphBuilder builder ( w, h, ff0x::GraphBuilder::PlusPlus, f );
    ff0x::BasicGraphBuilder::GraphDataLine lines;
    for ( int i = 0; i < mGrapfs->data.size(); ++i )
        lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->data[i], ff0x::BasicGraphBuilder::LabelInfo( "Результат " + QString::number(i+1), Qt::darkBlue ) ) );

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

    painter.restore();
}

}
