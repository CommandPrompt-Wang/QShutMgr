/*
 *  版权所有(C)2022 Command Prompt
 *本程序为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
 *对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。
 *本程序是基于使用目的而加以发布，然而不负任何担保责任；
 *亦无对适售性或特定目的适用性所为的默示性担保。
 *详情请参照GNU通用公共授权。
 */
#include "mainwindow.h"
#include "chosemachine.h"
#include "logwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h>
#include <shellapi.h>
#include <QSystemTrayIcon>
#include "MyFuncs.h"
#include "uac.h"

#define FULL_CHECKED    2   //button fully checked [√]

int ChosenShutdownType=0;//type
#define SHUTDOWN        1
#define RESTART         2
#define HIBERNATE       3   //使用ShutMgr.Ext
#define SLEEP           4   //同上
#define LOGOUT          5   //同上
#define SHUTDOWN_P      6
#define LOCK_COMPUTER   7
#define ABORT           8

bool MoreOption[10]={0};//options(switches)
#define FORCE           0
#define WAIT            1
#define \
    RESTART_REGISTED    2
#define HYBRID          3
#define TOBIOS          4
#define \
    TO_ADVANCED_STARTUP 5
#define OTHER_MACHINE   6
#define GIVE_REASON     7
#define COMMENT         9

int ReasonNum[2]={0,0};//Major&Minor
#define MAJOR           0
#define MINOR           1

//针对combox_ReasonType
#define PLANNED         0
#define OUT_OF_PLAN     1
#define USER_DEFINED    2

#define MAX_COMMAND_CHAR 114514 //用于StatusStr(sprintf_s的要求)

int Time_To_Wait=30;

QList<QString> MachineNameList;

//之所以使用静态窗口是因为日志窗口、远程计算机选择窗口不能重复打开
static LogWindow* logWindow;
static ChoseMachine* MachineChoosing;
QSystemTrayIcon *trayIcon;

QString Log;
int logLines=0;

bool EsterEgg=false;

bool AdminRequired;
bool stop_Booting_to_BIOS;

bool DarkModeOn=false;

#define Disable_and_Uncheck(x)  (x)->setEnabled(false); (x)->setChecked(false);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    logWindow=new LogWindow;            //日志窗口
    MachineChoosing=new ChoseMachine;   //计算机选择窗口
    trayIcon=new QSystemTrayIcon(this); //通知
    trayIcon->setIcon(QIcon(":/new/prefix1/resources/Programico.ico"));
    LOG_APPEND("主程序启动");
    QDir::setCurrent(qApp->applicationDirPath());//设置工作目录
    LOG_APPEND("检查扩展...");
    if(MyFuncs::CheckExt()==false)
    {
        LOG_APPEND("警告: 扩展程序 ShutMgr.Ext.exe 不存在或无法访问！");
        trayIcon->show();
        trayIcon->showMessage(tr("警告"),
                             tr("ShutMgr.Ext.exe 不存在或无法访问！\n"
                                "请检查该程序的位置或访问权限"));
        trayIcon->hide();
        ui->Hibernate->setEnabled(false);       //禁用ShutMgr.Ext.exe相关内容
        ui->LockComputer->setEnabled(false);;
        ui->Logout->setEnabled(false);
        ui->Sleep->setEnabled(false);
//        ui->AbortShutdown->setEnabled(false);
    }
    else
        {LOG_APPEND("检测到扩展程序");}
//---------------夜间模式（实验性）------------
    on_DarkModeAuto_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//from https://blog.csdn.net/weixin_40429361/article/details/125785775
std::string Utf8ToGbk(const char* src_str)
{
    std::string result;
    wchar_t* strSrc;
    char* szRes;
    int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    strSrc = new wchar_t[len + 1];
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, strSrc, len);

    len = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
    szRes = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, len, NULL, NULL);
    result = szRes;
    if(strSrc)
        delete[]strSrc;
    if(szRes)
        delete[]szRes;
    return result;
}

