/*
 *  版权所有(C)2022 Command Prompt
 *本程序为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
 *对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。
 *本程序是基于使用目的而加以发布，然而不负任何担保责任；
 *亦无对适售性或特定目的适用性所为的默示性担保。
 *详情请参照GNU通用公共授权。
 */
#include <QCoreApplication>
#include "ShutMgr_Ext.h"
#include <windows.h>

#include <ShlObj_core.h>
#pragma comment (lib, "Shell32.lib")

#ifdef  __cplusplus
extern  "C"  {
#endif
   #include <powrprof.h>
#ifdef  __cplusplus
}
#endif

#define HIBERNATE	3
#define SLEEP		4
#define LOGOUT		5

#pragma comment(lib,"powrprof.lib")
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

int ChosenShutdownType=0;
using namespace std;
int WaitingTime=0;
bool toFirmware=false;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
//    RunHide("");
    //	cout<<argc;
    if(argc<=1)
    {
        ErrOutput("E: 参数少于 1 个 \n"
                  "要获取帮助，请键入ShutMgr.Ext -?");
        HelpFunc();
        return -1;
    }

    //1st argument:这里真的懒得改了，太屎山了
    if(ArgCmp(argv[1],"-h")
    or ArgCmp(argv[1],"-hibernate"))
    ChosenShutdownType=HIBERNATE;
    else if(ArgCmp(argv[1],"-sleep"))
    ChosenShutdownType=SLEEP;
    else if(ArgCmp(argv[1],"-l")
    or ArgCmp(argv[1],"-logout"))
    ChosenShutdownType=LOGOUT;
    else if(ArgCmp(argv[1],"-a"))
    {

        return RunHide("taskkill -im ShutMgr.Ext.exe -f");;
    }
    else if(ArgCmp(argv[1],"-?")
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
            if(WaitingTime==-1)             //x的格式无效
            {
                WaitingTime=0;
                ErrOutput("E: \"-t\" 之后必须接有效的时间！(0~315360000)");
            }
        }
        if(ArgCmp(argv[i],"-fw"))
        {
            if(ChosenShutdownType==HIBERNATE)
            {

                if(IsUserAnAdmin()==true)
                    toFirmware=true;
                else
                    ErrOutput("E: 当前程序不是以管理员身份运行的，\n"
                              "请以管理员身份运行并重试。");
            }
            else
            {ErrOutput("\"-fw\" 参数只能搭配 \"-h\"使用！");}
        }
    }
    DWORD pid;//占位
    if(FindProcess("ShutMgr.Ext.exe",pid))
    {
        ErrOutput("E: 已经有计划的睡眠/休眠/注销在运行，请先取消之。");
    }
    Sleep(WaitingTime*1000);
    if(ChosenShutdownType==HIBERNATE)
    {
        if(toFirmware)
            return RunHide("shutdown -h -fw");
        else
            return RunHide("shutdown -h");
    }
    else if(ChosenShutdownType==SLEEP)
    {
        return SetSuspendState(false,false,false);
    }
    else if(ChosenShutdownType==LOGOUT)
    {
        return RunHide("shutdown -l");
    }
    return a.exec();
}
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
