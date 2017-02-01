#include "attestation_tests.h"
#include "attestation_params.h"
#include <thread>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <GraphBuilder/graph_builder.h>

namespace test
{

Attestaion::Attestaion ( QString const& name, uint8_t id ):
    Test(name, id),
    mSuccess(false),
    mControls(cpu::CpuMemory::Instance().AttestationLaunchControls)
{}

void Attestaion::Start()
{
    mControls.Reset();
    mControls.Write();

    while( !StopBit() )
    {
        mControls.Read();
        UpdateData();
        if ( IsStopped() )
        {
            SetStopBit( true );
            mControls.Write();
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

        mControls.Write();
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
    }
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
    res = DrawFoter(num, painter,free_rect);

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
        drw.DrawRowCenter( rect, header_font, Qt::black, Name() );
    }, 1.5 );
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


AttPressure::AttPressure():
    Attestaion("Аттестация по давлению", 0)
{
    Reset();
}

bool AttPressure::Run()
{
    Start();
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
        dt.mResult = mem.Pressure();
        if (mControls.AttPressureSave() && !mWait)
        {
            for ( auto it = mData.begin(), end = mData.end(); it != end; ++it )
            {
                it->mCurrent = false;
            }
            dt.mCurrent = true;
            mWait = true;
            ++mCurrenPos;
        }
    }
    if ( mRunMarker && *mRunMarker )
    {
        *mRunMarker = false;
        mWait = false;
        if (mControls.AttPressureSave())
            mControls.AttPressureSave(false);
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
    mWait=false;
    mData.clear();
    for ( int i = 5; i <= 100; i += 5  )
    {
        Data d;
        d.mTask = round(100*i*1/10.19716212978)/100.0;
        if ( i == 5 )
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
    obj.insert("Wait",mWait);
    return obj;
}
bool AttPressure::Deserialize( QJsonObject const& obj )
{
    FromJson( mData, obj.value("Data").toArray() );
    mCurrenPos = obj.value("CurrenPos").toInt();
    mWait = obj.value("Wait").toBool();
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
                "<table valign='middle' width='50%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                  "<th>Заданное<br>давление, МПа</th>"
                  "<th>Достигнутое<br>значение, МПа</th>"
                  "<th>Контрольное<br>значение, МПа</th>"
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
                                "<td>"+ToString(dt.mFact)+"</td>";
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
    Attestaion("Аттестация по времени", 1)
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


AttPressureSpeed::AttPressureSpeed():
    Attestaion("Аттестация по времени нарастания давления", 2)
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
    mCurrenTime = 0;
    mData.clear();
    mData.push_back(Data());
}
void AttPressureSpeed::UpdateData()
{
    auto const& mem = cpu::CpuMemory::Instance().Sensors;
    int time = mem.Time();
    int ofset = AttestationParams::Instance().UpdatePeriod();
    if ( mCurrenTime == time )
        return;

    if ( time < mCurrenTime + ofset )
        return;

    mCurrenTime = time;
    Data dt;
    dt.mCpuTime = round( mem.Time()*100)/100.0;
    dt.mResult = mem.Pressure();
    mData.push_back(dt);
}

QJsonObject AttPressureSpeed::Serialise() const
{
    QJsonObject obj = Attestaion::Serialise();
    obj.insert("CurrenPos",mCurrenTime);
    obj.insert("Data", ToJson(mData));
    return obj;
}
bool AttPressureSpeed::Deserialize( QJsonObject const& obj )
{
    FromJson( mData, obj.value("Data").toArray() );
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
AttPressureSpeed::DataSet const& AttPressureSpeed::GetData() const
{
    return mData;
}
AttPressureSpeed::DataSet& AttPressureSpeed::GetData()
{
    return mData;
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
                  "<th>Давление, сек</th>"
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
        data = Process( test->mData, x_range, y_range );
    }

    QVector<ff0x::GraphBuilder::LinePoints> data;


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
    lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->data[0], ff0x::BasicGraphBuilder::LabelInfo( "Результат", Qt::darkBlue ) ) );
    lines.push_back( ff0x::BasicGraphBuilder::Line(mGrapfs->data[1], ff0x::BasicGraphBuilder::LabelInfo( "Контрольное значение", Qt::darkRed ) ) );

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
