#include <main_window.h>
#include "ui_main_window.h"
#include "test/impl/attestation_params.h"
#include "test/impl/attestation_tests.h"
#include "settings/settings.h"

void MainWindow::InitAttestationModule()
{
    auto & wp = test::AttestationParams::Instance();
    wp.AddTest( std::unique_ptr<test::Test>( new test::AttTime() ) );
    wp.AddTest( std::unique_ptr<test::Test>( new test::AttPressure() ) );
    wp.AddTest( std::unique_ptr<test::Test>( new test::AttPressureSpeed() ) );
    wp.AddTest( std::unique_ptr<test::Test>( new test::AttPressureTime() ) );
    ui->ProtoDate->setDate( QDate::currentDate() );
    ui->ProtoNo->setText( QString::number(1) );
}

void MainWindow::on_tAttestaion_currentChanged(int index)
{
//    QWidget *ptr = ui->tAttestaion->tabBar() ;
//    QPalette pal = ptr->palette();
//    pal.setColor(ptr->backgroundRole(), Qt::yellow);
//    ptr->setPalette(pal);
//    ptr->setAutoFillBackground(true);

//    QPalette pal = ui->tAttestaion->tabBar()->palette();
////        pal.setColor(QPalette::Window, QColor(255,0,0));
//        pal.setColor(QPalette::Inactive, QPalette::Button, QColor(255,255,0));
////        pal.setColor(QPalette::WindowText, QColor(0,0,255));
////        pal.setColor(QPalette::ButtonText, QColor(255,0,255));
////        pal.setColor(QPalette::Base, QColor(0, 200, 0));
////        pal.setColor(QPalette::AlternateBase, QColor(0, 255, 255));
////        pal.setColor(QPalette::Text, Qt::white);
//        ui->tAttestaion->tabBar()->setPalette(pal);

    for ( auto i = 0; i < ui->tAttestaion->tabBar()->count(); ++i )
    {
        QColor c = i == index ? Qt::red : Qt::black;
        ui->tAttestaion->tabBar()->setTabTextColor(i, c);
    }

    auto & wp = test::AttestationParams::Instance();
    auto it = wp.TestsCase().find( index );
    if ( it != wp.TestsCase().end() )
        wp.TestForExec( *it->second );
}

//управление аттестацией
void MainWindow::SetProtoParams()
{
    auto& params = test::AttestationParams::Instance();
    params.ProtoDate( ui->ProtoDate->date().toString( "dd.MM.yyyy" ) );
    params.ProtoNo( ui->ProtoNo->text() );

}
void MainWindow::RunAttestation()
{
    LockSkreen( mdAttestation );
    auto& params = test::AttestationParams::Instance();
    params.Date( QDateTime::currentDateTime() );
    params.User( app::Settings::Instance().User() );
    SetProtoParams();

    mWorker.reset( new Worker() );
    QObject::connect( mWorker.get(), &Worker::to_exec, this, &MainWindow::exec );
    QObject::connect( mWorker.get(), &Worker::done, this, &MainWindow::OnEndTests );
    mWorker->start();

    mWorker->fill();
}
void MainWindow::on_bAPStart_clicked()
{
    on_bAPClear_clicked();

    auto& params = test::AttestationParams::Instance();
    params.PressureSpeed( ui->cbPressureSpeed->itemText(1) );

    RunAttestation();
}

void MainWindow::on_bAPStop_clicked()
{//кнопка стоп
    if (mWorker)
    {
        mWorker->terminate();
    }
}
void MainWindow::on_bAPReport_clicked()
{
    SetProtoParams();
    on_a_proto_triggered();
}
void MainWindow::on_bAPClear_clicked()
{
    auto & wp = test::AttestationParams::Instance();
    auto* test = wp.TestForExec();
    if ( !test || test->ID() != 1 )
        return;

    on_bAPStop_clicked();

    auto *ptr = static_cast<test::AttPressure*>(test);
    ptr->Reset();
    auto* table = ui->tblAttPressure;

    // Deselects all selected items
    table->clearSelection();
    table->disconnect();
    table->clearContents();
    table->setRowCount(0);
}
void MainWindow::on_bAPTerm_clicked()
{
    on_bAPStop_clicked();
    auto &mem = cpu::CpuMemory::Instance().LaunchControl;
    auto lock = mem.Locker();
    mem.Read();
    mem.Terminated(true);
    mem.Write();
}

