/*
 *  版权所有(C)2022 Command Prompt
 *本程序为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
 *对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。
 *本程序是基于使用目的而加以发布，然而不负任何担保责任；
 *亦无对适售性或特定目的适用性所为的默示性担保。
 *详情请参照GNU通用公共授权。
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h>
#include <shellapi.h>
#include "uac.h"

#define FULL_CHECKED    2   //button fully checked [√]
#define SHUTDOWN        1
#define RESTART         2
#define HIBERNATE       3   //使用ShutMgr.Ext
#define SLEEP           4   //同上
#define LOGOUT          5   //同上
#define SHUTDOWN_P      6
#define FORCE           0
#define WAIT            1
#define \
    RESTART_REGISTED    2
#define HYBRID          3
#define TOBIOS          4
#define \
    TO_ADVANCED_STARTUP 5
#define COMMENT         9
int ChosenShutdownType=0;
bool MoreOption[10]={0};
int Time_To_Wait=30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    std::string command;
    if(ChosenShutdownType<3 or ChosenShutdownType>5)    //for -s -r -p
        command="start shutdown.exe ";
    else
        command="start ShutMgr.Ext.exe ";
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
    };
    if(MoreOption[FORCE]==true)
        {command+=" -f ";}
    if(MoreOption[HYBRID]==true)
        {command+=" -hybrid ";}
    if(MoreOption[TOBIOS]==true) //要管理员
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
    if(MoreOption[COMMENT]==true)
    {
        command+=" -c \"";
        QString temp=ui->Comment_Text->document()->toPlainText();
        command+=Utf8ToGbk(temp.toStdString().c_str());
        command+="\" ";
    }
    if(MoreOption[RESTART_REGISTED]==true)
    {
        if(ChosenShutdownType==SHUTDOWN)
            replacestr(command," -s "," -sg ");
        else if(ChosenShutdownType==RESTART)
            replacestr(command," -r "," -g ");
    }

//调用shutdown.exe 或 ShutMgr.Ext（API、多线程后面编）
//#define FOR_DEBUG
#ifdef FOR_DEBUG
        system(((std::string)"echo "+command).c_str());
#else
        system(command.c_str());// -->乱码已解决
#endif
}

void MainWindow::on_Shutdown_clicked()
{
    ChosenShutdownType=SHUTDOWN;
    ui->Force_Shutdown->setEnabled(true);
    ui->WaitingTimeCheckBox->setEnabled(true);
    ui->Comment->setEnabled(true);
    ui->RestartRegistedProgram->setEnabled(true);
    ui->PrepareForHybrid->setEnabled(true);
    ui->ToBIOS->setEnabled(true);
    ui->ToAdvStartUp->setEnabled(false);
    ui->ToAdvStartUp->setChecked(false);
}

void MainWindow::on_Restart_clicked()
{
    ChosenShutdownType=RESTART;
    ui->Force_Shutdown->setEnabled(true);
    ui->WaitingTimeCheckBox->setEnabled(true);
    ui->Comment->setEnabled(true);
    ui->RestartRegistedProgram->setEnabled(true);
    ui->PrepareForHybrid->setEnabled(false);
    ui->PrepareForHybrid->setChecked(false);
    ui->ToBIOS->setEnabled(true);
    ui->ToAdvStartUp->setEnabled(true);
}

void MainWindow::on_Shutdown_p_clicked()
{
    ChosenShutdownType=SHUTDOWN_P;
    ui->Force_Shutdown->setEnabled(true);
    ui->WaitingTimeCheckBox->setEnabled(false);
    ui->WaitingTimeCheckBox->setChecked(false);
    ui->Comment_Text->setEnabled(false);
    ui->Comment->setEnabled(false);
    ui->Comment->setChecked(false);
    ui->RestartRegistedProgram->setEnabled(false);
    ui->RestartRegistedProgram->setChecked(false);
    ui->PrepareForHybrid->setEnabled(false);
    ui->PrepareForHybrid->setChecked(false);
    ui->ToBIOS->setEnabled(true);
    ui->ToAdvStartUp->setEnabled(false);
    ui->ToAdvStartUp->setChecked(false);
}

void MainWindow::on_Hibernate_clicked()
{
    ChosenShutdownType=HIBERNATE;
    ui->Force_Shutdown->setEnabled(true);
    ui->WaitingTimeCheckBox->setEnabled(true);  //已添加延时
    ui->Comment_Text->setEnabled(false);
    ui->Comment->setEnabled(false);
    ui->Comment->setChecked(false);
    ui->RestartRegistedProgram->setEnabled(false);
    ui->RestartRegistedProgram->setChecked(false);
    ui->PrepareForHybrid->setEnabled(false);
    ui->PrepareForHybrid->setChecked(false);
    ui->ToBIOS->setEnabled(true);
    ui->ToAdvStartUp->setEnabled(false);
    ui->ToAdvStartUp->setChecked(false);
}


void MainWindow::on_Sleep_clicked()
{
    ChosenShutdownType=SLEEP;
    ui->Force_Shutdown->setEnabled(false);
    ui->Force_Shutdown->setChecked(false);
    ui->WaitingTimeCheckBox->setEnabled(true);  //已添加延时
    ui->Comment_Text->setEnabled(false);
    ui->Comment->setEnabled(false);
    ui->Comment->setChecked(false);
    ui->RestartRegistedProgram->setEnabled(false);
    ui->RestartRegistedProgram->setChecked(false);
    ui->PrepareForHybrid->setEnabled(false);
    ui->PrepareForHybrid->setChecked(false);
    ui->ToBIOS->setEnabled(false);
    ui->ToBIOS->setChecked(false);
    ui->ToAdvStartUp->setEnabled(false);
    ui->ToAdvStartUp->setChecked(false);
}


void MainWindow::on_Logout_clicked()
{
    ChosenShutdownType=LOGOUT;

    ui->Force_Shutdown->setEnabled(true);
    ui->WaitingTimeCheckBox->setEnabled(true);  //已添加延时
    ui->Comment_Text->setEnabled(false);
    ui->Comment->setEnabled(false);
    ui->Comment->setChecked(false);
    ui->RestartRegistedProgram->setEnabled(false);
    ui->RestartRegistedProgram->setChecked(false);
    ui->PrepareForHybrid->setEnabled(false);
    ui->PrepareForHybrid->setChecked(false);
    ui->ToBIOS->setEnabled(false);
    ui->ToBIOS->setChecked(false);
    ui->ToAdvStartUp->setEnabled(false);
    ui->ToAdvStartUp->setChecked(false);
}

void MainWindow::on_StopShutdown_clicked()
{
    system("ShutMgr.Ext -a || shutdown -a & exit 0");
//    std::string CurrentPath=QCoreApplication::applicationDirPath().toStdString();
//    HWND ThisWindow=(HWND)(this->winId());
//    ShellExecute(ThisWindow,L"open",L"ShutExt.exe",L"-a",(LPCWSTR)(CurrentPath.c_str()),);
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
    close();
}

void MainWindow::on_WaitingTime_valueChanged(int arg1)
{
    Time_To_Wait=arg1;
}


void MainWindow::on_UseSystemUI_clicked()
{
    system("shutdown -i");
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
    QSize UACIconSize;
    if(arg1==FULL_CHECKED)
    {
        MoreOption[TOBIOS]=true;
        UACIconSize.setWidth(10);
        UACIconSize.setHeight(10);
        ui->Perform->setIconSize(UACIconSize);
        if(!IsUserAnAdmin())
        {
            int ret=QMessageBox::warning(this,tr("警告"),
                                 tr("此功能必须在管理员权限下运行!\n"
                                    "是否以管理员身份重启?"),
                                 QMessageBox::Ok|QMessageBox::Cancel);
            if(ret==QMessageBox::Ok)
            {
                if(UAC::runAsAdmin())
                {
                    close();
                }
            }
        }

    }
    else
    {
        MoreOption[TOBIOS]=false;
        UACIconSize.setWidth(0);
        UACIconSize.setHeight(0);
        ui->Perform->setIconSize(UACIconSize);
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

void MainWindow::on_About_linkActivated(const QString &link)
{
    QMessageBox::information(this, tr("关于"),
                              tr("关机管理器 ver-1.0.0\n\n"
                                 "使用（开源） Qt 5.1.0 开发\n\n"
                                 "\t版权所有(C)2022 Command Prompt\n"
                                 "本程序为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，\n"
                                 "对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。\n"
                                 "本程序是基于使用目的而加以发布，然而不负任何担保责任；\n"
                                 "亦无对适售性或特定目的适用性所为的默示性担保。\n"
                                 "详情请参照GNU通用公共授权。\n"
                                 "MSVC 2015(32-bit)，MSVC 2017(64-bit)编译\n\n"
                                 ),
                              QMessageBox::Ok);
}
