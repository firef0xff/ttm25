#include "main_window.h"
#include "ui_main_window.h"
#include "cpu/cpu_memory.h"

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
    //    UpdateData();
}

void MainWindow::SynkControls()
{
    cpu::CpuMemory::Instance().Controls.Read();
    UpdateMarks();
}
void MainWindow::InitUiControls()
{
    SetOnOffText( ui->bRegulatingOnOff, "Регулирующий закрыть", "Регулирующий открыть" );
    SetOnOffText( ui->bUnknownOnOff, "Unknown включить", "Unknown отключить" );
    SetOnOffText( ui->bResetOnOff, "Сброс включить", "Сброс отключить" );
    SetOnOffText( ui->bFeedOnOff, "Подача включить", "Подача отключить" );
    SetOnOffText( ui->bAirOnOff, "Атмосфера включить", "Атмосфера отключить" );
    SetOnOffText( ui->bVacuumOnOff, "Вакуум включить", "Вакуум отключить" );
}

void MainWindow::UpdateMarks()
{
    auto & controls = cpu::CpuMemory::Instance().Controls;
    UpdateButton( ui->bUnknownOnOff,    controls.UnknownOnOff()      );
    UpdateButton( ui->bRegulatingOnOff, controls.RegulatingOnOff()   );
    UpdateButton( ui->bResetOnOff,      controls.ResetOnOff()        );
    UpdateButton( ui->bFeedOnOff,       controls.FeedOnOff()         );
    UpdateButton( ui->bAirOnOff,        controls.AirOnOff()          );
    UpdateButton( ui->bVacuumOnOff,     controls.VacuumOnOff()       );
}

void MainWindow::on_bRegulatingOnOff_clicked(bool checked)
{
//    ChangeName( ui->bRegulatingOnOff, checked );
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    controls.RegulatingOnOff( checked );
    controls.Write();
}
void MainWindow::on_bUnknownOnOff_clicked(bool checked)
{
//    ChangeName( ui->bUnknownOnOff, checked );
    auto & controls = cpu::CpuMemory::Instance().Controls;
    auto lock = controls.Locker();
    controls.UnknownOnOff( checked );
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