void MainWindow::on_bATStart_clicked()
{
    on_bATClear_clicked();

    RunAttestation();
}
void MainWindow::on_bATWrite_clicked()
{
    auto & wp = test::AttestationParams::Instance();
    auto* test = wp.TestForExec();
    if ( !test || test->ID() != 0 )
        return;

    auto *ptr = static_cast<test::AttTime*>(test);

    auto& data = ptr->GetData();

    for ( auto i = 0; i < data.size(); ++i )
    {
        test::AttTime::Data &lnk = data[i];
        if ( lnk.mCurrent )
        {
            auto* item = ui->tblAttTime->item(i,1);
            if ( !test::ParseValue( lnk.mResult, item->text() ) )
                item->setBackgroundColor(Qt::red);
            else
            {
                item->setBackgroundColor(Qt::green);
                break;
            }
        }
    }

    //кнопка записать
    if (mWorker)
    {
        mWorker->test();
    }
}
void MainWindow::on_bATStop_clicked()
{
    on_bAPStop_clicked();
}
void MainWindow::on_bATReport_clicked()
{
    SetProtoParams();
    on_a_proto_triggered();
}
void MainWindow::on_bATClear_clicked()
{
    auto & wp = test::AttestationParams::Instance();
    auto* test = wp.TestForExec();
    if ( !test || test->ID() != 0 )
        return;

    on_bATStop_clicked();

    auto *ptr = static_cast<test::AttPressure*>(test);
    ptr->Reset();
    auto* table = ui->tblAttTime;

    // Deselects all selected items
    table->clearSelection();
    table->disconnect();
    table->clearContents();
    table->setRowCount(0);
}

void MainWindow::on_bAPTStart_clicked()
{
    auto& params = test::AttestationParams::Instance();
    params.PressureSpeed( ui->cbPressureSpeed->currentText() );
    params.UpdatePeriod( ui->bxPeriod->value() );

    RunAttestation();
}
void MainWindow::on_bAPTStop_clicked()
{
    on_bAPStop_clicked();
}
void MainWindow::on_bAPTReport_clicked()
{
    SetProtoParams();
    on_a_proto_triggered();
}
void MainWindow::on_bAPTClear_clicked()
{
    auto & wp = test::AttestationParams::Instance();
    auto* test = wp.TestForExec();
    if ( !test || test->ID() != 2 )
        return;

    on_bAPTStop_clicked();

    auto *ptr = static_cast<test::AttPressureSpeed*>(test);
    ptr->Reset();
    auto* table = ui->tblAttPressureSpeed;

    // Deselects all selected items
    table->clearSelection();
    table->disconnect();
    table->clearContents();
    table->setRowCount(0);
}
void MainWindow::on_bAPTTerm_clicked()
{
    on_bAPTerm_clicked();
}

void MainWindow::on_bAPPStart_clicked()
{
    on_bAPPClear_clicked();

    auto& params = test::AttestationParams::Instance();
    params.PressureSpeed( ui->cbPressureSpeed->itemText(0) );

    RunAttestation();
}
void MainWindow::on_bAPPStop_clicked()
{
    on_bAPStop_clicked();
}
void MainWindow::on_bAPPReport_clicked()
{
    SetProtoParams();
    on_a_proto_triggered();
}
void MainWindow::on_bAPPClear_clicked()
{
    auto & wp = test::AttestationParams::Instance();
    auto* test = wp.TestForExec();
    if ( !test || test->ID() != 3 )
        return;

    on_bAPPStop_clicked();

    auto *ptr = static_cast<test::AttPressureTime*>(test);
    ptr->Reset();
    auto* table = ui->tblAttPressureTime;

    // Deselects all selected items
    table->clearSelection();
    table->disconnect();
    table->clearContents();
    table->setRowCount(0);
}
void MainWindow::on_bAPPTerm_clicked()
{
    on_bAPTerm_clicked();
}