void MainWindow::on_Perform_clicked()       //操作执行部分
{
    char StatusStr[MAX_COMMAND_CHAR]="";
    int MachineCount=0;
    std::string command;

    CHECK_ADMIN();

    if(ChosenShutdownType==SHUTDOWN
    or ChosenShutdownType==RESTART
    or ChosenShutdownType==SHUTDOWN_P)    //这三种shutdown.exe -t可以用，-p不需要延迟
        command="shutdown.exe ";
    else                                 //剩下的shutdown.exe -t不行
    {
        if(MyFuncs::CheckExt())         //检查扩展程序
            command="ShutMgr.Ext.exe ";
        else
        {
            LOG_APPEND("ShutMgr.Ext.exe 不存在或无法访问, 停止执行");
            QMessageBox::critical(this,tr("错误"),
                                  tr("ShutMgr.Ext.exe 不存在或无法访问！\n"
                                     "某些功能不可用\n"
                                     "请检查该程序的位置或访问权限"));
            return;
        }
    }
    char Str_Time_To_Wait[10];
    switch(ChosenShutdownType)
    {
        case SHUTDOWN   :command+="-s ";
             break;
        case RESTART    :command+="-r ";
             break;
        case SHUTDOWN_P :command+="-p ";
             break;
        case HIBERNATE  :command+="-h ";
            break;
        case SLEEP      :command+="-Sleep ";
            break;
        case LOGOUT     :command+="-l ";
            break;
        case LOCK_COMPUTER
                        :command+="-Lock";
            break;
        case ABORT      :LOG_APPEND((MoreOption[TOBIOS]
                                   ?"正在执行...，命令为「 ShutMgr.Ext -a -fw 」"
                                   :"正在执行...，命令为「 ShutMgr.Ext -a 」"));
                         abort_Shutdown(MoreOption[TOBIOS]);
                         LOG_APPEND("完成");

                         return;

        default         ://未作出选择
            LOG_APPEND("未选择任何操作，停止执行");
            QMessageBox::critical(this,tr("错误"),tr("您没有选择任何操作"));
            return;
    };
    if(MoreOption[FORCE]==true)
        {command+=" -f ";}
    if(MoreOption[HYBRID]==true)
        {command+=" -hybrid ";}
    if(MoreOption[TOBIOS]==true)
    {
        command+=" -fw ";
    }
    if(MoreOption[TO_ADVANCED_STARTUP]==true)
        {command+=" -o ";}
    if(MoreOption[WAIT]==true)
    {
        command+=" -t ";

        _itoa_s(Time_To_Wait,Str_Time_To_Wait,10,10);
        command+=Str_Time_To_Wait;
        command+=" ";
    }
    if(MoreOption[RESTART_REGISTED]==true)
    {
        if(ChosenShutdownType==SHUTDOWN)
            MyFuncs::replacestr(command," -s "," -sg ");
        else if(ChosenShutdownType==RESTART)
            MyFuncs::replacestr(command," -r "," -g ");
    }
    if(MoreOption[OTHER_MACHINE])
    {
        if(MachineNameList.length()<=0)
        {
            ui->SetMachine->setChecked(false);
            QMessageBox::critical(this, tr("错误"),
                                         tr("计算机列表是空的，已禁用“指定计算机”\n"
                                            "\n要继续，请再次点击“执行”"),
                                         QMessageBox::Ok);
            LOG_APPEND("空的计算机列表，停止执行");
            return;
        }
        else if(MoreOption[OTHER_MACHINE])
            MachineCount=MachineNameList.length();
    }

//    p indicates that the restart or shutdown is planned.
//    u indicates that the reason is user defined.
//        If neither p nor u is specified the restart or shutdown is
//        unplanned.
    if(MoreOption[GIVE_REASON])
    {
        command+=" -d ";
        switch(ui->comboBox_ReasonType->currentIndex())
        {
            case PLANNED        :command+="P:";
                break;
            case OUT_OF_PLAN    :
                break;
            case USER_DEFINED   :command+="U:";
                break;
        }
        char temp[5]; //65535，5位数
        _itoa_s(ui->Number_MajorReason->value(),temp,10);
        command+=temp;//major
        command+=":";
        _itoa_s(ui->Number_MinorReason->value(),temp,10);
        command+=temp;//minor
        command+=" ";
    }
    std::string command_for_log=command;
    //这里我偷懒了, 创建了command的副本供日志窗口使用
    //因为日志窗口采用UTF8编码，而系统命令行采用ANSI编码
    //所以command_for_log用的是UTF编码
    //comman用的是GBK(中文系统)
    //所以command需要UTF8ToGbk()
    //command_for_log不能要UTF8ToGbk()
    if(MoreOption[COMMENT]==true)
    {
        command+=" -c \"";
        command_for_log=command;
        QString temp=ui->Comment_Text->document()->toPlainText();
        command+=Utf8ToGbk(temp.toStdString().c_str());//转换编码为GBK，否则乱码
        command_for_log+=temp.toStdString();
        //以后转换成ANSI系统编码...
        command+="\" ";//添加引号->支持空格
        command_for_log+="\" ";
    }
    if(command.length()>=114200) //MAX_COMMAND_CHAR=114514, 除去数字、计算机名够了
    {
        QMessageBox::critical(this, tr("错误"),
                                     tr("命令过长，请减少注释量\n"),
                                     QMessageBox::Ok);
        LOG_APPEND("命令过长，停止执行");
        return;
    }
//----------正式执行----------
//调用shutdown.exe 或 ShutMgr.Ext（API、多线程后面编）

//#define FOR_DEBUG
#ifdef FOR_DEBUG
        system(((std::string)"echo "+command).c_str());
#else
    sprintf_s(StatusStr,MAX_COMMAND_CHAR,"正在执行 0/%d...",MachineCount);
    LOG_APPEND(StatusStr);
    if(MoreOption[OTHER_MACHINE])//其他计算机
        for(int i=0;i<MachineCount;i++)
        {
            sprintf_s(StatusStr,MAX_COMMAND_CHAR,
                      "正在执行 %d/%d..., 命令为「 %s 」",
                      i+1,
                      MachineCount,
                      (command_for_log+" -m "+(MachineNameList[i].toStdString())).c_str());
            LOG_APPEND(StatusStr);

            RunHide((command+" -m "+MachineNameList[i].toStdString()).c_str());
        }
    else//本机
    {
        sprintf_s(StatusStr,MAX_COMMAND_CHAR,"正在执行..., 命令为「 %s 」",command_for_log.c_str());
        LOG_APPEND(StatusStr);
//        RunHide(command.c_str());
        system(((std::string)"start "+command).c_str());
        LOG_APPEND("完成");


    }
    LOG_APPEND("完成");
#endif
}

