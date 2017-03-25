#pragma once
#include <QMainWindow>
#include <memory>
#include <QThread>
#include <settings/strings_source.h>
#include "test/worker.h"
#include <QComboBox>

namespace Ui {
class MainWindow;
}

class ControlsUpdater : public QThread
{
    Q_OBJECT
public:
    ControlsUpdater();
    void run();
    void stop();
private:
    mutable bool mStopSignal;
signals:
    void update();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    typedef std::unique_ptr< QWidget > ChildPtr;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //главное окно
    void on_a_users_triggered();
    void on_a_options_triggered();
    void on_a_proto_triggered();
    void on_bTitleTire_clicked();
    void on_bTitleModel_clicked();
    void on_tMode_currentChanged(int index);//смена вкладок
    void onUpdateControls();

    //вкладка тестирования
    void onUpdateConnection( bool connected );
    //график
    void RepaintGraph();
    void on_puUnits_currentIndexChanged(int index);
    void on_tuUnits_currentIndexChanged(int index);

    //кнопки управления
    void on_bPumpOnOff_clicked(bool checked);
    void on_bResetOnOff_clicked(bool checked);
    void on_bFeedOnOff_clicked(bool checked);
    void on_bAirOnOff_clicked(bool checked);
    void on_bVacuumOnOff_clicked(bool checked);
    void on_bRegulatingClose_clicked(bool checked);
    void on_bRegulatingOpen_clicked(bool checked);
    void on_cbManualControl_clicked(bool checked);

    //параметры
    void on_eTestingMethod_activated(const QString &arg1);
    void on_bParams_clicked();
    void on_aSaveParams_triggered();
    void on_aLoadParams_triggered();

    void on_aSaveResults_triggered();
    void on_aLoadResults_triggered();

    //управление тестом
    void on_bFill_clicked();
    void on_bStart_clicked();
    void on_bStop_clicked();
    void on_bTERMINATE_clicked();
    void on_bResults_clicked();

    //колбяки
    void exec( Functor );
    void OnEndTests();

    //аттестация
    void on_tAttestaion_currentChanged(int index);
    void on_bAPStart_clicked();
    void on_bAPStop_clicked();
    void on_bAPReport_clicked();
    void on_bAPClear_clicked();
    void on_bAPTerm_clicked();


    void on_bATStart_clicked();
    void on_bATWrite_clicked();
    void on_bATStop_clicked();
    void on_bATReport_clicked();
    void on_bATClear_clicked();

    void on_bAPTStart_clicked();
    void on_bAPTStop_clicked();
    void on_bAPTReport_clicked();
    void on_bAPTClear_clicked();
    void on_bAPTTerm_clicked();

private:
    //главное окно
    void closeEvent(QCloseEvent *e);
    void showEvent( QShowEvent *e );
    void resizeEvent( QResizeEvent *e );

    void CheckRights();
    void ShowChildWindow( ChildPtr child, bool maximized = false );
    void SourceToControl( QComboBox& combo, app::StringsSource const& source );
    void AddItem( QComboBox& combo, app::StringsSource& source );

    enum Mode
    {
        mdNone,     //снятие блокировки интрефейса
        mdAutoTest, //запущен тест
        mdManual,   //активирован режим ручного управления
        mdAttestation
    };
    void LockSkreen( Mode m );

    //вкладка тестирования
    void SynkControls();
    void UpdateMarks();
    void UpdateData();
    void InitTestingModule();

    void SaveParams();
    void LoadParams();

    //вкладка аттестации
    void InitAttestationModule();
    void UpdateAttestation();

    Ui::MainWindow *ui;
    ChildPtr mChildWindow;
    //Поток обновления данных датчиков
    ControlsUpdater Updater;   
    //
    std::unique_ptr< Worker > mWorker;
    app::StringsSource mTitleTire;
    app::StringsSource mTitleModel;
    app::StringsSource mMark;
    app::StringsSource mTitleKKT;        
};
