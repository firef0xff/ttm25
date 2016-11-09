#include "inside_pressure.h"
#include <QJsonObject>
#include <QJsonArray>
#include <GraphBuilder/graph_builder.h>
#include "../test_params.h"
#include <QTextDocument>
#include <QtGui/QAbstractTextDocumentLayout>

namespace test
{

namespace
{

QJsonArray ToJson( InsidePressure::DataSet const& data )
{
    QJsonArray arr;
    foreach ( InsidePressure::Point const& d, data )
    {
        arr.insert( arr.end(), d.Serialise() );
    }
    return std::move( arr );
}
InsidePressure::DataSet FromJson( QJsonArray const& arr )
{
    InsidePressure::DataSet data;

    foreach (QJsonValue const& v, arr)
    {
        InsidePressure::Point d;
        if ( d.Deserialize( v.toObject() ) )
            data.insert( data.end(), d );
    }
    return std::move( data );
}

ff0x::NoAxisGraphBuilder::LinePoints Process( InsidePressure::DataSet const& src, QPointF& x_range, QPointF& y_range )
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
    if ( 0 > x_range.x() )
        x_range.setX( 0 );
    if ( 0 < x_range.y() )
        x_range.setY( 0 );

    if ( 0 > y_range.x() )
        y_range.setX( 0 );
    if ( 0 < y_range.y() )
        y_range.setY( 0 );


    return std::move( result );
}
}//namespace
InsidePressure::Point::Point():
    mX(0),mY(0)
{
}
InsidePressure::Point::Point( double x, double y ):
    mX(x),
    mY(y)
{}
double const& InsidePressure::Point::X() const
{
    return mX;
}
double const& InsidePressure::Point::Y() const
{
    return mY;
}

QJsonObject InsidePressure::Point::Serialise() const
{
    QJsonObject obj;
    obj.insert("x", mX);
    obj.insert("y", mY);
    return std::move( obj );
}
bool InsidePressure::Point::Deserialize( QJsonObject const& obj )
{
    mX = obj.value("x").toDouble();
    mY = obj.value("y").toDouble();
}

class InsidePressure::GrapfData
{
public:
    GrapfData( InsidePressure const* test, QString compare_width )
    {
        QPointF x_range_e;
        QPointF y_range_e;
        //поиск данных теста
        bool use_etalone = false;
        foreach (QJsonValue const& val, test::ReadFromFile(compare_width).value("Results").toArray())
        {
            auto obj = val.toObject();
            if ( obj.value("id").toInt() == test->mId )
            {
                InsidePressure::DataSet data = FromJson( obj.value("data").toObject().value("Data").toArray() );
                dataA_e2 = Process( data, x_range_e, y_range_e );
                use_etalone = true;
            }
        }

        dataA = Process( test->mData, x_range, y_range );
        x_range = ff0x::MergeRanges( x_range, x_range_e, use_etalone );
        y_range = ff0x::MergeRanges( y_range, y_range_e, use_etalone );
    }

    ff0x::GraphBuilder::LinePoints dataA;
    ff0x::GraphBuilder::LinePoints dataA_e2;

    QPointF x_range;
    QPointF y_range;
};


InsidePressure::InsidePressure():
    TestCommonData( "Испытание внутренним гидравлическим давлением", 1 )
{
#ifdef DEMO
    for ( auto x = 0; x <100; ++x )
    {
        mData.append(Point( x, (10+x*x/50) ));
    }

#endif
}
InsidePressure::~InsidePressure()
{}

bool InsidePressure::Run()
{
    bool work = true;
    bool done = true;

    Start();
    Wait( work, done );
    if ( IsStopped() )
        return false;

    return Success();
}
bool InsidePressure::Success() const
{
    return mSuccess;
}

QJsonObject InsidePressure::Serialise() const
{
    QJsonObject obj = TestCommonData::Serialise();
    obj.insert("Data", ToJson( mData ) );

    return obj;
}
bool InsidePressure::Deserialize( QJsonObject const& obj )
{
    mData = FromJson( obj.value("Data").toArray() );
    TestCommonData::Deserialize( obj );
    return true;
}