//void MainWindow::on_StopShutdown_clicked()
//{
//    char StatusStr[MAX_COMMAND_CHAR]="";

//    LOG_APPEND("正在取消关机...");


//    if(not MoreOption[OTHER_MACHINE])
//    {
//        LOG_APPEND("命令为「 ShutMgr.Ext -a 」");
//        RunHide("shutmgr.ext -a"); 这个好像没效果，换成system有效果但是有弹窗
//        RunHide("ShutMgr.Ext.exe -a");

//        sprintf_s(StatusStr,MAX_COMMAND_CHAR,"正在取消..., 命令为「 shutdown -a 」");
//        LOG_APPEND(StatusStr);
//        if(not stop_Booting_to_BIOS)
//        {
//            RunHide("shutdown -a");
//        }
//        else if (AdminRequired&&(!IsUserAnAdmin()))
//        {
//            LOG_APPEND("没有管理员权限，正在申请...");
//            int ret=QMessageBox::warning(this,tr("警告"),
//                                 tr("某项功能只能在管理员权限下可用!\n"
//                                    "是否以管理员身份重启?"),
//                                 QMessageBox::Ok|QMessageBox::Cancel);
//            if(ret==QMessageBox::Ok)    //确定->获取权限
//            {
//                if(UAC::runAsAdmin())
//                {
//                    close();
//                }
//            }
//            //如果点了cancel或者没获取到admin
//            ui->ToBIOS->setChecked(false);
//            ui->checkBox_not_boot_to_Firmware->setChecked(false);
//            QMessageBox::information(this,tr("提示"),
//                                 tr("似乎没有获取到管理员权限\n"
//                                    "已禁用不可用的功能\n"
//                                    "要再次启用，请重试或手动将程序以管理员权限运行\n"
//                                    "\n要继续，请再次点击“执行”"),
//                                 QMessageBox::Ok);
//            LOG_APPEND("未获取到管理员权限，停止执行");
//            return;
//        }

