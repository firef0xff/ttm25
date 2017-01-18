#include <main_window.h>
#include "ui_main_window.h"
#include "test/impl/attestation_params.h"
#include "test/impl/attestation_tests.h"
#include "settings/settings.h"

void MainWindow::InitAttestationModule()
{
    auto & wp = test::AttestationParams::Instance();
    wp.AddTest( std::unique_ptr<test::Test>( new test::AttPressure() ) );
    wp.AddTest( std::unique_ptr<test::Test>( new test::AttTime() ) );
    wp.AddTest( std::unique_ptr<test::Test>( new test::AttPressureSpeed() ) );
}

void MainWindow::on_tAttestaion_currentChanged(int index)
{
    on_bTERMINATE_clicked();

    auto & wp = test::AttestationParams::Instance();
    auto it = wp.TestsCase().find( index );
    if ( it != wp.TestsCase().end() )
        wp.TestForExec( *it->second );
}

//управление аттестацией
void MainWindow::on_bAPStart_clicked()
{
    LockSkreen( mdAttestation );
    if (!mWorker)
    {
        auto& params = test::AttestationParams::Instance();
        params.Date( QDateTime::currentDateTime() );
        params.User( app::Settings::Instance().User() );
        mWorker.reset( new Worker() );
        QObject::connect( mWorker.get(), &Worker::to_exec, this, &MainWindow::exec );
        QObject::connect( mWorker.get(), &Worker::done, this, &MainWindow::OnEndTests );
        mWorker->start();
    }
    mWorker->fill();
}
void MainWindow::on_bAPWrite_clicked()
{
    auto & wp = test::AttestationParams::Instance();
    auto* test = wp.TestForExec();
    if ( !test || test->ID() != 0 )
        return;

    auto *ptr = static_cast<test::AttPressure*>(test);

    auto& data = ptr->GetData();
    for ( auto i = 0; i < data.size(); ++i )
    {
        test::AttPressure::Data &lnk = data[i];
        if ( lnk.mCurrent )
        {
            auto* item = ui->tblAttPressure->item(i,2);
            if ( !test::ParseValue( lnk.mFact, item->text() ) )
                item->setBackgroundColor(Qt::red);
            else
            {
                item->setBackgroundColor(Qt::green);
                lnk.mCurrent = false;
                if ( i < data.size() - 1 )
                    data[i+1].mCurrent = true;
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
void MainWindow::on_bAPStop_clicked()
{//кнопка стоп
    on_bTERMINATE_clicked();
}
void MainWindow::on_bAPReport_clicked()
{
    on_a_proto_triggered();
}
void MainWindow::on_bAPClear_clicked()
{
    auto & wp = test::AttestationParams::Instance();
    auto* test = wp.TestForExec();
    if ( !test || test->ID() != 0 )
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

//обновление данных на вкладке
namespace
{
void UpdatePresureTest( test::AttPressure const& test, Ui::MainWindow *ui )
{
    auto const& data = test.GetData();
    auto* table = ui->tblAttPressure;
    for( size_t it = 0, end = data.size(); it < end; ++it )
    {
        test::AttPressure::Data const& dt = data[it];

        if ( table->rowCount() <= it )
        {
            table->insertRow(table->rowCount());

            std::unique_ptr<QTableWidgetItem> mark( new QTableWidgetItem );
            std::unique_ptr<QTableWidgetItem> i_task( new QTableWidgetItem );
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

            i_task->setText(test::ToString( dt.mTask ));
            i_result->setText(test::ToString( dt.mResult ));
            i_fact->setText(test::ToString( dt.mFact ));

            i_task->setFlags( Qt::ItemIsEnabled );
            i_result->setFlags( Qt::ItemIsEnabled );

            table->setVerticalHeaderItem(table->rowCount() - 1, mark.release());
            table->setItem(table->rowCount() - 1, 0, i_task.release());
            table->setItem(table->rowCount() - 1, 1, i_result.release());
            table->setItem(table->rowCount() - 1, 2, i_fact.release());
        }
        else
        {
            QTableWidgetItem *mark = table->verticalHeaderItem(it);
            QTableWidgetItem *i_task = table->item(it, 0);
            QTableWidgetItem *i_result = table->item(it, 1);
            QTableWidgetItem *i_fact = table->item(it, 2);

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

            i_task->setText(test::ToString( dt.mTask ));
            i_result->setText(test::ToString( dt.mResult ));
            if (!dt.mCurrent)
                i_fact->setText(test::ToString( dt.mFact ));
        }
    }
}
void UpdateTimeTest( test::AttTime const& test, Ui::MainWindow *ui )
{

}
void UpdatePresureSpeedTest( test::AttPressureSpeed const& test, Ui::MainWindow *ui )
{

}

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
        UpdatePresureTest( static_cast<test::AttPressure const&>( *test ), ui );
        break;
    case 1:
        UpdateTimeTest( static_cast<test::AttTime const&>( *test ), ui );
        break;
    case 2:
        UpdatePresureSpeedTest( static_cast<test::AttPressureSpeed const&>( *test ), ui );
        break;
    default:
        break;
    }
}
