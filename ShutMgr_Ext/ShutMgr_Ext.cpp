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
int ChosenShutdownType;
using namespace std;
int WaitingTime;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //	cout<<argc;
    if(argc<=1)
    {
        cout<<"E: �������� 1 �� \n";
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
            system("taskkill -im \"ShutMgr.Ext.exe\" -f");
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
            cout<<"E: ���� "<<argv[1]<<" �������λ��(�� 1 ��)���� \n";
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
                cout<<"E: �� 2 ������ֻ��Ϊ -t \n";
                return -1;
            }
            else if(argc==3 or Atoi(argv[3])!=-1)
            {
                cout<<"E: -t ֮��������Ч��(0~"<<MAX_TIME<<")ʱ�� \n";
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