//    }
//    else
//    {
//        int MachineCount=MachineNameList.length();
//            for(int i=0;i<MachineCount;i++)
//        {
//            sprintf_s(StatusStr,MAX_COMMAND_CHAR,"正在取消 %d/%d..., 命令为「 %s 」",
//                      i+1,
//                      MachineCount,
//                      ("shutdown -a -m "+MachineNameList[i].toStdString()).c_str());
//            LOG_APPEND(StatusStr);

//            RunHide(((std::string)"shutdown "+MachineNameList[i].toStdString()).c_str());
//        }
//    }

//    LOG_APPEND("完成");


//}


void MainWindow::on_Shutdown_clicked()
{
    ChosenShutdownType=SHUTDOWN;

    ui->Force_Shutdown->setEnabled(true);

    ui->WaitingTimeCheckBox->setEnabled(true);

    ui->Comment->setEnabled(true);

    ui->RestartRegistedProgram->setEnabled(true);

    ui->PrepareForHybrid->setEnabled(true);

    ui->ToBIOS->setEnabled(true);

    Disable_and_Uncheck(ui->ToAdvStartUp);

    ui->SetMachine->setEnabled(true);
    ui->ChooseMachine_but->setEnabled(true);

    ui->check_GiveReason->setEnabled(true);

    ui->ToBIOS->setText("启动时转到固件");
}

void MainWindow::on_Restart_clicked()
{
    ChosenShutdownType=RESTART;

    ui->Force_Shutdown->setEnabled(true);

    ui->WaitingTimeCheckBox->setEnabled(true);

    ui->Comment->setEnabled(true);

    ui->RestartRegistedProgram->setEnabled(true);

    Disable_and_Uncheck(ui->PrepareForHybrid);

    ui->ToBIOS->setEnabled(true);

    ui->ToAdvStartUp->setEnabled(true);

    ui->SetMachine->setEnabled(true);
    ui->ChooseMachine_but->setEnabled(true);

    ui->check_GiveReason->setEnabled(true);

    ui->ToBIOS->setText("启动时转到固件");
}

void MainWindow::on_Shutdown_p_clicked()
{
    ChosenShutdownType=SHUTDOWN_P;

    ui->Force_Shutdown->setEnabled(true);

    Disable_and_Uncheck(ui->WaitingTimeCheckBox);

    ui->Comment_Text->setEnabled(false);

    Disable_and_Uncheck(ui->Comment);

    Disable_and_Uncheck(ui->RestartRegistedProgram);

    Disable_and_Uncheck(ui->PrepareForHybrid)

    ui->ToBIOS->setEnabled(true);

    Disable_and_Uncheck(ui->ToAdvStartUp)

    Disable_and_Uncheck(ui->SetMachine)
    ui->ChooseMachine_but->setEnabled(false);

    ui->check_GiveReason->setEnabled(true);

    ui->ToBIOS->setText("启动时转到固件");
}