//обновление данных на вкладке
namespace
{
void RepaintGraph( test::Attestaion const& test, QLabel *label )
{
    QPixmap pixmap( label->width()-2, label->height()-2 );
    QPainter painter(&pixmap);
    QFont font = painter.font();
    font.setFamily("Arial");
    font.setPointSize(12);
    auto r = pixmap.rect();
    painter.fillRect( r, Qt::white );

    test.PaintGraph( painter, font, pixmap.rect(), 1 );

    label->setScaledContents( true );
    label->setPixmap( pixmap );
    label->setMinimumSize( 10, 10 );
}
void UpdatePresureTest( test::AttPressure const& test, Ui::MainWindow *ui )
{
    RepaintGraph( test, ui->lAPGraph );
    auto const& data = test.GetData();
    auto* table = ui->tblAttPressure;
    for( size_t it = 0, end = data.size(); it < end; ++it )
    {
        test::AttPressure::Data const& dt = data[it];

        if ( static_cast<size_t>( table->rowCount() ) <= it )
        {
            table->insertRow(table->rowCount());

            std::unique_ptr<QTableWidgetItem> mark( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_task( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_result( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_fact( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_error( new QTableWidgetItem );

            if (dt.mCurrent)
            {
                mark->setText("*");                
            }
            else
            {
                mark->setText("");                
            }

            i_task->setText(test::ToString( dt.mTask ));
            i_result->setText(test::ToString( dt.mResult ));
            i_fact->setText(test::ToString( dt.mFact ));
            i_error->setText(test::ToString( dt.Error() ));

            i_task->setFlags( Qt::ItemIsEnabled );
            i_result->setFlags( Qt::ItemIsEnabled );
            i_fact->setFlags( Qt::ItemIsEnabled );
            i_error->setFlags( Qt::ItemIsEnabled );

            table->setVerticalHeaderItem(table->rowCount() - 1, mark.release());
            table->setItem(table->rowCount() - 1, 0, i_task.release());
            table->setItem(table->rowCount() - 1, 1, i_result.release());
            table->setItem(table->rowCount() - 1, 2, i_fact.release());
            table->setItem(table->rowCount() - 1, 3, i_error.release());
        }
        else
        {
            QTableWidgetItem *mark = table->verticalHeaderItem(it);
            QTableWidgetItem *i_task = table->item(it, 0);
            QTableWidgetItem *i_result = table->item(it, 1);
            QTableWidgetItem *i_fact = table->item(it, 2);
            QTableWidgetItem *i_error = table->item(it, 3);

            if (dt.mCurrent)
            {
                mark->setText("*");
            }
            else
            {
                mark->setText("");
            }

            i_task->setText(test::ToString( dt.mTask ));
            i_result->setText(test::ToString( dt.mResult ));
            i_fact->setText(test::ToString( dt.mFact ));
            i_error->setText(test::ToString( dt.Error() ));
        }
    }
}
void UpdateTimeTest( test::AttTime const& test, Ui::MainWindow *ui )
{
    RepaintGraph( test, ui->lATGraph );
    auto const& data = test.GetData();
    auto* table = ui->tblAttTime;

    while ( data.size() < table->rowCount() )
    {
        table->removeRow( table->rowCount() - 1 );
    }
    for( size_t it = 0, end = data.size(); it < end; ++it )
    {
        test::AttTime::Data const& dt = data[it];

        if ( static_cast<size_t>( table->rowCount() ) <= it )
        {
            table->insertRow(table->rowCount());

            std::unique_ptr<QTableWidgetItem> mark( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_result( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_fact( new QTableWidgetItem );

            if (dt.mCurrent)
            {
                mark->setText("*");
                i_fact->setFlags( Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable );
            }
            else
            {
                mark->setText("");
                i_fact->setFlags( Qt::ItemIsEnabled );
            }

            i_result->setText(test::ToString( dt.mCpuTime ));
            i_fact->setText(test::ToString( dt.mResult ));

            i_result->setFlags( Qt::ItemIsEnabled );


            table->setVerticalHeaderItem(table->rowCount() - 1, mark.release());
            table->setItem(table->rowCount() - 1, 0, i_result.release());
            table->setItem(table->rowCount() - 1, 1, i_fact.release());
        }
        else
        {
            QTableWidgetItem *mark = table->verticalHeaderItem(it);
            QTableWidgetItem *i_result = table->item(it, 0);
            QTableWidgetItem *i_fact = table->item(it, 1);

            if (dt.mCurrent)
            {
                mark->setText("*");
                Qt::ItemFlags f = Qt::ItemIsEnabled|Qt::ItemIsEditable|Qt::ItemIsSelectable;
                if ( i_fact->flags() != f )
                    i_fact->setFlags( f );
            }
            else
            {
                mark->setText("");
                i_fact->setFlags( Qt::ItemIsEnabled );
            }
            i_result->setText(test::ToString( dt.mCpuTime ));
            if (!dt.mCurrent)
                i_fact->setText(test::ToString( dt.mResult ));
        }
    }
}
void UpdatePresureSpeedTest( test::AttPressureSpeed const& test, Ui::MainWindow *ui )
{
    RepaintGraph( test, ui->lAPTGraph );

    auto const& data = test.GetData();
    auto* table = ui->tblAttPressureSpeed;

    for( size_t it = 0, end = data.size(); it < end; ++it )
    {
        test::AttPressureSpeed::Data const& dt = data[it];

        double speed = it == 0 ? 0 : dt.Speed(data[it-1]);
        if ( static_cast<size_t>( table->rowCount() ) <= it )
        {
            table->insertRow(table->rowCount());

            std::unique_ptr<QTableWidgetItem> i_result( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_fact( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_speed( new QTableWidgetItem );


            i_result->setText(test::ToString( dt.mCpuTime ));
            i_fact->setText(test::ToString( dt.mResult ));
            i_speed->setText(test::ToString( speed ) );

            i_result->setFlags( Qt::ItemIsEnabled );
            i_fact->setFlags( Qt::ItemIsEnabled );
            i_speed->setFlags( Qt::ItemIsEnabled );

            table->setItem(table->rowCount() - 1, 0, i_result.release());
            table->setItem(table->rowCount() - 1, 1, i_fact.release());
            table->setItem(table->rowCount() - 1, 2, i_speed.release());
        }
        else
        {
            QTableWidgetItem *i_result = table->item(it, 0);
            QTableWidgetItem *i_fact = table->item(it, 1);
            QTableWidgetItem *i_speed = table->item(it, 2);

            i_result->setText(test::ToString( dt.mCpuTime ));
            i_fact->setText(test::ToString( dt.mResult ) );

            i_speed->setText(test::ToString( speed ));
        }
    }
}
void UpdatePresureTimeTest( test::AttPressureTime const& test, Ui::MainWindow *ui )
{
    RepaintGraph( test, ui->lAPPGraph );
    auto const& data = test.GetData();
    auto* table = ui->tblAttPressureTime;

    for( size_t it = 0, end = data.size(); it < end; ++it )
    {
        test::AttPressureTime::Data const& dt = data[it];

        if ( static_cast<size_t>( table->rowCount() ) <= it )
        {
            table->insertRow(table->rowCount());

            std::unique_ptr<QTableWidgetItem> i_task( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_vpd( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_fact( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_error( new QTableWidgetItem );


            i_task->setText(test::ToString( dt.mPtask ));
            i_vpd->setText(test::ToString( dt.mPvpd ));
            i_fact->setText(test::ToString( dt.mPmax ));
            i_error->setText(test::ToString( dt.Error() ));

            i_task->setFlags( Qt::ItemIsEnabled );
            i_fact->setFlags( Qt::ItemIsEnabled );
            i_error->setFlags( Qt::ItemIsEnabled );

            table->setItem(table->rowCount() - 1, 0, i_task.release());
            table->setItem(table->rowCount() - 1, 1, i_vpd.release());
            table->setItem(table->rowCount() - 1, 2, i_fact.release());
            table->setItem(table->rowCount() - 1, 3, i_error.release());
        }
        else
        {
            QTableWidgetItem *i_task = table->item(it, 0);
            QTableWidgetItem *i_vpd = table->item(it, 1);
            QTableWidgetItem *i_fact = table->item(it, 2);
            QTableWidgetItem *i_error = table->item(it, 3);

            i_task->setText(test::ToString( dt.mPtask ));
            i_vpd->setText(test::ToString( dt.mPvpd ));
            i_fact->setText(test::ToString( dt.mPmax ));
            i_error->setText(test::ToString( dt.Error() ));
        }
    }
}

}
void MainWindow::ClearAttestation()
{
    auto Clear = []( QTableWidget * table )
    {
        // Deselects all selected items
        table->clearSelection();
        table->disconnect();
        table->clearContents();
        table->setRowCount(0);
    };
    Clear( ui->tblAttPressureTime );
    Clear( ui->tblAttPressureSpeed );
    Clear( ui->tblAttTime );
    Clear( ui->tblAttPressure );
}
void MainWindow::UpdateAttestation()
{
    auto & wp = test::AttestationParams::Instance();
    auto* test = wp.TestForExec();
    if (!test)
        return;

    switch (test->ID())
    {
    case 0:
        UpdateTimeTest( static_cast<test::AttTime const&>( *test ), ui );
        break;
    case 1:
        UpdatePresureTest( static_cast<test::AttPressure const&>( *test ), ui );
        break;
    case 2:
        UpdatePresureSpeedTest( static_cast<test::AttPressureSpeed const&>( *test ), ui );
        break;
    case 3:
        UpdatePresureTimeTest( static_cast<test::AttPressureTime const&>( *test ), ui );
        break;
    default:
        break;
    }
}
