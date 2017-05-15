#include "viewer.h"
#include "ui_viewer.h"
#include "test_params.h"
#include "test.h"
#include <QFileDialog>
#include "settings/settings.h"
#include <QPrintDialog>

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

    bool print_all_tests = test::CURRENT_PARAMS->PrintAll();
    QRect rc = PreparePage( painter, QRect(0,0,793,1123) ); //397 562

    auto PrintTest = [&]( test::Test* test )
    {
        if (!test)
            return;

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
    };

    test::CURRENT_PARAMS->Draw( painter, rc, CompareWidth );

    if ( print_all_tests )
    {
        auto const& tc = test::CURRENT_PARAMS->TestsCase();
        for ( auto it = tc.begin(), end = tc.end(); it != end; ++it   )
        {
           test::Test* test = it->second.get();
           PrintTest( test );
        }
    }
    else
    {
        test::Test* test = test::CURRENT_PARAMS->TestForExec();
        PrintTest( test );
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

        Print( printer );
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

    test::CURRENT_PARAMS->DrawLogo( painter, work_area, logo );

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

void Viewer::on_Print_clicked()
{
    QPrintDialog printDialog( this );
    if (printDialog.exec() == QDialog::Accepted)
    {
        Print( *printDialog.printer() );
    }
}

void Viewer::Print(QPrinter &printer)
{
    QPainter painter(&printer);
    QPoint start (0,0);
    start -= printer.pageRect().topLeft();
    painter.translate( start );

    bool print_all_tests = test::CURRENT_PARAMS->PrintAll();
    QRect rc = PreparePage( painter, printer.paperRect() );

    auto PrintTest = [&]( test::Test* test )
    {
        if (!test)
            return;

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
    };

    test::CURRENT_PARAMS->Draw( painter, rc, CompareWidth );

    if ( print_all_tests )
    {
        auto const& tc = test::CURRENT_PARAMS->TestsCase();
        for ( auto it = tc.begin(), end = tc.end(); it != end; ++it   )
        {
           test::Test* test = it->second.get();
           PrintTest( test );
        }
    }
    else
    {
        test::Test* test = test::CURRENT_PARAMS->TestForExec();
        PrintTest( test );
    }

    if ( test::CURRENT_PARAMS->HasResults() )
    {
        printer.newPage();
        rc = PreparePage( painter, printer.paperRect() );
        test::CURRENT_PARAMS->DrawResults( painter, rc );
    }
    painter.end();
}
