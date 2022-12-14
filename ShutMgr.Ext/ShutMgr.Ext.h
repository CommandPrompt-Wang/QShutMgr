/*
 *  版权所有(C)2022 Command Prompt
 *本程序为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
 *对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。
 *本程序是基于使用目的而加以发布，然而不负任何担保责任；
 *亦无对适售性或特定目的适用性所为的默示性担保。
 *详情请参照GNU通用公共授权。
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
    MessageBoxA(NULL,err.c_str(),"错误",MB_OK);
    cout<<err.c_str()<<endl;
//    abort();
    exit(-1);
}

void HelpFunc(void)
{
    ErrOutput("用法:\n"
          "ShutMgr.Ext [-h(ibernate)|-Sleep|-l(ogout)] [-t xxx] [-fw]\n"
          "[-a]\n"
          "-h"      "\t使计算机休眠\n"
          "-Sleep"  "\t使计算机睡眠\n"
          "-l"      "\t使用户登出(注销)\n"
          "-Lock"   "\t锁定计算机\n"
          "-t x"    "\t设置执行操作前的延时, x取值从0~315360000, \n"
                    "\t如果不指定该选项, 则默认值无延迟(x=60), 这与shutdown.exe相同\n"
          "-a"      "\t取消包括本程序和shutdown.exe在内的关机、重启等操作，\n"
          "-fw"     "\t启动时首先切换到BIOS/UEFI界面\n"
          "-f"      "\t强制关闭正在运行的应用程序而不事先警告用户。\n"
                    "\t当 -t 开关的 x 大于 0 时候默示带 -f\n"
          "-c xxx"  "\t增加注释，用“\\n”代替换行\n\n"
          "注意: -f 选项仅针对 -h,\n"
          "     -fw 针对 -h 和 -a 且需要管理员权限\n"
          "如果注释中含有空格，请将整个注释用英文引号括起来\n"
          "详细说明参见shutdown.exe -?\n");
}

//From Website
bool FindProcess(LPCWSTR strProcessName, DWORD processPid)
{

    TCHAR tszProcess[64] = { 0 };
    lstrcpy(tszProcess, strProcessName);
    //查找进程
    STARTUPINFO st;
    PROCESS_INFORMATION pi;
    PROCESSENTRY32 ps;
    HANDLE hSnapshot;
    memset(&st, 0, sizeof(STARTUPINFO));
    st.cb = sizeof(STARTUPINFO);
    memset(&ps, 0, sizeof(PROCESSENTRY32));
    ps.dwSize = sizeof(PROCESSENTRY32);
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    // 遍历进程
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
                //找到指定的程序
                CloseHandle(hSnapshot);
                //            printf("找到进程: %ls\n", tszProcess);
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
