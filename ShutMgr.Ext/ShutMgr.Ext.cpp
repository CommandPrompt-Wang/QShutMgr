/*
 *  ��Ȩ����(C)2022 Command Prompt
 *������Ϊ������������������������������������GNUͨ�ù�����Ȩ���
 *�Ա������ٴη�����/���޸ģ����������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ���һ�պ��еİ汾��
 *�������ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε������Σ�
 *���޶������Ի��ض�Ŀ����������Ϊ��Ĭʾ�Ե�����
 *���������GNUͨ�ù�����Ȩ��
 */
#include <QMainWindow>
#include <QApplication>
#include "ShutMgr.Ext.h"
#include <windows.h>
#include <QDir>
#include <ShlObj_core.h>
#pragma comment (lib, "Shell32.lib")

#ifdef  __cplusplus
extern  "C"  {
#endif
   #include <powrprof.h>
#ifdef  __cplusplus
}
#endif

int ChosenShutdownType=0;
#define HIBERNATE	3
#define SLEEP		4
#define LOGOUT		5
#define LOCK_COMPUTER\
                    7
#define ABORT       8

#pragma comment(lib,"powrprof.lib")
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )


using namespace std;
int WaitingTime=60;
std::string comment;
bool commentLeft=false; //-c
bool toFirmware=false; //-fw
bool Forcefully=false; //-f

DWORD Pid;