bool InsidePressure::Draw( QPainter& painter, QRect &free_rect, QString  const& compare_width ) const
{
    bool res = false;
    bool is_auto = false;
    uint32_t num = 0;
    if ( is_auto )
        res = DrawAuto(num, painter,free_rect);
    else
        res = DrawAvia(num, painter,free_rect);
    res = DrawGraph(num, painter,free_rect, compare_width);
    return res;
}

bool InsidePressure::DrawAuto( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont header_font = painter.font();
    header_font.setFamily("Arial");
    header_font.setPointSize( 14 );
    header_font.setBold(true);
    QFont result_font = header_font;
    result_font.setUnderline(true);
    result_font.setBold(false);
    QFont text_font = header_font;
    text_font.setPointSize( 12 );
    text_font.setBold(false);
    QFont underline_font = text_font;
    underline_font.setPointSize( 6 );


    auto DrawRowCenter = [ &painter, &free_rect ](QRect const& place, QFont const& font, QColor const& color, QString const& text )
    {
        painter.save();
        QFontMetrics metrix( font );
        QPoint start_point( place.center().x() - metrix.width( text ) / 2, place.center().y() +metrix.height()/2);
        painter.setFont( font );
        painter.setPen( color );
        painter.drawText( start_point, text );
        painter.restore();
    };
    auto DrawRowLeft = [ &painter, &free_rect ](    QRect const& place,
                                                    QFont const& font,
                                                    QColor const& color1,
                                                    QString const& label,
                                                    QColor const& color2 = Qt::black,
                                                    QString const& value = "")
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
    };

    bool res = DrawLine( num, free_rect, header_font,
    [ this, &painter, &DrawRowCenter, &header_font ]( QRect const& rect )
    {
        DrawRowCenter( rect, header_font, Qt::black, "Протокол No _________ от #DATE#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, header_font,
    [ this, &painter, &DrawRowCenter, &header_font ]( QRect const& rect )
    {
        DrawRowCenter( rect, header_font, Qt::black, "испытания шины #NAME# производства" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& )
    {
    }, 3 );

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
            "<table width='100%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                    "<th rowspan='4'>Дата изготовления<br>порядковый<br>номер изделия</th>"
                    "<th colspan='5'>Оценка устойчивости шины к разрыву</th>"
                "</tr>"
                "<tr>"
                    "<th colspan='5'>Правила ЕЭК ООН No106 прил. 8</th>"
                "</tr>"
                "<tr>"
                    "<th colspan='2'>Давление в шине<br>при измерении,<br>МПа (кгс/см^2)</th>"
                    "<th colspan='2'>Поддержание давления<br>на неизменном уровне<br>в шине, мин.</th>"
                    "<th rowspan='2'>Состояние шин после испытаний<br>(наличие и виды дефектов)</th>"
                "</tr>"
                "<tr>"
                    "<th>Норматив</th>"
                    "<th>Факт</th>"
                    "<th>Норматив</th>"
                    "<th>Факт</th>"
                "</tr>";

    QString footer = "</table>"
            "</body>"
            "</html>";

    QString table = header;
    auto MakeRow = [ this ]() -> QString
    {
        QString row =   "<tr>";
        row +=              "<td>#1#</td>"
                            "<td>#2#</td>"
                            "<td>#3#</td>"
                            "<td>#4#</td>"
                            "<td>#5#</td>"
                            "<td>#6#</td>";
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
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& )
    {
    }, 3 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_font, Qt::black, "Заключение: ______________________________________________________" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& )
    {
    }, 3 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& )
    {
    }, 3 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_font, Qt::black, "Главный инженер ООО \"ШИЦ \"Вершина \"  ______________" + QString(10,' ') +"__________________" );
    }, 1.5 );
    res = DrawLine( num, free_rect, underline_font,
    [ this, &painter, &DrawRowLeft, &underline_font, &free_rect ]( QRect const& rect )
    {
        DrawRowLeft( rect, underline_font, Qt::black, QString(180,' ') + "подпись" + QString(60,' ') + "расшифровка подписи" );
        free_rect.setHeight( 0 );
    }, 1.5 );
    return res;
}
bool InsidePressure::DrawAvia( uint32_t& num, QPainter& painter, QRect &free_rect ) const
{
    QFont header_font = painter.font();
    header_font.setFamily("Arial");
    header_font.setPointSize( 14 );
    QFont result_font = header_font;
    result_font.setUnderline(true);
    QFont text_font = header_font;
    text_font.setPointSize( 12 );
    QFont text_bold_font = text_font;
    text_bold_font.setBold(true);
    QFont underline_font = text_font;
    underline_font.setPointSize( 6 );


    auto DrawRowCenter = [ &painter, &free_rect ](QRect const& place, QFont const& font, QColor const& color, QString const& text )
    {
        painter.save();
        QFontMetrics metrix( font );
        QPoint start_point( place.center().x() - metrix.width( text ) / 2, place.center().y() +metrix.height()/2);
        painter.setFont( font );
        painter.setPen( color );
        painter.drawText( start_point, text );
        painter.restore();
    };
    auto DrawRowLeft = [ &painter, &free_rect ](    QRect const& place,
                                                    QFont const& font,
                                                    QColor const& color1,
                                                    QString const& label,
                                                    QColor const& color2 = Qt::black,
                                                    QString const& value = "")
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
    };
    auto DrawRowRight = [ &painter, &free_rect ](   QRect const& place,
                                                    QFont const& font,
                                                    QColor const& color1,
                                                    QString const& label)
    {
        painter.save();
        QFontMetrics metrix( font );
        QPoint start_point( place.right() - metrix.width( label ) , place.center().y()+metrix.height()/2 );
        painter.setFont( font );
        painter.setPen( color1 );
        painter.drawText( start_point, label );
        painter.restore();
    };


    bool res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowRight, &text_font ]( QRect const& rect )
    {
        DrawRowRight( rect, text_font, Qt::black, "Прил. М И11-2005 #DATE#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, "ОАО \"ЯШЗ\"" );
    }, 1 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, "АКТ" );
    }, 1 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, "#DATE# No" );
    }, 1 );

    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& )
    {
    }, 1.5 );

    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_font, Qt::black, QString(5,' ') + "Проведены испытания авиационных шин на прочность внутренним гидравлическим" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_font, Qt::black, "давлением (методика МИ 5С-2006) в соответствии с требованиями технических условий" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_font, Qt::black, "получены следующие результаты:" );
    }, 1.5 );

    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &text_font ]( QRect const& )
    {
    }, 1.5 );

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
                  "<th rowspan='2'>Обозна-<br>чение шины</th>"
                  "<th rowspan='2'>Обозна-<br>чение<br>модели</th>"
                  "<th rowspan='2'>Заводской номер</th>"
                  "<th rowspan='2'>Мас-<br>са,<br>кг</th>"
                  "<th rowspan='2'>Марки-<br>ровка</th>"
                  "<th rowspan='2'>Обозна-<br>чение<br>ККТ</th>"
                  "<th colspan='2'>Контрольно-раз-<br>рушающее давление<br>МПа (кгс/см^2 )</th>"
                  "<th rowspan='2'>Характер разрушения</th>"
                "</tr>"
                "<tr>"
                  "<th>по ТУ, не менее</th>"
                  "<th>значение измеренное</th>"
                "</tr>";

        QString footer = "</table>"
                "</body>"
                "</html>";

        QString table = header;
        auto MakeRow = [ this ]() -> QString
        {
            QString row =   "<tr>";
            row +=              "<td>#1#</td>"
                                "<td>#2#</td>"
                                "<td>#3#</td>"
                                "<td>#4#</td>"
                                "<td>#5#</td>"
                                "<td>#6#</td>"
                                "<td>#7#</td>"
                                "<td>#8#</td>"
                                "<td>#9#</td>";
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
    }

    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &text_font ]( QRect const& )
    {
    }, 3 );


    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowCenter, &text_font ]( QRect const& rect )
    {
        DrawRowCenter( rect, text_font, Qt::black, "Используемые средства измерения." );
    }, 1.5 );

    {
        QString header = "<html>"
                "<head>"
                  "<meta charset='utf-8'>"
                  "<style type='text/css'>"
                       "td { text-align: center;}"
                       "th { font-weight: normal;}"
                       "table {border-collapse: collapse; border-style: solid; }"
                 "</style>"
                "</head>"
                "<body>"
                "<table valign='middle' width='100%' border='1.5' cellspacing='-0.5' cellpadding='-0.5'>"
                "<tr>"
                  "<th>Наименование и тип средства измерения</th>"
                  "<th>Диапазон измерений</th>"
                  "<th>Класс точности (разряд, цена деления)</th>"
                  "<th>Номер средства измерения</th>"
                "</tr>"
                "<tr>"
                  "<td>Датчик давления<br>Метран 100-ДИ 1172</td>"
                  "<td>0-25 МПа</td>"
                  "<td>01</td>"
                  "<td>343100</td>"
                "</tr>"
                "<tr>"
                  "<td>Весы ВУ-3/150</td>"
                  "<td>0 - 150 кг</td>"
                  "<td>КТШ</td>"
                  "<td>1254</td>"
                "</tr>";

        QString footer = "</table>"
                "</body>"
                "</html>";

        QString table = header;
        auto MakeRow = [ this ]() -> QString
        {
            QString row =   "<tr>";
            row +=              "<td>#1#</td>"
                                "<td>#2#</td>"
                                "<td>#3#</td>"
                                "<td>#4#</td>";
            row +=          "</tr>";
            return row;
        };

        QTextDocument doc;
        doc.setUndoRedoEnabled( false );
        doc.setTextWidth( free_rect.width() );
        doc.setUseDesignMetrics( true );
        doc.setDefaultTextOption ( QTextOption (Qt::AlignHCenter )  );
//        table += MakeRow();
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
    }
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& )
    {
    }, 3 );
    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_font, Qt::black, "Представитель ООО \"ШИЦ\"Вершина\"  ____________  ____________  ____________" );
    }, 1.5 );
    res = DrawLine( num, free_rect, underline_font,
    [ this, &painter, &DrawRowLeft, &underline_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, underline_font, Qt::black, QString(165,' ') + "подпись" + QString(30,' ') + "инициалы, фамилия"+ QString(35,' ') + "дата" );
    }, 1.5 );

    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_font, Qt::black, "Контролер  ____________  ____________  ____________" );
    }, 1.5 );
    res = DrawLine( num, free_rect, underline_font,
    [ this, &painter, &DrawRowLeft, &underline_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, underline_font, Qt::black, QString(65,' ') + "подпись" + QString(30,' ') + "инициалы, фамилия"+ QString(35,' ') + "дата" );
    }, 1.5 );

    res = DrawLine( num, free_rect, underline_font,
    [ this, &painter, &DrawRowLeft, &underline_font ]( QRect const& )
    {
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, "Заключение представительства заказчика ________________________________" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, QString(70,'_') );
    }, 1.5 );
    res = DrawLine( num, free_rect, underline_font,
    [ this, &painter, &DrawRowLeft, &underline_font ]( QRect const& )
    {
    }, 3 );

    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &DrawRowLeft, &text_font ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_font, Qt::black, "____________  ____________  ____________" );
    }, 1.5 );
    res = DrawLine( num, free_rect, underline_font,
    [ this, &painter, &DrawRowLeft, &underline_font, &free_rect ]( QRect const& rect )
    {
        DrawRowLeft( rect, underline_font, Qt::black, QString(20,' ') + "подпись" + QString(30,' ') + "инициалы, фамилия"+ QString(35,' ') + "дата" );
        free_rect.setHeight( 0 );
    }, 1.5 );

    return res;
}
bool InsidePressure::DrawGraph( uint32_t& num, QPainter& painter, QRect &free_rect, QString  const& compare_width ) const
{

    QFont header_font = painter.font();
    header_font.setFamily("Arial");
    header_font.setPointSize( 14 );
    QFont result_font = header_font;
    result_font.setUnderline(true);
    QFont text_font = header_font;
    text_font.setPointSize( 12 );
    QFont text_bold_font = text_font;
    text_bold_font.setBold(true);


    auto DrawRowCenter = [ &painter, &free_rect ](QRect const& place, QFont const& font, QColor const& color, QString const& text )
    {
        painter.save();
        QFontMetrics metrix( font );
        QPoint start_point( place.center().x() - metrix.width( text ) / 2, place.center().y() +metrix.height()/2);
        painter.setFont( font );
        painter.setPen( color );
        painter.drawText( start_point, text );
        painter.restore();
    };
    auto DrawRowLeft = [ &painter, &free_rect ](    QRect const& place,
                                                    QFont const& font,
                                                    QColor const& color1,
                                                    QString const& label,
                                                    QColor const& color2 = Qt::black,
                                                    QString const& value = "")
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
    };

    QFontMetrics m(text_bold_font);
    int width = m.width(QString(45,'0'));
    char symbol = '.';
    auto FillToSize = [ width, &m, symbol ]( QString text )
    {
        while( m.width( text + symbol ) < width )
            text += symbol;
        return text + " ";
    };

    bool res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font, &FillToSize ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, FillToSize("Дата испытания:"), Qt::black, "#DATE#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font, &FillToSize ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, FillToSize("Обозначение шины:"), Qt::black, "#NAME#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font, &FillToSize ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, FillToSize("Маркировка:"), Qt::black, "#MARK#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font, &FillToSize ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, FillToSize("Обозначение ККТ:"), Qt::black, "#KKT#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font, &FillToSize ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, FillToSize("Заводской номер:"), Qt::black, "#SERIAL#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font, &FillToSize ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, FillToSize("Масса шины, кг:"), Qt::black, "#MASS#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font, &FillToSize ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, FillToSize("Контрольно разрущающее давление,кгс\\см2:"), Qt::black, "#CONTROL PRESSURE#" );
    }, 1.5 );
    res = DrawLine( num, free_rect, text_bold_font,
    [ this, &painter, &DrawRowLeft, &text_bold_font, &FillToSize ]( QRect const& rect )
    {
        DrawRowLeft( rect, text_bold_font, Qt::black, FillToSize("Давление разрушения, кгс\\см2:"), Qt::black, "#PRESSURE#" );
    }, 1.5 );


    res = DrawLine( num, free_rect, text_bold_font,
    []( QRect const& )
    {
    }, 1.5 );


    res = DrawLine( num, free_rect, text_font,
    [ this, &painter, &text_font, &compare_width ]( QRect const& rect )
    {
        PaintGraph( painter, text_font, rect, compare_width );
    }, 1, 480  );

    free_rect.setHeight( 0 );

    return res;
}

