#include "main_window.h"
#include "ui_main_window.h"
#include "cpu/cpu_memory.h"
#include "cpu/net_connection.h"

#include "test/test_params.h"
#include "test/impl/work_params.h"
#include "settings/settings.h"

#include <QLabel>
#include <QAbstractButton>
#include <QFileDialog>

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

namespace
{
void ChangeName( QAbstractButton *btn, bool checked )
{
    if ( checked )
        btn->setText( btn->property("off").toString() );
    else
        btn->setText( btn->property("on").toString() );
}

void UpdateButton( QAbstractButton *btn, bool checked )
{
    btn->setChecked( checked );
    ChangeName( btn, checked );
}
void UpdateMark  ( QLabel *btn, bool draw, QColor color )
{
    QPalette palette = btn->palette();

    if ( draw )
    {
        palette.setColor( btn->backgroundRole(), color );
        btn->setAutoFillBackground( true );
    }
    else
    {
        btn->setAutoFillBackground( false );
    }
    btn->setPalette( palette );
}

void SetOnOffText( QAbstractButton *btn, QString on, QString off )
{
    btn->setProperty( "on", on );
    btn->setProperty( "off", off );
}

}

void MainWindow::onUpdateControls()
{
    UpdateMarks();
    UpdateData();
}

void MainWindow::SynkControls()
{
    cpu::CpuMemory::Instance().Controls.Read();
    UpdateMarks();
}
void MainWindow::InitUiControls()
{
    SetOnOffText( ui->bRegulatingClose, "Регулирующий закрыть", "Регулирующий остановить" );
    SetOnOffText( ui->bRegulatingOpen, "Регулирующий открыть", "Регулирующий остановить" );
    SetOnOffText( ui->bPumpOnOff, "Насос включить", "Насос отключить" );
    SetOnOffText( ui->bResetOnOff, "Сброс включить", "Сброс отключить" );
    SetOnOffText( ui->bFeedOnOff, "Подача включить", "Подача отключить" );
    SetOnOffText( ui->bAirOnOff, "Атмосфера включить", "Атмосфера отключить" );
    SetOnOffText( ui->bVacuumOnOff, "Вакуум включить", "Вакуум отключить" );

    QObject::connect( &cpu::NetConnection::Communicator(), SIGNAL(connected(bool)), this, SLOT(onUpdateConnection(bool)) );
}
void MainWindow::onUpdateConnection( bool connected )
{
    UpdateMark( ui->lConnect, true , !connected ? Qt::red : Qt::green );
}

void MainWindow::UpdateMarks()
{
    auto & controls = cpu::CpuMemory::Instance().Controls;
    UpdateButton( ui->bPumpOnOff,       controls.PumpOnOff()         );
    UpdateButton( ui->bRegulatingOpen,  controls.RegulatingOpen()    );
    UpdateButton( ui->bRegulatingClose, controls.RegulatingClose()   );
    UpdateButton( ui->bResetOnOff,      controls.ResetOnOff()        );
    UpdateButton( ui->bFeedOnOff,       controls.FeedOnOff()         );
    UpdateButton( ui->bAirOnOff,        controls.AirOnOff()          );
    UpdateButton( ui->bVacuumOnOff,     controls.VacuumOnOff()       );
}
void MainWindow::UpdateData()
{
    auto & sensors = cpu::CpuMemory::Instance().Sensors;
    QString text;
    text.sprintf("%07.2f", sensors.Pressure());
    ui->lPressure->setText( text );
    text.clear();
    text.sprintf("%07.2f", sensors.Volume());
    ui->lVolume->setText( text );
    text.clear();
    text.sprintf("%07.2f", sensors.Expenditure());
    ui->lExpenditure->setText( text );
    text.clear();
    text.sprintf("%07.0f", sensors.Time());
    ui->lTime->setText( text );
}