void MainWindow::on_Hibernate_clicked()
{
    ChosenShutdownType=HIBERNATE;
    ui->Force_Shutdown->setEnabled(true);

    ui->WaitingTimeCheckBox->setEnabled(true);  //已添加延时
    ui->Comment_Text->setEnabled(true);//已添加

    Disable_and_Uncheck(ui->Comment);

    Disable_and_Uncheck(ui->RestartRegistedProgram);

    Disable_and_Uncheck(ui->PrepareForHybrid)

    ui->ToBIOS->setEnabled(true);

    Disable_and_Uncheck(ui->ToAdvStartUp)

    Disable_and_Uncheck(ui->SetMachine)
    ui->ChooseMachine_but->setEnabled(false);

    Disable_and_Uncheck(ui->check_GiveReason)

    ui->ToBIOS->setText("启动时转到固件");
}


void MainWindow::on_Sleep_clicked()
{
    ChosenShutdownType=SLEEP;

    Disable_and_Uncheck(ui->Force_Shutdown);

    ui->WaitingTimeCheckBox->setEnabled(true);  //已添加延时

    ui->Comment_Text->setEnabled(true);//已添加

    Disable_and_Uncheck(ui->Comment)

    Disable_and_Uncheck(ui->RestartRegistedProgram)

    Disable_and_Uncheck(ui->PrepareForHybrid);

    Disable_and_Uncheck(ui->ToBIOS)

    Disable_and_Uncheck(ui->ToAdvStartUp);

    Disable_and_Uncheck(ui->SetMachine);
    ui->ChooseMachine_but->setEnabled(false);

    Disable_and_Uncheck(ui->check_GiveReason);

    ui->ToBIOS->setText("启动时转到固件");
}


void MainWindow::on_Logout_clicked()
{
    ChosenShutdownType=LOGOUT;

    ui->Force_Shutdown->setEnabled(true);

    ui->WaitingTimeCheckBox->setEnabled(true);  //已添加延时

    ui->Comment_Text->setEnabled(true);//已添加

    Disable_and_Uncheck(ui->Comment);

    Disable_and_Uncheck(ui->RestartRegistedProgram);

    Disable_and_Uncheck(ui->PrepareForHybrid);

    Disable_and_Uncheck(ui->ToBIOS);

    Disable_and_Uncheck(ui->ToAdvStartUp);

    Disable_and_Uncheck(ui->SetMachine);
    ui->ChooseMachine_but->setEnabled(false);

    Disable_and_Uncheck(ui->check_GiveReason);

    ui->ToBIOS->setText("启动时转到固件");
}

void MainWindow::on_LockComputer_clicked()
{
    ChosenShutdownType=LOCK_COMPUTER;

    Disable_and_Uncheck(ui->Force_Shutdown);

    ui->WaitingTimeCheckBox->setEnabled(true);//已添加

    ui->Comment_Text->setEnabled(true);//已添加

    Disable_and_Uncheck(ui->Comment);

    Disable_and_Uncheck(ui->RestartRegistedProgram);

    Disable_and_Uncheck(ui->PrepareForHybrid)

    Disable_and_Uncheck(ui->ToBIOS);

    Disable_and_Uncheck(ui->ToAdvStartUp)

    Disable_and_Uncheck(ui->SetMachine)
    ui->ChooseMachine_but->setEnabled(false);

    ui->check_GiveReason->setEnabled(false);

    ui->ToBIOS->setText("启动时转到固件");
}

void MainWindow::on_AbortShutdown_clicked()
{
    ChosenShutdownType=ABORT;

    Disable_and_Uncheck(ui->Force_Shutdown);

    Disable_and_Uncheck(ui->WaitingTimeCheckBox);

    ui->Comment_Text->setEnabled(false);

    Disable_and_Uncheck(ui->Comment);

    Disable_and_Uncheck(ui->RestartRegistedProgram);

    Disable_and_Uncheck(ui->PrepareForHybrid)

    ui->ToBIOS->setEnabled(true);

    Disable_and_Uncheck(ui->ToAdvStartUp)

    Disable_and_Uncheck(ui->SetMachine)
    ui->ChooseMachine_but->setEnabled(false);

    ui->check_GiveReason->setEnabled(false);

    ui->ToBIOS->setText("启动时不转到固件");

//    AdminRequired=true;
}