void InsidePressure::PaintGraph( QPainter& painter, QFont const& font, QRect const &rect, QString  const& compare_width ) const
{
    QFontMetrics metrix( font );
    if (!mGrapfs)
        mGrapfs.reset( new GrapfData( this, compare_width ) );

    painter.save();

    QFont f = font;
    f.setPointSize( 12 );
    int w = (rect.width())*0.98;
    int h = (rect.height() - metrix.height())*0.98;

    ff0x::NoAxisGraphBuilder builder ( w, h, f );
    ff0x::NoAxisGraphBuilder::GraphDataLine lines;
    lines.push_back( ff0x::NoAxisGraphBuilder::Line(mGrapfs->dataA, ff0x::NoAxisGraphBuilder::LabelInfo( "Испытуемый аппарат", Qt::blue ) ) );
    if ( !mGrapfs->dataA_e2.empty() )
        lines.push_back( ff0x::NoAxisGraphBuilder::Line(mGrapfs->dataA_e2, ff0x::NoAxisGraphBuilder::LabelInfo( "Предыдущий результат", Qt::gray ) ) );

    QRect p1(rect.left(), rect.top(), w, h );
    QRect p1t(p1.left(), p1.bottom(), p1.width(), metrix.height());

    {
        QPointF x_range;
        QPointF y_range;
        double x_step = 0;
        double y_step = 0;
        ff0x::DataLength( mGrapfs->x_range,x_range, x_step );
        ff0x::DataLength( mGrapfs->y_range,y_range, y_step );

        painter.drawPixmap( p1, builder.Draw( lines, x_range, y_range, x_step, y_step, "Время, мин.", "Давление ,кгс\\см2", true ) );
    }

    painter.restore();
}

}