void MainWindow::on_bRegulatingClose_clicked(bool checked)
{
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    if ( checked )
        controls.RegulatingOpen( false );
    controls.RegulatingClose( checked );
    controls.Write();
}
void MainWindow::on_bRegulatingOpen_clicked(bool checked)
{
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    if ( checked )
        controls.RegulatingClose( false );
    controls.RegulatingOpen( checked );
    controls.Write();
}
void MainWindow::on_bPumpOnOff_clicked(bool checked)
{
//    ChangeName( ui->bUnknownOnOff, checked );
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    controls.PumpOnOff( checked );
    controls.Write();
}
void MainWindow::on_bResetOnOff_clicked(bool checked)
{
//    ChangeName( ui->bResetOnOff, checked );
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    controls.ResetOnOff( checked );
    controls.Write();
}
void MainWindow::on_bFeedOnOff_clicked(bool checked)
{
//    ChangeName( ui->bFeedOnOff, checked );
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    controls.FeedOnOff( checked );
    controls.Write();
}
void MainWindow::on_bAirOnOff_clicked(bool checked)
{
//    ChangeName( ui->bAirOnOff, checked );
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    controls.AirOnOff( checked );
    controls.Write();
}
void MainWindow::on_bVacuumOnOff_clicked(bool checked)
{
//    ChangeName( ui->bVacuumOnOff, checked );
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    controls.VacuumOnOff( checked );
    controls.Write();
}


//управление параметрами
void MainWindow::on_bParams_clicked()
{
    SaveParams();
    test::CURRENT_PARAMS->WriteToController();
}

void MainWindow::SaveParams()
{
    auto ParamChecker = []( QWidget* control, bool r ) -> bool
    {
        QPalette palette = control->palette();

        if ( !r )
        {
            palette.setColor( control->backgroundRole(), Qt::red );
            control->setAutoFillBackground( true );
        }
        else
        {
            control->setAutoFillBackground( false );
        }
        control->setPalette( palette );
        return r;
    };
    auto ValidateRange = []( QLineEdit* control, bool r ) -> bool
    {
        QString text = control->text();
        int pos = control->cursorPosition();
        return std::min( control->validator()->validate( text, pos ) == QValidator::Acceptable , r );
    };

    auto &params = test::WorkParams::Instance();
    test::CURRENT_PARAMS = &params;

    params.Model( ui->eTitleModel->currentText() );
    params.Size( ui->eTitleTire->currentText() );
    params.Customer( ui->eCustomer->text() );
    params.OrderNo( ui->eOrderNumber->text() );
    params.TireNo( ui->eSerialNumber->text() );

    params.BreakPressure( ui->eBreakPressure->text() );
    params.ConstPressureTime( ui->eConstPressureTime->text() );

    params.Date( QDateTime::currentDateTime() );
    params.User( app::Settings::Instance().User() );

    params.Frequency( ui->eFRP->text() );

    bool res = true;
    res *= ParamChecker( ui->lPressure_2,    ValidateRange( ui->ePressure, params.Pressure( ui->ePressure->text() ) ) );
    res *= ParamChecker( ui->lPressureSpeed, ValidateRange( ui->ePressureSpeed,params.PressureSpeed( ui->ePressureSpeed->text() ) ) );
    res *= ParamChecker( ui->lExpenditure_2, ValidateRange( ui->eExpenditure,params.Expenditure( ui->eExpenditure->text() ) ) );
    res *= ParamChecker( ui->lTFV,           ValidateRange( ui->eTFV,params.Volume( ui->eTFV->text() ) ) );

}
void MainWindow::LoadParams()
{
    auto &params = test::WorkParams::Instance();
    test::CURRENT_PARAMS = &params;

    ui->eTitleModel->setCurrentIndex( ui->eTitleModel->findText( params.Model() ) );
    ui->eTitleTire->setCurrentIndex( ui->eTitleTire->findText( params.Size() ) );

    ui->eCustomer->setText( params.Customer() );
    ui->eOrderNumber->setText( test::ToString( params.OrderNo() ) );
    ui->eSerialNumber->setText( params.TireNo() );

    ui->eBreakPressure->setText( test::ToString( params.BreakPressure() ) );
    ui->eConstPressureTime->setText( test::ToString( params.ConstPressureTime() ) );
    ui->eFRP->setValue( params.Frequency() );

    ui->ePressure->setText( test::ToString( params.Pressure() ) );
    ui->ePressureSpeed->setText( test::ToString( params.PressureSpeed() ) );
    ui->eExpenditure->setText( test::ToString( params.Expenditure() ) );
    ui->eTFV->setText( test::ToString( params.Volume() ) );

    QString method = params.TestForExec()->Name();

    for ( size_t it = 0, size = ui->eTestingMethod->count(); it < size; ++it )
    {
        if ( method == ui->eTestingMethod->itemText( it ) )
        {
            ui->eTestingMethod->setCurrentIndex( it );
            break;
        }
    }
}