void MainWindow::on_Comment_stateChanged(int arg1)
{
    if(arg1==FULL_CHECKED)
    {
        ui->Comment_Text->setEnabled(true);
        MoreOption[COMMENT]=true;
    }
    else
    {
        ui->Comment_Text->setDisabled(true);
        MoreOption[COMMENT]=false;
    }
}

void MainWindow::on_WaitingTimeCheckBox_stateChanged(int arg1)
{
    ui->WaitingTime->setEnabled(true);
    if(arg1==FULL_CHECKED)
    {
        MoreOption[WAIT]=true;
    }
    else
    {
        ui->WaitingTime->setEnabled(false);
        MoreOption[WAIT]=false;
    }
}

void MainWindow::on_Force_Shutdown_stateChanged(int arg1)
{
    if(arg1==FULL_CHECKED) {MoreOption[FORCE]=true;}
    else {MoreOption[FORCE]=false;}
}

void MainWindow::on_Quit_clicked()
{
    exit(0);
}

void MainWindow::on_WaitingTime_valueChanged(int arg1)
{
    Time_To_Wait=arg1;
    if(arg1==114514 or arg1==1919810)
    {

        trayIcon->show();
        trayIcon->showMessage("臭死力","啊啊啊啊啊 啊啊啊啊啊啊啊啊啊啊啊啊啊啊");
        QMessageBox::information(this,tr("哼哼哼啊啊啊啊啊"),
                                 tr("Homo特有的倒计时（喜"));
        trayIcon->hide();
        EsterEgg=true;
    }
}


void MainWindow::on_UseSystemUI_clicked()
{
    LOG_APPEND("启动系统关机对话框...");
    WinExec("Shutdown -i",SW_NORMAL);
    LOG_APPEND("完成");
}


void MainWindow::on_RestartRegistedProgram_stateChanged(int arg1)
{
    if(arg1==FULL_CHECKED)
    {
        MoreOption[RESTART_REGISTED]=true;
        ui->PrepareForHybrid->setChecked(false);
        ui->ToAdvStartUp->setChecked(false);
    }
    else
    {
        MoreOption[RESTART_REGISTED]=false;
    }
}


void MainWindow::on_PrepareForHybrid_stateChanged(int arg1)
{
    if(arg1==FULL_CHECKED)
    {
        MoreOption[HYBRID]=true;
        ui->RestartRegistedProgram->setChecked(false);
    }
    else
    {
        MoreOption[HYBRID]=false;
    }
}

void MainWindow::on_ToBIOS_stateChanged(int arg1)  //要管理员/Admin Required
{
    if(arg1==FULL_CHECKED)
    {
        MoreOption[TOBIOS]=true;
        ui->SetMachine->setChecked(false);
        ui->Perform->setIconSize(QSize(10,10));
        AdminRequired=true;
    }
    else
    {
        ui->ToBIOS->setChecked(false);
        MoreOption[TOBIOS]=false;
        ui->Perform->setIconSize(QSize(0,0));
        AdminRequired=false;
    }
}

void MainWindow::on_ToAdvStartUp_stateChanged(int arg1)
{
    if(arg1==FULL_CHECKED)
    {
        MoreOption[TO_ADVANCED_STARTUP]=true;
        ui->RestartRegistedProgram->setChecked(false);
    }
    else
    {MoreOption[TO_ADVANCED_STARTUP]=false;}
}


void MainWindow::on_ChooseMachine_but_clicked()
{
//    Popup->setWindowModality(Qt::ApplicationModal);
    MachineChoosing->show();
}


