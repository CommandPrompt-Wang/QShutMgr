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

int ChosenShutdownType;
using namespace std;
int WaitingTime;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //	cout<<argc;
    if(argc<=1)
    {
        cout<<"E: 参数少于 1 个 \n";
        HelpFunc();
        return -1;
    }
    if(argc>1)
    {
        //1st argument
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
            system("taskkill -im \"ShutMgr.Ext.exe\" -f & exit 0");
            return 0;
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
            cout<<"E: 参数 "<<argv[1]<<" 错误或其位置(第 1 个)错误 \n";
            HelpFunc();
            return -1;
        }
        if(argc>2) //-t xxx
        {
            if( ArgCmp(argv[2],"-t") and
                argc>3 and
                Atoi(argv[3])!=-1) //-t xxx
            {
               WaitingTime=Atoi(argv[3]);
            }
            else if(not ArgCmp(argv[2],"-t"))
            {
                cout<<"E: 第 2 个参数只能为 -t \n";
                return -1;
            }
            else if(argc==3 or Atoi(argv[3])!=-1)
            {
                cout<<"E: -t 之后必须接有效的(0~"<<MAX_TIME<<")时间 \n";
                return -1;
            }
        }
        Sleep(WaitingTime*1000);
        if(ChosenShutdownType==HIBERNATE)
        {
            system("shutdown -h");
            return 0;
        }
        else if(ChosenShutdownType==SLEEP)
        {
            SetSuspendState(false,false,false);
            return 0;
        }
        else if(ChosenShutdownType==LOGOUT)
        {
            system("shutdown -l");
            return 0;
        }
    }
    return a.exec();
}
