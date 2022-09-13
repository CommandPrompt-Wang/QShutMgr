/*
 *  ��Ȩ����(C)2022 Command Prompt
 *������Ϊ������������������������������������GNUͨ�ù�����Ȩ���
 *�Ա������ٴη�����/���޸ģ����������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ���һ�պ��еİ汾��
 *�������ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε������Σ�
 *���޶������Ի��ض�Ŀ����������Ϊ��Ĭʾ�Ե�����
 *���������GNUͨ�ù�����Ȩ��
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
        ErrOutput("E: �������� 1 �� \n"
                  "Ҫ��ȡ�����������ShutMgr.Ext -?");
        HelpFunc();
        return -1;
    }

    //1st argument:����������ø��ˣ�̫ʺɽ��
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
        ErrOutput((std::string)"E: ���� "+argv[1]+" �������λ��(�� 1 ��)���� \n"
                               "Ҫ��ȡ�����������ShutMgr.Ext -?");
    }

    for(int i=2;i<argc;i++) //��д���ӵڶ�������ɨ��
    {
        if(ArgCmp(argv[i],"-t")&&
                argc>(i+1))//��֤��Խ��
        {
            WaitingTime=Atoi(argv[i+1]);    //��ȡ-t x�е�"x"
            if(WaitingTime==-1)             //x�ĸ�ʽ��Ч
            {
                WaitingTime=0;
                ErrOutput("E: \"-t\" ֮��������Ч��ʱ�䣡(0~315360000)");
            }
        }
        if(ArgCmp(argv[i],"-fw"))
        {
            if(ChosenShutdownType==HIBERNATE)
            {

                if(IsUserAnAdmin()==true)
                    toFirmware=true;
                else
                    ErrOutput("E: ��ǰ�������Թ���Ա������еģ�\n"
                              "���Թ���Ա������в����ԡ�");
            }
            else
            {ErrOutput("\"-fw\" ����ֻ�ܴ��� \"-h\"ʹ�ã�");}
        }
    }
    DWORD pid;//ռλ
    if(FindProcess("ShutMgr.Ext.exe",pid))
    {
        ErrOutput("E: �Ѿ��мƻ���˯��/����/ע�������У�����ȡ��֮��");
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
//        ErrOutput("E: �� 2 ������ֻ��Ϊ -t \n");
//        return -1;
//    }
//    else if(argc==3 or Atoi(argv[3])!=-1)
//    {
//        ErrOutput((std::string)"E: -t ֮��������Ч��(0~315360000)���� \n");
//        return -1;
//    }
//}
