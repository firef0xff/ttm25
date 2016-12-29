#include "main_window.h"
#include "ui_main_window.h"
#include "cpu/cpu_memory.h"
#include "cpu/net_connection.h"

#include "test/test_params.h"
#include "test/impl/work_params.h"

#include <QLabel>
#include <QAbstractButton>

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
#warning TODO Добавить проверки и код для загрузки из файла

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

    params.Frequency( ui->eFRP->text() );
    params.Pressure( ui->ePressure->text() );
    params.PressureSpeed( ui->ePressureSpeed->text() );
    params.Expenditure( ui->eExpenditure->text() );
    params.Volume( ui->eTFV->text() );

}