void MainWindow::on_SetMachine_stateChanged(int arg1)
{
    if(arg1==FULL_CHECKED)
    {
        ui->ToBIOS->setChecked(false);
        MoreOption[OTHER_MACHINE]=true;
    }
    else
    {MoreOption[OTHER_MACHINE]=false;}

}

void MainWindow::on_check_GiveReason_stateChanged(int arg1)
{
    if(arg1==FULL_CHECKED)
    {
        ui->Number_MajorReason->setEnabled(true);
        ui->Number_MinorReason->setEnabled(true);
        ui->comboBox_ReasonType->setEnabled(true);
        ui->label_MajorReason->setEnabled(true);
        ui->label_MinorReason->setEnabled(true);
        MoreOption[GIVE_REASON]=true;
    }
    else
    {
        ui->Number_MajorReason->setEnabled(false);
        ui->Number_MinorReason->setEnabled(false);
        ui->comboBox_ReasonType->setEnabled(false);
        ui->label_MajorReason->setEnabled(false);
        ui->label_MinorReason->setEnabled(false);
        MoreOption[GIVE_REASON]=false;
    }
}

void MainWindow::on_OpenLogWindow_linkActivated(const QString &link)
{
    logWindow->show();
}


void MainWindow::on_About_linkActivated(const QString &link)
{
    QString aboutStr="关机管理器 [版本 2.0.0.143]\n"
                     "\t版权所有(c)2022 Command Prompt\n\n"
                     "使用（开源） Qt 5.15.2 开发\n"
                     "使用 MSVC 2019(32-bit) 编译\n\n"
                     "本程序为自由软件\n"
                     "您可依据自由软件基金会所发表的GNU通用公共授权条款，对本程序再次发布和/或修改\n"
                     "无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。\n\n"
                     "本程序是基于使用目的而加以发布，然而不负任何担保责任,亦无对适售性或特定目的适用性所为的默示性担保。\n"
                     "\n"
                     "详情请参照GNU通用公共授权。\n"
                     "\n";
    if(EsterEgg==true)
        aboutStr="这事一个一个关机管理器啊啊啊啊啊啊\n"
                 "版本事2.0.0.143（呕\n\n"
                 "\t版权所有(c)2022 Command Prompt(一个一个homo\n\n"
                 "使用（开源） Qt (5.1.4)+(0.14.-2)=5.15.2 开发\n"
                 "使用 MSVC x(32-bit) 编译\n"
                 "x=-(1-145)*14+11*(-4)+51-4\n\n"
                 "本程序为自由♂软件\n"
                 "您可依据自由♂软件基金会所发表的GNU通用公共授权条款，对本程序再次发布和/或修改\n"
                 "无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。\n\n"
                 "本程序是基于使用目的而加以发布，然而不负任何担保责任,亦无对适售性或特定目的适用性所为的默示性担保。\n"
                 "\n"
                 "详情请参照GNU通用公共授权。\n"
                 "\n";
    QMessageBox::about(this, tr("关于"),
                              aboutStr);

}

void MainWindow::on_LightModeOn_but_clicked()
{
    DarkModeOn=false;
    MainWindow::setStyleSheet("");
    logWindow->setStyleSheet("");
    MachineChoosing->setStyleSheet("");
}

void MainWindow::on_DarkModeOn_but_clicked()
{
    DarkModeOn=true;
    MainWindow::setStyleSheet(MainWindow_DarkModeStyle);
    logWindow->setStyleSheet(Logwindow_DarkModeStyle);
    MachineChoosing->setStyleSheet(ChoseMachine_DarkModeStyle);
}


void MainWindow::on_DarkModeAuto_clicked()
{
    if(MyFuncs::getDarkModeStatus()==0) //1->light 0->dark
    {
        on_DarkModeOn_but_clicked();
    }
    else
    {
        on_LightModeOn_but_clicked();
    }
}

