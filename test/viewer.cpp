#include "viewer.h"
#include "ui_viewer.h"
#include "test_params.h"
#include "test.h"
#include <QFileDialog>
#include "settings/settings.h"
#include <QPrintEngine>

Viewer::Viewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewer),
    mPageNo( 0 ),
    logo( "./img/vershina_small.png", "PNG" )
{
    ui->setupUi(this);

    Init();
}

void Viewer::Init()
{
    mPages.clear();
    QPixmap pixmap( 793, 1123 );
    QPainter painter(&pixmap);
    QRect rc = PreparePage( painter, QRect(0,0,793,1123) ); //397 562


    test::CURRENT_PARAMS->Draw( painter, rc, CompareWidth );
    if ( test::Test* test =  test::CURRENT_PARAMS->TestForExec() )
    {
        test->ResetDrawLine();

        bool draw = false;
        while( !draw )
        {
            draw = test->Draw( painter, rc, CompareWidth );
            if ( !draw )
            {
                painter.end();
                mPages.push_back( pixmap );
                pixmap = QPixmap( 793, 1123 );
                painter.begin( &pixmap );
                rc = PreparePage( painter, QRect(0,0,793,1123) );
            }
            painter.end();
            painter.begin( &pixmap );
            QFontMetrics m (painter.font());
            rc.setTop( rc.top() + m.height() );
        }
    }

    if ( test::CURRENT_PARAMS->HasResults() )
    {
        painter.end();
        mPages.push_back( pixmap );
        pixmap = QPixmap( 793, 1123 );
        painter.begin( &pixmap );
        rc = PreparePage( painter, QRect(0,0,793,1123) );
        test::CURRENT_PARAMS->DrawResults( painter, rc );
    }


    painter.end();
    mPages.push_back( pixmap );

    DrawPage();
}

Viewer::~Viewer()
{
    delete ui;
}
void Viewer::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent( e );
    emit closed();
}
void Viewer::on_pushButton_clicked()
{
    if ( mPageNo > 0 )
        --mPageNo;
    DrawPage();
}

void Viewer::on_pushButton_2_clicked()
{
    if ( mPageNo < ( mPages.size() - 1 ) )
        ++mPageNo;
    DrawPage();
}

void Viewer::on_Save_clicked()
{
    QString file_name;
    QFileDialog dlg;
    dlg.setFileMode( QFileDialog::AnyFile );
    dlg.setDirectory( app::Settings::Instance().TestPath() );
    dlg.setNameFilter( "Результаты испытаний (*.res )" );
    dlg.setAcceptMode( QFileDialog::AcceptSave );
    dlg.setViewMode( QFileDialog::Detail );
    if ( dlg.exec() )
        file_name = dlg.selectedFiles().front();
    if ( !file_name.isEmpty() )
    {
        if ( !file_name.endsWith(".res", Qt::CaseInsensitive) )
            file_name += ".res";
        test::DataToFile( file_name, *test::CURRENT_PARAMS );
    }
}

void Viewer::on_SavePDF_clicked()
{
    QString file_name;
    QFileDialog dlg;
    dlg.setFileMode( QFileDialog::AnyFile );
    dlg.setDirectory( app::Settings::Instance().TestPath() );
    dlg.setNameFilter( "Результаты PDF (*.pdf )" );
    dlg.setAcceptMode( QFileDialog::AcceptSave );
    dlg.setViewMode( QFileDialog::Detail );
    if ( dlg.exec() )
        file_name = dlg.selectedFiles().front();
    if ( !file_name.isEmpty() )
    {
        if ( !file_name.endsWith(".pdf", Qt::CaseInsensitive) )
            file_name += ".pdf";
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOrientation( QPrinter::Portrait );
        printer.setPaperSize( QPrinter::A4 );
        printer.setPageMargins( 20, 20, 5, 10, QPrinter::Millimeter );
        printer.setOutputFileName(file_name);

        QPainter painter(&printer);
        QPoint start (0,0);
        start -= printer.pageRect().topLeft();
        painter.translate( start );
        QRect rc = PreparePage( painter, printer.paperRect() );

        test::CURRENT_PARAMS->Draw( painter, rc, CompareWidth );
        if( test::Test* test= test::CURRENT_PARAMS->TestForExec() )
        {
            test->ResetDrawLine();

            bool draw = false;
            while( !draw )
            {
                draw = test->Draw( painter, rc, CompareWidth );
                if ( !draw )
                {
                    printer.newPage();
                    rc = PreparePage( painter, printer.paperRect() );
                }
                QFontMetrics m (painter.font());
                rc.setTop( rc.top() + m.height() );
            }
        }
        if ( test::CURRENT_PARAMS->HasResults() )
        {
            printer.newPage();
            rc = PreparePage( painter, printer.paperRect() );
            test::CURRENT_PARAMS->DrawResults( painter, rc );
        }
        painter.end();
    }
}

void Viewer::DrawPage()
{
    ui->mPicture->setPixmap( mPages[mPageNo] );
}

QRect Viewer::PreparePage( QPainter& painter, QRect const& page_rect )
{
    painter.fillRect( page_rect, Qt::white );
    QRect work_area( 76, 76, 698, 1010 );// 698 x 1010 19 = 5мм
    QRect print_area( work_area.left() + 38, work_area.top() + 10, work_area.width() - ( 38 + 19 ), work_area.height() - 19 );

    QRect header_rect( 0, 0, 55, 55 );
    header_rect.setHeight( logo.height() * header_rect.width() / logo.width() );

    painter.save();
    QPoint header_point( work_area.center().x() - header_rect.width()/2, work_area.top() - header_rect.height() - 2 );
    painter.translate( header_point );
    painter.drawPixmap( header_rect, logo );
    painter.restore();

//    painter.drawRect( work_area );

//    painter.drawRect( print_area );

    return work_area;
}

void Viewer::on_Compare_clicked()
{
    QFileDialog dlg;
    dlg.setFileMode( QFileDialog::ExistingFile );
    dlg.setDirectory( app::Settings::Instance().TestPath() );
    dlg.setNameFilter( "Результаты испытаний (*.res )" );
    dlg.setViewMode( QFileDialog::Detail );
    if ( dlg.exec() )
    {
        CompareWidth = dlg.selectedFiles().front();
    }
    else
        CompareWidth.clear();
    Init();
}
