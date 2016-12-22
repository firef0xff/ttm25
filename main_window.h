#pragma once
#include <QMainWindow>
#include <memory>
#include <QThread>
#include "test/impl/inside_pressure.h"
#include <settings/strings_source.h>
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
    void on_a_users_triggered();
    void onUpdateControls();
    void RepaintGraph();
    void on_a_options_triggered();
    void on_a_proto_triggered();
    void on_puUnits_currentIndexChanged(int index);
    void on_tuUnits_currentIndexChanged(int index);
    void on_bTitleTire_clicked();
    void on_bTitleModel_clicked();
    void on_bMark_clicked();
    void on_bTitleKKT_clicked();

    //кнопки управления
    void on_bPumpOnOff_clicked(bool checked);
    void on_bResetOnOff_clicked(bool checked);
    void on_bFeedOnOff_clicked(bool checked);
    void on_bAirOnOff_clicked(bool checked);
    void on_bVacuumOnOff_clicked(bool checked);
    void on_bRegulatingClose_clicked(bool checked);
    void on_bRegulatingOpen_clicked(bool checked);

    void onUpdateConnection( bool connected );
private:
    void closeEvent(QCloseEvent *e);
    void showEvent( QShowEvent *e );
    void resizeEvent( QResizeEvent *e );

    void CheckRights();
    void ShowChildWindow( ChildPtr child, bool maximized = false );
    void SourceToControl( QComboBox& combo, app::StringsSource const& source );
    void AddItem( QComboBox& combo, app::StringsSource& source );

    void SynkControls();
    void UpdateMarks();
    void UpdateData();
    void InitUiControls();


    Ui::MainWindow *ui;
    ChildPtr mChildWindow;
    //Поток обновления данных датчиков
    ControlsUpdater Updater;
    test::InsidePressure mTest;
    app::StringsSource mTitleTire;
    app::StringsSource mTitleModel;
    app::StringsSource mMark;
    app::StringsSource mTitleKKT;    
};
