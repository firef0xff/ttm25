#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "E:/ttm25/cpu/metran/metran.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    try
    {
        auto &inst = cpu::Metran::Instance();
        inst.Read();
        ui->Pressure->setText( QString::number( inst.Pressure() ) );
    }
    catch( std::exception &e )
    {
        ui->Pressure->setText( e.what() );
    }

}
