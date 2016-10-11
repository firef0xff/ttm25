#pragma once
#include <QMainWindow>
#include <memory>
#include <QThread>

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

    void on_a_options_triggered();

    void on_a_proto_triggered();

private:
    void closeEvent(QCloseEvent *e);
    void showEvent( QShowEvent *e );

    void CheckRights();
    void ShowChildWindow( ChildPtr child, bool maximized = false );

    Ui::MainWindow *ui;
    ChildPtr mChildWindow;
    //Поток обновления данных датчиков
    ControlsUpdater Updater;
};
