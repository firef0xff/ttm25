#include "common_data.h"
#include <QJsonObject>
#include <QMessageBox>

namespace test
{

QString ErrMsg()
{
//    auto& errs = cpu::CpuMemory::Instance().DB40;
//    errs.Read();

    QString str_errs;
////    if (errs.sl1)
////        str_errs += "уровень масла в норме\n";
//    if (errs.sl2)
//        str_errs += "пониженный уровень масла\n";
//    if (errs.sl3)
//        str_errs += "аварийный уровень масла\n";
//    if (errs.sl4)
//        str_errs += "верхний уровень перелив\n";
////    if (errs.sl5)
////        str_errs += "нижний уровень перелив\n";
//    if (errs.sp1)
//        str_errs += "фильтр всасывающий М1 загрязнен\n";
//    if (errs.sp2)
//        str_errs += "фильтр всасывающий М2 загрязнен\n";
//    if (errs.sp3)
//        str_errs += "фильтр всасывающий М3 загрязнен\n";
//    if (errs.sp4)
//        str_errs += "фильтр всасывающий М3 загрязнен\n";
//    if (errs.sp5)
//        str_errs += "фильтр всасывающий М4 загрязнен\n";
//    if (errs.sp6)
//        str_errs += "фильтр всасывающий М5 загрязнен\n";
//    if (errs.sp7)
//        str_errs += "фильтр тонкой очистки М1 загрязнен\n";
//    if (errs.sp8)
//        str_errs += "фильтр грубой очистки М1 загрязнен\n";
//    if (errs.sp9)
//        str_errs += "фильтр тонкой очистки М2 загрязнен\n";
//    if (errs.sp10)
//        str_errs += "фильтр грубой очистки М2 загрязнен\n";
//    if (errs.sp11)
//        str_errs += "фильтр тонкой очистки М4 загрязнен\n";
//    if (errs.sp12)
//        str_errs += "фильтр грубой очистки М3 загрязнен\n";
//    if (errs.sp13)
//        str_errs += "фильтр грубой очистки М3 загрязнен\n";
//    if (errs.sp14)
//        str_errs += "фильтр контура охлаждения загрязнен\n";
//    if (errs.sp15)
//        str_errs += "фильтр тонкой очистки М12 загрязнен\n";
//    if (errs.sq1)
//        str_errs += "кран М1 закрыт\n";
//    if (errs.sq3)
//        str_errs += "кран М2 закрыт\n";
//    if (errs.sq5)
//        str_errs += "кран М3 закрыт\n";
//    if (errs.sq7)
//        str_errs += "кран М3 закрыт\n";
//    if (errs.sq9)
//        str_errs += "кран М4 закрыт\n";
//    if (errs.sq11)
//        str_errs += "кран М5 закрыт\n";
//    if (errs.sq13)
//        str_errs += "кран дренажа М1 закрыт\n";
//    if (errs.sq14)
//        str_errs += "кран дренажа М2 закрыт\n";
//    if (errs.sq15)
//        str_errs += "кран дренажа М4 закрыт\n";
//    if (errs.fault_m1)
//        str_errs += "нет включения М1\n";
//    if (errs.fault_m2)
//        str_errs += "нет включения М2\n";
//    if (errs.fault_m3)
//        str_errs += "нет включения М3\n";
//    if (errs.fault_m4)
//        str_errs += "нет включения М4\n";
//    if (errs.fault_m5)
//        str_errs += "нет включения М5\n";
//    if (errs.fault_m6)
//        str_errs += "нет включения М6\n";
//    if (errs.fault_m8)
//        str_errs += "нет включения М8\n";
//    if (errs.fault_m10)
//        str_errs += "нет включения М10\n";
//    if (errs.fault_m12)
//        str_errs += "нет включения М12\n";
//    if (errs.fault_phasing)
//        str_errs += "не правильная фазировка\n";
//    if (errs.p_no)
//        str_errs += "давление в системе не установилось\n";
//    if (errs.p_upr_no)
//        str_errs += "давление в канале Х не установилось\n";
//    if (errs.q_no)
//        str_errs += "расход не установился\n";
//    if (errs.v_no)
//        str_errs += "Напряжение не в допуске\n";
//    if (errs.fault_p_op12)
//        str_errs += "Аварийное давление\n";
//    if (errs.fault_p_op22)
//        str_errs += "Аварийное давление\n";

    return str_errs;
}



TestCommonData::TestCommonData( QString const& name, uint8_t id ):
    test::Test(  name, id )/*,
    mCommand( cpu::CpuMemory::Instance().DB31 )*/
{}

QJsonObject TestCommonData::Serialise() const
{
    QJsonObject obj;
    obj.insert("TestingTime", TestingTime );

    return obj;
}
bool TestCommonData::Deserialize( QJsonObject const& obj )
{
    TestingTime = obj.value("TestingTime").toInt();
    return true;
}

uint8_t TestCommonData::CommandID()
{
    return mId;
}
void TestCommonData::Start()
{
//    mCommand.N_Operation = CommandID();
//    mCommand.Start_Oper = true;
//    mCommand.Stop_Oper = false;
//    mCommand.Nasos_M2 = app::Settings::Instance().MainPupm() == "M2";
//    mCommand.OP15_25_Continum = false;
//    mCommand.Next_Amp = false;
//    mCommand.Write();
    StartTime.start();
}
void TestCommonData::Wait( bool& work, bool& done)
{
    work = false;
    done = false;
    bool started = false;
    while( !done || !started )
    {
        if (work)
            started = true;
        UpdateData();
        if ( IsStopped() )
        {
//            mCommand.Start_Oper = false;
//            mCommand.Stop_Oper = true;
//            mCommand.Nasos_M2 = app::Settings::Instance().MainPupm() == "M2";
//            mCommand.OP15_25_Continum = false;
//            mCommand.Next_Amp = false;
//            mCommand.Write();
            Log( "Испытание прервано" );
            return;
        }
    }
    TestingTime = StartTime.elapsed()/1000;
    *mStopMarker = !CheckErrors();
}
bool TestCommonData::CheckErrors()
{
//    auto& mem = cpu::CpuMemory::Instance().M1;

    QString str_errs = ErrMsg();

    if ( str_errs.isEmpty() )
        return true;

    std::mutex mutex;
    std::unique_lock< std::mutex > lock( mutex );
    Launcher( std::bind( &TestCommonData::ShowErrors, this, str_errs ) );

    mCondVar.wait( lock );
//    mem.SetKvitir_Osch( true );
    return false;
}
void TestCommonData::ShowErrors( QString const& err )
{
    QMessageBox msg;
    msg.setWindowTitle( "Ошибки тестирования" );
    msg.setText( err.left(200) + "..." );
    msg.addButton( QMessageBox::Ok );
    msg.setModal( true );
    msg.exec();
    mCondVar.notify_one();
}

}
