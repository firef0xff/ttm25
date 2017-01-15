#include "main_window.h"
#include "ui_main_window.h"
#include "users/users.h"
#include "settings/settings_wnd.h"
#include "settings/textitem.h"
#include "test/viewer.h"
#include "test/test_params.h"
#include "test/impl/work_params.h"
#include "test/impl/attestation_params.h"
#include "test/impl/main_tests.h"
#include "cpu/cpu_memory.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),  
    mTitleTire( "tires_mark.json" ),
    mTitleModel( "models.json" ),
    mMark( "marks.json" ),
    mTitleKKT( "kkt.json" )
{
    ui->setupUi(this);


    InitTestingModule();
    InitAttestationModule();
    on_tMode_currentChanged( ui->tMode->currentIndex() );

    QObject::connect( &Updater, SIGNAL(update()), this, SLOT(onUpdateControls()) ); 
    Updater.start();



    ui->ePressure->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );
    ui->ePressureSpeed->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );
    ui->eExpenditure->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );
    ui->eTFV->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );
}

MainWindow::~MainWindow()
{
    Updater.stop();
    if ( mWorker )
        mWorker->terminate();

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
    RepaintGraph();
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

//наполнение комбобоксов
void MainWindow::SourceToControl( QComboBox& combo, app::StringsSource const& source )
{
    QString curr_val = combo.currentText();
    combo.clear();
    auto arr = source.Strings();
    for ( auto it = arr.begin(), end = arr.end(); it != end; ++it )
    {
        combo.addItem( *it );
    }
    auto item = combo.findText( curr_val );
    combo.setCurrentIndex( item );
}
void MainWindow::on_bTitleTire_clicked()
{
    AddItem( *ui->eTitleTire, mTitleTire );
}
void MainWindow::on_bTitleModel_clicked()
{
    AddItem( *ui->eTitleModel, mTitleModel);
}

void MainWindow::AddItem( QComboBox& combo, app::StringsSource& source )
{
    ShowChildWindow( ChildPtr( new TextItem(
    [ &combo, &source, this ]( QString const& item )
    {
        source.Strings().push_back( item );
        source.Save();
        SourceToControl( combo, source );
    }) ) );
}

//смена вкладки
void MainWindow::on_tMode_currentChanged(int index)
{
    on_bTERMINATE_clicked();    
    switch (index)
    {
    case 0:
        test::CURRENT_PARAMS = &test::WorkParams::Instance();
        SynkControls();
        LoadParams();
        break;
    case 1:
        test::CURRENT_PARAMS = &test::AttestationParams::Instance();
        on_tAttestaion_currentChanged( ui->tAttestaion->currentIndex() );
        break;
    default:
        break;
    }
}

//обновление данных контроддера
void MainWindow::onUpdateControls()
{
    UpdateMarks();
    UpdateData();
    RepaintGraph();
    UpdateAttestation();
}

ControlsUpdater::ControlsUpdater():
    mStopSignal(false)
{}
void ControlsUpdater::run()
{
    mStopSignal = false;
    while ( !mStopSignal )
    {
        cpu::CpuMemory::Instance().Controls.Read();
        cpu::CpuMemory::Instance().Sensors.Read();
        emit update();
        msleep(100);
    }
}
void ControlsUpdater::stop()
{
    mStopSignal = true;
    wait();
}
