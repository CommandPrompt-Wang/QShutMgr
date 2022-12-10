/*
 *  ��Ȩ����(C)2022 Command Prompt
 *������Ϊ������������������������������������GNUͨ�ù�����Ȩ���
 *�Ա������ٴη�����/���޸ģ����������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ���һ�պ��еİ汾��
 *�������ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε������Σ�
 *���޶������Ի��ض�Ŀ����������Ϊ��Ĭʾ�Ե�����
 *���������GNUͨ�ù�����Ȩ��
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

#define FULL_CHECKED    2   //button fully checked [��]

int ChosenShutdownType=0;//type
#define SHUTDOWN        1
#define RESTART         2
#define HIBERNATE       3   //ʹ��ShutMgr.Ext
#define SLEEP           4   //ͬ��
#define LOGOUT          5   //ͬ��
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

//���combox_ReasonType
#define PLANNED         0
#define OUT_OF_PLAN     1
#define USER_DEFINED    2

#define MAX_COMMAND_CHAR 114514 //����StatusStr(sprintf_s��Ҫ��)

int Time_To_Wait=30;

QList<QString> MachineNameList;

//֮����ʹ�þ�̬��������Ϊ��־���ڡ�Զ�̼����ѡ�񴰿ڲ����ظ���
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

    logWindow=new LogWindow;            //��־����
    MachineChoosing=new ChoseMachine;   //�����ѡ�񴰿�
    trayIcon=new QSystemTrayIcon(this); //֪ͨ
    trayIcon->setIcon(QIcon(":/new/prefix1/resources/Programico.ico"));
    LOG_APPEND("����������");
    QDir::setCurrent(qApp->applicationDirPath());//���ù���Ŀ¼
    LOG_APPEND("�����չ...");
    if(MyFuncs::CheckExt()==false)
    {
        LOG_APPEND("����: ��չ���� ShutMgr.Ext.exe �����ڻ��޷����ʣ�");
        trayIcon->show();
        trayIcon->showMessage(tr("����"),
                             tr("ShutMgr.Ext.exe �����ڻ��޷����ʣ�\n"
                                "����ó����λ�û����Ȩ��"));
        trayIcon->hide();
        ui->Hibernate->setEnabled(false);       //����ShutMgr.Ext.exe�������
        ui->LockComputer->setEnabled(false);;
        ui->Logout->setEnabled(false);
        ui->Sleep->setEnabled(false);
//        ui->AbortShutdown->setEnabled(false);
    }
    else
        {LOG_APPEND("��⵽��չ����");}
//---------------ҹ��ģʽ��ʵ���ԣ�------------
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

void MainWindow::on_Perform_clicked()       //����ִ�в���
{
    char StatusStr[MAX_COMMAND_CHAR]="";
    int MachineCount=0;
    std::string command;

    CHECK_ADMIN();

    if(ChosenShutdownType==SHUTDOWN
    or ChosenShutdownType==RESTART
    or ChosenShutdownType==SHUTDOWN_P)    //������shutdown.exe -t�����ã�-p����Ҫ�ӳ�
        command="shutdown.exe ";
    else                                 //ʣ�µ�shutdown.exe -t����
    {
        if(MyFuncs::CheckExt())         //�����չ����
            command="ShutMgr.Ext.exe ";
        else
        {
            LOG_APPEND("ShutMgr.Ext.exe �����ڻ��޷�����, ִֹͣ��");
            QMessageBox::critical(this,tr("����"),
                                  tr("ShutMgr.Ext.exe �����ڻ��޷����ʣ�\n"
                                     "ĳЩ���ܲ�����\n"
                                     "����ó����λ�û����Ȩ��"));
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
                                   ?"����ִ��...������Ϊ�� ShutMgr.Ext -a -fw ��"
                                   :"����ִ��...������Ϊ�� ShutMgr.Ext -a ��"));
                         abort_Shutdown(MoreOption[TOBIOS]);
                         LOG_APPEND("���");

                         return;

        default         ://δ����ѡ��
            LOG_APPEND("δѡ���κβ�����ִֹͣ��");
            QMessageBox::critical(this,tr("����"),tr("��û��ѡ���κβ���"));
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
            QMessageBox::critical(this, tr("����"),
                                         tr("������б��ǿյģ��ѽ��á�ָ���������\n"
                                            "\nҪ���������ٴε����ִ�С�"),
                                         QMessageBox::Ok);
            LOG_APPEND("�յļ�����б�ִֹͣ��");
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
        char temp[5]; //65535��5λ��
        _itoa_s(ui->Number_MajorReason->value(),temp,10);
        command+=temp;//major
        command+=":";
        _itoa_s(ui->Number_MinorReason->value(),temp,10);
        command+=temp;//minor
        command+=" ";
    }
    std::string command_for_log=command;
    //������͵����, ������command�ĸ�������־����ʹ��
    //��Ϊ��־���ڲ���UTF8���룬��ϵͳ�����в���ANSI����
    //����command_for_log�õ���UTF����
    //comman�õ���GBK(����ϵͳ)
    //����command��ҪUTF8ToGbk()
    //command_for_log����ҪUTF8ToGbk()
    if(MoreOption[COMMENT]==true)
    {
        command+=" -c \"";
        command_for_log=command;
        QString temp=ui->Comment_Text->document()->toPlainText();
        command+=Utf8ToGbk(temp.toStdString().c_str());//ת������ΪGBK����������
        command_for_log+=temp.toStdString();
        //�Ժ�ת����ANSIϵͳ����...
        command+="\" ";//�������->֧�ֿո�
        command_for_log+="\" ";
    }
    if(command.length()>=114200) //MAX_COMMAND_CHAR=114514, ��ȥ���֡������������
    {
        QMessageBox::critical(this, tr("����"),
                                     tr("��������������ע����\n"),
                                     QMessageBox::Ok);
        LOG_APPEND("���������ִֹͣ��");
        return;
    }
//----------��ʽִ��----------
//����shutdown.exe �� ShutMgr.Ext��API�����̺߳���ࣩ

//#define FOR_DEBUG
#ifdef FOR_DEBUG
        system(((std::string)"echo "+command).c_str());
#else
    sprintf_s(StatusStr,MAX_COMMAND_CHAR,"����ִ�� 0/%d...",MachineCount);
    LOG_APPEND(StatusStr);
    if(MoreOption[OTHER_MACHINE])//���������
        for(int i=0;i<MachineCount;i++)
        {
            sprintf_s(StatusStr,MAX_COMMAND_CHAR,
                      "����ִ�� %d/%d..., ����Ϊ�� %s ��",
                      i+1,
                      MachineCount,
                      (command_for_log+" -m "+(MachineNameList[i].toStdString())).c_str());
            LOG_APPEND(StatusStr);

            RunHide((command+" -m "+MachineNameList[i].toStdString()).c_str());
        }
    else//����
    {
        sprintf_s(StatusStr,MAX_COMMAND_CHAR,"����ִ��..., ����Ϊ�� %s ��",command_for_log.c_str());
        LOG_APPEND(StatusStr);
//        RunHide(command.c_str());
        system(((std::string)"start "+command).c_str());
        LOG_APPEND("���");


    }
    LOG_APPEND("���");
#endif
}

//void MainWindow::on_StopShutdown_clicked()
//{
//    char StatusStr[MAX_COMMAND_CHAR]="";

//    LOG_APPEND("����ȡ���ػ�...");


//    if(not MoreOption[OTHER_MACHINE])
//    {
//        LOG_APPEND("����Ϊ�� ShutMgr.Ext -a ��");
//        RunHide("shutmgr.ext -a"); �������ûЧ��������system��Ч�������е���
//        RunHide("ShutMgr.Ext.exe -a");

//        sprintf_s(StatusStr,MAX_COMMAND_CHAR,"����ȡ��..., ����Ϊ�� shutdown -a ��");
//        LOG_APPEND(StatusStr);
//        if(not stop_Booting_to_BIOS)
//        {
//            RunHide("shutdown -a");
//        }
//        else if (AdminRequired&&(!IsUserAnAdmin()))
//        {
//            LOG_APPEND("û�й���ԱȨ�ޣ���������...");
//            int ret=QMessageBox::warning(this,tr("����"),
//                                 tr("ĳ���ֻ���ڹ���ԱȨ���¿���!\n"
//                                    "�Ƿ��Թ���Ա�������?"),
//                                 QMessageBox::Ok|QMessageBox::Cancel);
//            if(ret==QMessageBox::Ok)    //ȷ��->��ȡȨ��
//            {
//                if(UAC::runAsAdmin())
//                {
//                    close();
//                }
//            }
//            //�������cancel����û��ȡ��admin
//            ui->ToBIOS->setChecked(false);
//            ui->checkBox_not_boot_to_Firmware->setChecked(false);
//            QMessageBox::information(this,tr("��ʾ"),
//                                 tr("�ƺ�û�л�ȡ������ԱȨ��\n"
//                                    "�ѽ��ò����õĹ���\n"
//                                    "Ҫ�ٴ����ã������Ի��ֶ��������Թ���ԱȨ������\n"
//                                    "\nҪ���������ٴε����ִ�С�"),
//                                 QMessageBox::Ok);
//            LOG_APPEND("δ��ȡ������ԱȨ�ޣ�ִֹͣ��");
//            return;
//        }

//    }
//    else
//    {
//        int MachineCount=MachineNameList.length();
//            for(int i=0;i<MachineCount;i++)
//        {
//            sprintf_s(StatusStr,MAX_COMMAND_CHAR,"����ȡ�� %d/%d..., ����Ϊ�� %s ��",
//                      i+1,
//                      MachineCount,
//                      ("shutdown -a -m "+MachineNameList[i].toStdString()).c_str());
//            LOG_APPEND(StatusStr);

//            RunHide(((std::string)"shutdown "+MachineNameList[i].toStdString()).c_str());
//        }
//    }

//    LOG_APPEND("���");


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

    ui->ToBIOS->setText("����ʱת���̼�");
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

    ui->ToBIOS->setText("����ʱת���̼�");
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

    ui->ToBIOS->setText("����ʱת���̼�");
}

void MainWindow::on_Hibernate_clicked()
{
    ChosenShutdownType=HIBERNATE;
    ui->Force_Shutdown->setEnabled(true);

    ui->WaitingTimeCheckBox->setEnabled(true);  //�������ʱ
    ui->Comment_Text->setEnabled(true);//�����

    Disable_and_Uncheck(ui->Comment);

    Disable_and_Uncheck(ui->RestartRegistedProgram);

    Disable_and_Uncheck(ui->PrepareForHybrid)

    ui->ToBIOS->setEnabled(true);

    Disable_and_Uncheck(ui->ToAdvStartUp)

    Disable_and_Uncheck(ui->SetMachine)
    ui->ChooseMachine_but->setEnabled(false);

    Disable_and_Uncheck(ui->check_GiveReason)

    ui->ToBIOS->setText("����ʱת���̼�");
}


void MainWindow::on_Sleep_clicked()
{
    ChosenShutdownType=SLEEP;

    Disable_and_Uncheck(ui->Force_Shutdown);

    ui->WaitingTimeCheckBox->setEnabled(true);  //�������ʱ

    ui->Comment_Text->setEnabled(true);//�����

    Disable_and_Uncheck(ui->Comment)

    Disable_and_Uncheck(ui->RestartRegistedProgram)

    Disable_and_Uncheck(ui->PrepareForHybrid);

    Disable_and_Uncheck(ui->ToBIOS)

    Disable_and_Uncheck(ui->ToAdvStartUp);

    Disable_and_Uncheck(ui->SetMachine);
    ui->ChooseMachine_but->setEnabled(false);

    Disable_and_Uncheck(ui->check_GiveReason);

    ui->ToBIOS->setText("����ʱת���̼�");
}


void MainWindow::on_Logout_clicked()
{
    ChosenShutdownType=LOGOUT;

    ui->Force_Shutdown->setEnabled(true);

    ui->WaitingTimeCheckBox->setEnabled(true);  //�������ʱ

    ui->Comment_Text->setEnabled(true);//�����

    Disable_and_Uncheck(ui->Comment);

    Disable_and_Uncheck(ui->RestartRegistedProgram);

    Disable_and_Uncheck(ui->PrepareForHybrid);

    Disable_and_Uncheck(ui->ToBIOS);

    Disable_and_Uncheck(ui->ToAdvStartUp);

    Disable_and_Uncheck(ui->SetMachine);
    ui->ChooseMachine_but->setEnabled(false);

    Disable_and_Uncheck(ui->check_GiveReason);

    ui->ToBIOS->setText("����ʱת���̼�");
}

void MainWindow::on_LockComputer_clicked()
{
    ChosenShutdownType=LOCK_COMPUTER;

    Disable_and_Uncheck(ui->Force_Shutdown);

    ui->WaitingTimeCheckBox->setEnabled(true);//�����

    ui->Comment_Text->setEnabled(true);//�����

    Disable_and_Uncheck(ui->Comment);

    Disable_and_Uncheck(ui->RestartRegistedProgram);

    Disable_and_Uncheck(ui->PrepareForHybrid)

    Disable_and_Uncheck(ui->ToBIOS);

    Disable_and_Uncheck(ui->ToAdvStartUp)

    Disable_and_Uncheck(ui->SetMachine)
    ui->ChooseMachine_but->setEnabled(false);

    ui->check_GiveReason->setEnabled(false);

    ui->ToBIOS->setText("����ʱת���̼�");
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

    ui->ToBIOS->setText("����ʱ��ת���̼�");

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
        trayIcon->showMessage("������","���������� ����������������������������");
        QMessageBox::information(this,tr("�ߺߺ߰���������"),
                                 tr("Homo���еĵ���ʱ��ϲ"));
        trayIcon->hide();
        EsterEgg=true;
    }
}


void MainWindow::on_UseSystemUI_clicked()
{
    LOG_APPEND("����ϵͳ�ػ��Ի���...");
    WinExec("Shutdown -i",SW_NORMAL);
    LOG_APPEND("���");
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

void MainWindow::on_ToBIOS_stateChanged(int arg1)  //Ҫ����Ա/Admin Required
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
    QString aboutStr="�ػ������� [�汾 2.0.0.143]\n"
                     "\t��Ȩ����(c)2022 Command Prompt\n\n"
                     "ʹ�ã���Դ�� Qt 5.15.2 ����\n"
                     "ʹ�� MSVC 2019(32-bit) ����\n\n"
                     "������Ϊ�������\n"
                     "���������������������������GNUͨ�ù�����Ȩ����Ա������ٴη�����/���޸�\n"
                     "���������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ���һ�պ��еİ汾��\n\n"
                     "�������ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε�������,���޶������Ի��ض�Ŀ����������Ϊ��Ĭʾ�Ե�����\n"
                     "\n"
                     "���������GNUͨ�ù�����Ȩ��\n"
                     "\n";
    if(EsterEgg==true)
        aboutStr="����һ��һ���ػ�������������������\n"
                 "�汾��2.0.0.143��Ż\n\n"
                 "\t��Ȩ����(c)2022 Command Prompt(һ��һ��homo\n\n"
                 "ʹ�ã���Դ�� Qt (5.1.4)+(0.14.-2)=5.15.2 ����\n"
                 "ʹ�� MSVC x(32-bit) ����\n"
                 "x=-(1-145)*14+11*(-4)+51-4\n\n"
                 "������Ϊ���ɡ����\n"
                 "�����������ɡ����������������GNUͨ�ù�����Ȩ����Ա������ٴη�����/���޸�\n"
                 "���������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ���һ�պ��еİ汾��\n\n"
                 "�������ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε�������,���޶������Ի��ض�Ŀ����������Ϊ��Ĭʾ�Ե�����\n"
                 "\n"
                 "���������GNUͨ�ù�����Ȩ��\n"
                 "\n";
    QMessageBox::about(this, tr("����"),
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

