/*
 *  ��Ȩ����(C)2022 Command Prompt
 *������Ϊ������������������������������������GNUͨ�ù�����Ȩ���
 *�Ա������ٴη�����/���޸ģ����������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ���һ�պ��еİ汾��
 *�������ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε������Σ�
 *���޶������Ի��ض�Ŀ����������Ϊ��Ĭʾ�Ե�����
 *���������GNUͨ�ù�����Ȩ��
 */
#ifndef SHUTMGR_EXT_H
#define SHUTMGR_EXT_H

#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <ShellAPI.h>

#pragma comment(lib, "User32.lib")

//#include <QMessageBox>
using namespace std;
#define MAX_TIME 315360000
#define and &&
#define or ||
#define not !

bool ArgCmp(std::string a, std::string b)
{
    if(a[0] != '-' and a[0] != '/')
    { return false; }
    if(b[0] != '-' and b[0] != '/')
    { return false; }
    if(a.length()!=b.length())
    { return false; }
    for(unsigned it=1;it<a.length();it++)
    {
        if(tolower(a[it])!=tolower(b[it])) return false;
    }
    return true;
}

//modified from: https://blog.csdn.net/lanzhihui_10086/article/details/39995869
int Atoi(const char *str)	//positive number only
{
    int s=0;

    while(*str==' ')
    {
        str++;
    }

    if(not(*str>='0'&&*str<='9'))
    {
        return -1;
    }

    while(*str>='0'&&*str<='9')
    {
        s=s*10+*str-'0';
        str++;
        if(s<0)
        {
            s=2147483647;
            break;
        }
    }
    if(s>MAX_TIME)
        return -1;
    else
        return s;
}


inline void ErrOutput(std::string err)
{
//    TCHAR TcharErr[128]={0};
//    lstrcpy(TcharErr, (LPCWSTR)(err.c_str()));
//    WinExec(((std::string)"msg %username% "+err).c_str(),SW_SHOW);
    MessageBoxA(NULL,err.c_str(),"����",MB_OK);
    cout<<err<<endl;
    abort();
}

void HelpFunc(void)
{
    ErrOutput("�÷�:\n"
          "ShutMgr.Ext [-h(ibernate)|-Sleep|-l(ogout)] [-t xxx] [-fw]\n"
          "[-a]\n"
          "-h"     "\tʹ���������\n"
          "-Sleep" "\tʹ�����˯��\n"
          "-l"     "\tʹ�û��ǳ�(ע��)\n"
          "-t x"   "\t����ִ�в���ǰ����ʱ, xȡֵ��0~315360000, \n"
                   "\t�����ָ����ѡ��, ��Ĭ��Ϊ���ӳ�(x=0), ����shutdown.exe��ͬ\n"
          "-a"     "\tȡ������\n"
          "-fw"    "\t����ʱ�����л���BIOS/UEFI����\n"
          "ע��: -fw ѡ������ -h, ����Ҫ����ԱȨ��\n"
          "��ϸ˵���μ�shutdown.exe -?\n");
}

//From Website
bool FindProcess(std::string strProcessName, DWORD& nPid)
{

    TCHAR tszProcess[64] = { 0 };
    lstrcpy(tszProcess, (LPCWSTR)(strProcessName.c_str()));
    //���ҽ���
    STARTUPINFO st;
    PROCESS_INFORMATION pi;
    PROCESSENTRY32 ps;
    HANDLE hSnapshot;
    memset(&st, 0, sizeof(STARTUPINFO));
    st.cb = sizeof(STARTUPINFO);
    memset(&ps, 0, sizeof(PROCESSENTRY32));
    ps.dwSize = sizeof(PROCESSENTRY32);
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    // ��������
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;
    if (!Process32First(hSnapshot, &ps))
        return false;
    do {
        if (lstrcmp(ps.szExeFile, tszProcess) == 0)
        {
            //�ҵ�ָ���ĳ���
            nPid = ps.th32ProcessID;
            CloseHandle(hSnapshot);
//            printf("�ҵ�����: %ls\n", tszProcess);
            return true;
            //getchar();
            //return dwPid;
        }
    } while (Process32Next(hSnapshot, &ps));
    CloseHandle(hSnapshot);
    return false;
}

#define RunHide(commandline) WinExec(commandline,SW_HIDE)



#endif // SHUTMGR_EXT_H
