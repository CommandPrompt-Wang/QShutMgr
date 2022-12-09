/*
 *  版权所有(C)2022 Command Prompt
 *本程序为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
 *对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。
 *本程序是基于使用目的而加以发布，然而不负任何担保责任；
 *亦无对适售性或特定目的适用性所为的默示性担保。
 *详情请参照GNU通用公共授权。
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
    if(_waccess(L"ShutMgr.Notify.exe",04)==-1)//检查通知组件 -1->不能访问
    {
        MessageBoxA(NULL,"ShutMgr.Notify.exe丢失，-c参数不可用","提示",MB_OK);
    }

    Pid=(DWORD)QCoreApplication::applicationPid();
    if(argc<=1)
    {
        ErrOutput("E: 参数少于 1 个 \n"
                  "要获取帮助，请键入ShutMgr.Ext -?");
        HelpFunc();
        return -1;
    }

    //1st argument:这里真的懒得改了，太屎山了
    if(ArgCmp(argv[1],"-h")
    or ArgCmp(argv[1],"-hibernate")) //休眠
    {
        ChosenShutdownType=HIBERNATE;
    }
    else if(ArgCmp(argv[1],"-sleep")) //睡眠
    {
        ChosenShutdownType=SLEEP;
    }
    else if(ArgCmp(argv[1],"-l")        //注销
         or ArgCmp(argv[1],"-logout"))
    {
        ChosenShutdownType=LOGOUT;
    }
    else if(ArgCmp(argv[1],"-Lock"))    //锁定
    {
        ChosenShutdownType=LOCK_COMPUTER;
    }
    else if(ArgCmp(argv[1],"-a"))       //取消
    {
//        RunHide("taskkill -im ShutMgr.Ext.exe -f");
//        RunHide("taskkill -im ShutMgr_Ext.exe -f");
        ChosenShutdownType=ABORT;
    }
    else if(ArgCmp(argv[1],"-?")        //帮助
         or ArgCmp(argv[1],"-h")
         or ArgCmp(argv[1],"-help"))
    {
        HelpFunc();
        return 0;
    }
    else
    {
        ErrOutput((std::string)"E: 参数 "+argv[1]+" 错误或其位置(第 1 个)错误 \n"
                               "要获取帮助，请键入ShutMgr.Ext -?");
    }

    for(int i=2;i<argc;i++) //重写：从第二个参数扫起
    {
        if(ArgCmp(argv[i],"-t")&&
                argc>(i+1))//保证不越界
        {
            WaitingTime=Atoi(argv[i+1]);    //获取-t x中的"x"
            if(WaitingTime<0)             //x的格式无效
            {
                WaitingTime=0;
                ErrOutput("E: \"-t\" 之后必须接有效的时间！(0~315360000)");
            }
            i+=1;        //这里i+=1是因为下一次循环会扫描到数字（时间），然后直接挂
            continue;
        }
        else if(ArgCmp(argv[i],"-c")&&
                argc>(i+1))//保证不越界
        {
            commentLeft=true;
            comment=argv[i+1];//获取注释内容
            i+=1;        //这里i+=1是因为下一次循环会扫描到注释，然后直接挂
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
                    ErrOutput("E: 当前程序不是以管理员身份运行的，\n"
                              "请以管理员身份运行并重试。");
            }
            else
            {ErrOutput("\"-fw\" 参数只能搭配 \"-h\" \"-a\"使用！");}
        }
        else if(ArgCmp(argv[i],"-f"))
        {
            if(ChosenShutdownType==HIBERNATE)
            {Forcefully=true;}
            else
            {ErrOutput("E: \"-f\" 参数只能搭配 \"-h\"使用！");}
        }
        else
        {
            char posStr[114]="";
            _itoa_s(i,posStr,114,10);
            ErrOutput((std::string)"E: 参数 "+argv[i]+" 错误或其位置(第 "+posStr+" 个)错误 \n"
                                                    "要获取帮助，请键入ShutMgr.Ext -?");
        }
    }



/*取消*/
    if(ChosenShutdownType==ABORT)   //不等待，不检查进程
    {
        RunHide("taskkill -im ShutMgr.Ext.exe -f");
        RunHide("taskkill -im ShutMgr_Ext.exe -f");


        system("ShutMgr.Notify NULL 已取消(可能的)关机进程. NULL");
        if(toFirmware) //这里是“取消引导(not to)至固件” 所以有-fw
            return RunHide("shutdown -a -fw");
        else
            return RunHide("Shutdown -a");
    }

/*冲突*/
    if(FindProcess(L"ShutMgr.Ext.exe",Pid)   //检查冲突进程
     ||FindProcess(L"ShutMgr_Ext.exe",Pid))
    {
        ErrOutput("E: 已经有计划的睡眠/休眠/注销在运行，请先取消之（ShutMgr.Ext -a）。\n"
                  "或者请检查是否有名为ShutMgr.Ext.exe的进程运行");
    }
/*注释*/
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
        string notifyCommand="ShutMgr.Notify NULL \"即将";
        if(WaitingTime>0)
        {
            notifyCommand+="在";
            char timeStr[10]="";//把waitingtime转换为字符串
            _itoa_s(WaitingTime,timeStr,10,10);
            notifyCommand+=timeStr; //插入时间
            notifyCommand+="秒后";
        }
        switch (ChosenShutdownType) { //字符串，休眠，睡眠，注销，锁定
        case HIBERNATE:
            notifyCommand+="休眠";
            break;
        case SLEEP:
            notifyCommand+="睡眠";
            break;
        case LOGOUT:
            notifyCommand+="注销";
            break;
        case LOCK_COMPUTER:
            notifyCommand+="锁定";
            break;
        default:
            break;
        }
        notifyCommand+="\" NULL";
        system(notifyCommand.c_str());
    }


/*等待*/
    Sleep(WaitingTime*1000);

/*执行*/
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
//封印
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
//        ErrOutput("E: 第 2 个参数只能为 -t \n");
//        return -1;
//    }
//    else if(argc==3 or Atoi(argv[3])!=-1)
//    {
//        ErrOutput((std::string)"E: -t 之后必须接有效的(0~315360000)秒数 \n");
//        return -1;
//    }
//}