void MainWindow::on_aSaveParams_triggered()
{
    QString file_name;
    QFileDialog dlg;
    dlg.setFileMode( QFileDialog::AnyFile );
    dlg.setDirectory( app::Settings::Instance().TestPath() );
    dlg.setNameFilter( "Параметры испытаний (*.isx )" );
    dlg.setAcceptMode( QFileDialog::AcceptSave );
    dlg.setViewMode( QFileDialog::Detail );

    if ( dlg.exec() )
        file_name = dlg.selectedFiles().front();

    if ( !file_name.isEmpty() )
    {
        if ( !file_name.endsWith(".isx", Qt::CaseInsensitive) )
            file_name += ".isx";
        SaveParams();
        test::ParamsToFile( file_name );
    }
}
void MainWindow::on_aLoadParams_triggered()
{
    QString file_name;
    QFileDialog dlg;
    dlg.setFileMode( QFileDialog::ExistingFile );
    dlg.setDirectory( app::Settings::Instance().TestPath() );
    dlg.setNameFilter( "Параметры испытаний (*.isx )" );
    dlg.setViewMode( QFileDialog::Detail );
    if ( dlg.exec() )
        file_name = dlg.selectedFiles().front();

    test::CURRENT_PARAMS = test::ParamsFromFile( file_name );
    LoadParams();
}

void MainWindow::on_aSaveResults_triggered()
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
void MainWindow::on_aLoadResults_triggered()
{
    QString file_name;
    QFileDialog dlg;
    dlg.setFileMode( QFileDialog::ExistingFile );
    dlg.setDirectory( app::Settings::Instance().TestPath() );
    dlg.setNameFilter( "Результаты испытаний (*.res )" );
    dlg.setViewMode( QFileDialog::Detail );
    if ( dlg.exec() )
        file_name = dlg.selectedFiles().front();

    test::DataFromFile( file_name );
    RepaintGraph();
    LoadParams();
    on_a_proto_triggered();
}

//управление испытаниями
void MainWindow::on_bFill_clicked()
{
    if (!mWorker)
    {
        SaveParams();
        mWorker.reset( new Worker() );
        QObject::connect( mWorker.get(), &Worker::to_exec, this, &MainWindow::exec );
        mWorker->start();
    }
    mWorker->resume_fill();
}
void MainWindow::on_bStart_clicked()
{
    if (!mWorker)
    {
        SaveParams();
        mWorker.reset( new Worker() );
        mWorker->start();
    }
    mWorker->resume_test();
}
void MainWindow::on_bStop_clicked()
{
    if (mWorker)
        mWorker->stop();
}
void MainWindow::on_bTERMINATE_clicked()
{
    if (mWorker)
    {
        mWorker->terminate();
        QObject::disconnect( mWorker.get(), &Worker::to_exec, this, &MainWindow::exec );
        mWorker.reset();
    }
}
void MainWindow::on_bResults_clicked()
{
    on_a_proto_triggered();
}

void MainWindow::exec( Functor func )
{
    if ( func )
        func();
}
