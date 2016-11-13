#include "main_window.h"
#include "ui_main_window.h"
#include "users/users.h"
#include "settings/settings_wnd.h"
#include "test/viewer.h"
#include "test/test_params.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mTest( std::bind( &MainWindow::RepaintGraph, this ) )
{
    ui->setupUi(this);
    ui->ResultsData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QObject::connect( &Updater, SIGNAL(update()), this, SLOT(onUpdateControls()) );
    Updater.start();    
    test::CURRENT_PARAMS.TestCase( mTest );
}

MainWindow::~MainWindow()
{
    Updater.stop();

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QObject::disconnect( mChildWindow.get(), SIGNAL(closed()), this, SLOT(enable_modes()) );
    mChildWindow.reset();
    QWidget::closeEvent( e );
}
void MainWindow::showEvent( QShowEvent *e )
{
    CheckRights();
    ui->statusBar->showMessage("Текущий пользователь: " + app::Settings::Instance().User() );
    QMainWindow::showEvent( e );
}
void MainWindow::resizeEvent( QResizeEvent *e )
{
    QMainWindow::resizeEvent( e );
    RepaintGraph();
}

void MainWindow::CheckRights()
{
    if ( app::Settings::Instance().UserAccess() != app::Admin )
    {
//        ui->Users->setEnabled( false );
        if ( app::Settings::Instance().UserAccess() != app::Master )
        {
//            ui->ManualControl->setEnabled( false );
//            ui->TestCase1->setEnabled( false );
//            ui->TestCase2->setEnabled( false );
//            ui->TestCase3->setEnabled( false );
//            ui->TestCase4->setEnabled( false );
//            ui->menu_3->setEnabled( false );
//            ui->menu_4->setEnabled( false );
        }
    }
}

void MainWindow::ShowChildWindow( ChildPtr child, bool maximized )
{
    if ( mChildWindow.get() )
        QObject::disconnect( mChildWindow.get(), SIGNAL(closed()), this, SLOT(enable_modes()) );
    mChildWindow.reset( child.release() );
    QObject::connect( mChildWindow.get(), SIGNAL(closed()), this, SLOT(enable_modes()) );

    if ( maximized )
        mChildWindow->showMaximized();
    else
        mChildWindow->show();
}

void MainWindow::onUpdateControls()
{

}
void MainWindow::RepaintGraph()
{
    QPixmap pixmap( ui->Graph->width()-2, ui->Graph->height()-2 );
    QPainter painter(&pixmap);
    QFont font = painter.font();
    font.setFamily("Arial");
    font.setPointSize(12);
    auto r = pixmap.rect();
    painter.fillRect( r, Qt::white );
    mTest.PaintGraph( painter, font, pixmap.rect(), "", 1,
                      static_cast<test::InsidePressure::PressureUnits>( ui->puUnits->currentIndex() ),
                      static_cast<test::InsidePressure::TimeUnits>( ui->tuUnits->currentIndex() ) );
    ui->Graph->setScaledContents( true );
    ui->Graph->setPixmap( pixmap );
    ui->Graph->setMinimumSize( 10, 10 );
}

void MainWindow::on_a_users_triggered()
{
    ShowChildWindow( ChildPtr( new Users() ) );
}
void MainWindow::on_a_options_triggered()
{
    ShowChildWindow( ChildPtr( new settings_wnd() ) );
}
void MainWindow::on_a_proto_triggered()
{
    ShowChildWindow( ChildPtr( new Viewer() ) );
}

ControlsUpdater::ControlsUpdater():
    mStopSignal(false)
{}
void ControlsUpdater::run()
{
    mStopSignal = false;
    while ( !mStopSignal )
    {
//        cpu::CpuMemory::Instance().DB50.Read();
//        cpu::CpuMemory::Instance().I1.Read();
        emit update();
        msleep(100);
    }
}
void ControlsUpdater::stop()
{
    mStopSignal = true;
    wait();
}

void MainWindow::on_puUnits_currentIndexChanged(int /*index*/)
{
    RepaintGraph();
}

void MainWindow::on_tuUnits_currentIndexChanged(int /*index*/)
{
    RepaintGraph();
}
