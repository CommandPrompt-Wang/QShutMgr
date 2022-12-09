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
#ifdef _WIN32
#include <Windows.h>
#else
#include <iconv.h>
#endif

#pragma comment(lib, "User32.lib")
//#pragma execution_character_set("utf-8")

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
    cout<<err.c_str()<<endl;
//    abort();
    exit(-1);
}

void HelpFunc(void)
{
    ErrOutput("�÷�:\n"
          "ShutMgr.Ext [-h(ibernate)|-Sleep|-l(ogout)] [-t xxx] [-fw]\n"
          "[-a]\n"
          "-h"      "\tʹ���������\n"
          "-Sleep"  "\tʹ�����˯��\n"
          "-l"      "\tʹ�û��ǳ�(ע��)\n"
          "-Lock"   "\t���������\n"
          "-t x"    "\t����ִ�в���ǰ����ʱ, xȡֵ��0~315360000, \n"
                    "\t�����ָ����ѡ��, ��Ĭ��ֵ���ӳ�(x=60), ����shutdown.exe��ͬ\n"
          "-a"      "\tȡ�������������shutdown.exe���ڵĹػ��������Ȳ�����\n"
          "-fw"     "\t����ʱ�����л���BIOS/UEFI����\n"
          "-f"      "\tǿ�ƹر��������е�Ӧ�ó���������Ⱦ����û���\n"
                    "\t�� -t ���ص� x ���� 0 ʱ��Ĭʾ�� -f\n"
          "-c xxx"  "\t����ע�ͣ��á�\\n�����滻��\n\n"
          "ע��: -f ѡ������ -h,\n"
          "     -fw ��� -h �� -a ����Ҫ����ԱȨ��\n"
          "���ע���к��пո��뽫����ע����Ӣ������������\n"
          "��ϸ˵���μ�shutdown.exe -?\n");
}

//From Website
bool FindProcess(LPCWSTR strProcessName, DWORD processPid)
{

    TCHAR tszProcess[64] = { 0 };
    lstrcpy(tszProcess, strProcessName);
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
            if(processPid!=ps.th32ProcessID)
            {
                //�ҵ�ָ���ĳ���
                CloseHandle(hSnapshot);
                //            printf("�ҵ�����: %ls\n", tszProcess);
                return true;
                //getchar();
                //return dwPid;
            }
        }
    } while (Process32Next(hSnapshot, &ps));
    CloseHandle(hSnapshot);
    return false;
}

std::string GbkToUtf8(const char* src_str)
{
    std::string result;
    wchar_t* strSrc;
    char* szRes;
    int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    strSrc = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, strSrc, len);

    len = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
    szRes = new char[len + 1];
    WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, len, NULL, NULL);
    result = szRes;
    if (strSrc)
        delete[]strSrc;
    if (szRes)
        delete[]szRes;
    return result;
}

#define RunHide(commandline) WinExec(commandline,SW_HIDE)



#endif // SHUTMGR_EXT_H