int main(int argc, char *argv[])
{
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//    QCoreApplication a(argc, argv);
    QApplication a(argc, argv);
    QMainWindow w;
//    RunHide("");
    //	cout<<argc;
#ifdef DEBUG
    Sleep(10000);
#endif
    QDir::setCurrent(qApp->applicationDirPath());
    if(_waccess(L"ShutMgr.Notify.exe",04)==-1)//���֪ͨ��� -1->���ܷ���
    {
        MessageBoxA(NULL,"ShutMgr.Notify.exe��ʧ��-c����������","��ʾ",MB_OK);
    }

    Pid=(DWORD)QCoreApplication::applicationPid();
    if(argc<=1)
    {
        ErrOutput("E: �������� 1 �� \n"
                  "Ҫ��ȡ�����������ShutMgr.Ext -?");
        HelpFunc();
        return -1;
    }

    //1st argument:����������ø��ˣ�̫ʺɽ��
    if(ArgCmp(argv[1],"-h")
    or ArgCmp(argv[1],"-hibernate")) //����
    {
        ChosenShutdownType=HIBERNATE;
    }
    else if(ArgCmp(argv[1],"-sleep")) //˯��
    {
        ChosenShutdownType=SLEEP;
    }
    else if(ArgCmp(argv[1],"-l")        //ע��
         or ArgCmp(argv[1],"-logout"))
    {
        ChosenShutdownType=LOGOUT;
    }
    else if(ArgCmp(argv[1],"-Lock"))    //����
    {
        ChosenShutdownType=LOCK_COMPUTER;
    }
    else if(ArgCmp(argv[1],"-a"))       //ȡ��
    {
//        RunHide("taskkill -im ShutMgr.Ext.exe -f");
//        RunHide("taskkill -im ShutMgr_Ext.exe -f");
        ChosenShutdownType=ABORT;
    }
    else if(ArgCmp(argv[1],"-?")        //����
         or ArgCmp(argv[1],"-h")
         or ArgCmp(argv[1],"-help"))
    {
        HelpFunc();
        return 0;
    }
    else
    {
        ErrOutput((std::string)"E: ���� "+argv[1]+" �������λ��(�� 1 ��)���� \n"
                               "Ҫ��ȡ�����������ShutMgr.Ext -?");
    }

    for(int i=2;i<argc;i++) //��д���ӵڶ�������ɨ��
    {
        if(ArgCmp(argv[i],"-t")&&
                argc>(i+1))//��֤��Խ��
        {
            WaitingTime=Atoi(argv[i+1]);    //��ȡ-t x�е�"x"
            if(WaitingTime<0)             //x�ĸ�ʽ��Ч
            {
                WaitingTime=0;
                ErrOutput("E: \"-t\" ֮��������Ч��ʱ�䣡(0~315360000)");
            }
            i+=1;        //����i+=1����Ϊ��һ��ѭ����ɨ�赽���֣�ʱ�䣩��Ȼ��ֱ�ӹ�
            continue;
        }
        else if(ArgCmp(argv[i],"-c")&&
                argc>(i+1))//��֤��Խ��
        {
            commentLeft=true;
            comment=argv[i+1];//��ȡע������
            i+=1;        //����i+=1����Ϊ��һ��ѭ����ɨ�赽ע�ͣ�Ȼ��ֱ�ӹ�
            continue;
        }
        else if(ArgCmp(argv[i],"-fw"))
        {
            if(ChosenShutdownType==HIBERNATE||
               ChosenShutdownType==ABORT)
            {
                if(IsUserAnAdmin()==(BOOL)true)
                    toFirmware=true;
                else
                    ErrOutput("E: ��ǰ�������Թ���Ա������еģ�\n"
                              "���Թ���Ա������в����ԡ�");
            }
            else
            {ErrOutput("\"-fw\" ����ֻ�ܴ��� \"-h\" \"-a\"ʹ�ã�");}
        }
        else if(ArgCmp(argv[i],"-f"))
        {
            if(ChosenShutdownType==HIBERNATE)
            {Forcefully=true;}
            else
            {ErrOutput("E: \"-f\" ����ֻ�ܴ��� \"-h\"ʹ�ã�");}
        }
        else
        {
            char posStr[114]="";
            _itoa_s(i,posStr,114,10);
            ErrOutput((std::string)"E: ���� "+argv[i]+" �������λ��(�� "+posStr+" ��)���� \n"
                                                    "Ҫ��ȡ�����������ShutMgr.Ext -?");
        }
    }



/*ȡ��*/
    if(ChosenShutdownType==ABORT)   //���ȴ�����������
    {
        RunHide("taskkill -im ShutMgr.Ext.exe -f");
        RunHide("taskkill -im ShutMgr_Ext.exe -f");


        system("ShutMgr.Notify NULL ��ȡ��(���ܵ�)�ػ�����. NULL");
        if(toFirmware) //�����ǡ�ȡ������(not to)���̼��� ������-fw
            return RunHide("shutdown -a -fw");
        else
            return RunHide("Shutdown -a");
    }

/*��ͻ*/
    if(FindProcess(L"ShutMgr.Ext.exe",Pid)   //����ͻ����
     ||FindProcess(L"ShutMgr_Ext.exe",Pid))
    {
        ErrOutput("E: �Ѿ��мƻ���˯��/����/ע�������У�����ȡ��֮��ShutMgr.Ext -a����\n"
                  "���������Ƿ�����ΪShutMgr.Ext.exe�Ľ�������");
    }
/*ע��*/
    if(commentLeft==true)
    {
        string notifyCommand="ShutMgr.Notify NULL ";

        notifyCommand+=comment;
        notifyCommand+=" NULL";
        system(notifyCommand.c_str());
        MessageBoxA(NULL,notifyCommand.c_str(),notifyCommand.c_str(),MB_OK);
    }
    else
    {
        string notifyCommand="ShutMgr.Notify NULL \"����";
        if(WaitingTime>0)
        {
            notifyCommand+="��";
            char timeStr[10]="";//��waitingtimeת��Ϊ�ַ���
            _itoa_s(WaitingTime,timeStr,10,10);
            notifyCommand+=timeStr; //����ʱ��
            notifyCommand+="���";
        }
        switch (ChosenShutdownType) { //�ַ��������ߣ�˯�ߣ�ע��������
        case HIBERNATE:
            notifyCommand+="����";
            break;
        case SLEEP:
            notifyCommand+="˯��";
            break;
        case LOGOUT:
            notifyCommand+="ע��";
            break;
        case LOCK_COMPUTER:
            notifyCommand+="����";
            break;
        default:
            break;
        }
        notifyCommand+="\" NULL";
        system(notifyCommand.c_str());
    }


/*�ȴ�*/
    Sleep(WaitingTime*1000);

/*ִ��*/
    if(ChosenShutdownType==HIBERNATE)
    {
        string command="shutdown -h";
        if(toFirmware)
            command+=" -fw";
        if(Forcefully)
            command+=" -f";
        RunHide(command.c_str());
    }
    else if(ChosenShutdownType==SLEEP)
    {
        return SetSuspendState(false,false,false);
    }
    else if(ChosenShutdownType==LOGOUT)
    {
        return RunHide("shutdown -l");
    }
    else if(ChosenShutdownType==LOCK_COMPUTER)
    {
        return LockWorkStation();
    }
    return a.exec();
}
//��ӡ
//if(argc>2) //-t xxx
//{
//    if( ArgCmp(argv[2],"-t") and
//        argc>3 and
//        Atoi(argv[3])!=-1) //-t xxx
//    {
//       WaitingTime=Atoi(argv[3]);
//    }
//    else if(not ArgCmp(argv[2],"-t"))
//    {
//        ErrOutput("E: �� 2 ������ֻ��Ϊ -t \n");
//        return -1;
//    }
//    else if(argc==3 or Atoi(argv[3])!=-1)
//    {
//        ErrOutput((std::string)"E: -t ֮��������Ч��(0~315360000)���� \n");
//        return -1;
//    }
//}
