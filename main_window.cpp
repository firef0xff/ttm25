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
    qRegisterMetaType< Functor >();
    ui->setupUi(this);


    InitTestingModule();
    InitAttestationModule();
    on_tMode_currentChanged( ui->tMode->currentIndex() );

    QObject::connect( &Updater, SIGNAL(update()), this, SLOT(onUpdateControls()) ); 
    Updater.start();

    ui->eConstPressureTime->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );
    ui->ePressure->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );
    ui->ePressureSpeed->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );
    ui->eExpenditure->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );
    ui->eTFV->setValidator( new QDoubleValidator( INT32_MIN, INT32_MAX , 2, this ) );

    LockSkreen( mdNone );
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
    ui->tab_4->setEnabled(true);
    ui->tMode->tabBar()->setVisible(true);
    ui->menu_2->setEnabled(true);

    if ( app::Settings::Instance().UserAccess() != app::Admin )
    {
        ui->menu_2->setEnabled(false);
        if ( app::Settings::Instance().UserAccess() != app::Master )
        {
            ui->tab_4->setEnabled(false);
            ui->tMode->tabBar()->setVisible(false);
        }           
    }    
}

void MainWindow::ShowChildWindow( ChildPtr child, bool maximized )
{
    mChildWindow.reset( child.release() );

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
    cpu::CpuMemory::Instance().LaunchControl.Reset();
    cpu::CpuMemory::Instance().LaunchControl.Write();
    cpu::CpuMemory::Instance().AttestationLaunchControls.Reset();
    cpu::CpuMemory::Instance().AttestationLaunchControls.Write();
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

void MainWindow::LockSkreen( Mode m )
{
    switch (m)
    {
    case mdNone:
    {
        //menu
        ui->aLoadParams->setEnabled( true );
        ui->aLoadResults->setEnabled( true );
        //ручное управление
        ui->cbManualControl->setEnabled( true );
        ui->bPumpOnOff->setEnabled( false );
        ui->bParams->setEnabled( false );
        ui->bFeedOnOff->setEnabled( false );
        ui->bAirOnOff->setEnabled( false );
        ui->bResetOnOff->setEnabled( false );
        ui->bVacuumOnOff->setEnabled( false );
        ui->bRegulatingOpen->setEnabled( false );
        ui->bRegulatingClose->setEnabled( false );
        ui->bBuzen->setEnabled( false );

        //управление автотестами
        ui->bFill->setEnabled( true );
        ui->bStart->setEnabled( true );
        ui->bStop->setEnabled( true );
        ui->bResults->setEnabled( true );

        //часть настроек
        ui->eTitleTire->setEnabled( true );
        ui->bTitleTire->setEnabled( true );
        ui->eTitleModel->setEnabled( true );
        ui->bTitleModel->setEnabled( true );
        ui->eTestingMethod->setEnabled( true );
        ui->eCustomer->setEnabled( true );
        ui->eOrderNumber->setEnabled( true );
        ui->eSerialNumber->setEnabled( true );
        ui->eFRP->setEnabled( false );

        ui->eBreakPressure->setEnabled( true );
        ui->eConstPressureTime->setEnabled( true );
        ui->ePressure->setEnabled( true );
        ui->ePressureSpeed->setEnabled( true );
        ui->eExpenditure->setEnabled( true );
        ui->eTFV->setEnabled( true );

        //замораживаем навигацию по вкладкам
        ui->tMode->tabBar()->setEnabled(true);
        ui->tAttestaion->tabBar()->setEnabled(true);

        break;
    }
    case mdManual:
    {
        //меню
        ui->aLoadResults->setEnabled( false );
        //ручное управление
        ui->cbManualControl->setEnabled( true );
        ui->bPumpOnOff->setEnabled( true );
        ui->bParams->setEnabled( true );
        ui->bFeedOnOff->setEnabled( true );
        ui->bAirOnOff->setEnabled( true );
        ui->bResetOnOff->setEnabled( true );
        ui->bVacuumOnOff->setEnabled( true );
        ui->bRegulatingOpen->setEnabled( true );
        ui->bRegulatingClose->setEnabled( true );
        ui->bBuzen->setEnabled( true );
        //управление автотестами
        ui->bFill->setEnabled( false );
        ui->bStart->setEnabled( false );
        ui->bStop->setEnabled( false );
        ui->bResults->setEnabled( false );

        //часть настроек
        ui->eTitleTire->setEnabled( false );
        ui->bTitleTire->setEnabled( false );
        ui->eTitleModel->setEnabled( false );
        ui->bTitleModel->setEnabled( false );
        ui->eTestingMethod->setEnabled( false );
        ui->eCustomer->setEnabled( false );
        ui->eOrderNumber->setEnabled( false );
        ui->eSerialNumber->setEnabled( false );
        ui->eFRP->setEnabled( true );

        //замораживаем навигацию по вкладкам
        ui->tMode->tabBar()->setEnabled(false);
        break;
    }
    case mdAutoTest:
    {
        //меню
        ui->aLoadParams->setEnabled( false );
        ui->aLoadResults->setEnabled( false );
        //ручное управление
        ui->cbManualControl->setEnabled( false );
        ui->bPumpOnOff->setEnabled( false );
        ui->bParams->setEnabled( false );
        ui->bFeedOnOff->setEnabled( false );
        ui->bAirOnOff->setEnabled( false );
        ui->bResetOnOff->setEnabled( false );
        ui->bVacuumOnOff->setEnabled( false );
        ui->bRegulatingOpen->setEnabled( false );
        ui->bRegulatingClose->setEnabled( false );
        ui->bBuzen->setEnabled( false );

        //настройки
        ui->eTitleTire->setEnabled( false );
        ui->bTitleTire->setEnabled( false );
        ui->eTitleModel->setEnabled( false );
        ui->bTitleModel->setEnabled( false );
        ui->eTestingMethod->setEnabled( false );
        ui->eCustomer->setEnabled( false );
        ui->eOrderNumber->setEnabled( false );
        ui->eSerialNumber->setEnabled( false );

        ui->eBreakPressure->setEnabled( false );
        ui->eConstPressureTime->setEnabled( false );
        ui->eFRP->setEnabled( false );
        ui->ePressure->setEnabled( false );
        ui->ePressureSpeed->setEnabled( false );
        ui->eExpenditure->setEnabled( false );
        ui->eTFV->setEnabled( false );

        //замораживаем навигацию по вкладкам
        ui->tMode->tabBar()->setEnabled(false);
        break;
    }
    case mdAttestation:
    {
        //меню
        ui->aLoadParams->setEnabled( false );
        ui->aLoadResults->setEnabled( false );

        //замораживаем навигацию по вкладкам
        ui->tMode->tabBar()->setEnabled(false);
        ui->tAttestaion->tabBar()->setEnabled(false);
        break;
    }
    }

    CheckRights();
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
        cpu::CpuMemory::Instance().Indication.Read();
        emit update();
        msleep(100);
    }
}
void ControlsUpdater::stop()
{
    mStopSignal = true;
    wait();
}

