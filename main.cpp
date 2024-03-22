#include <QMessageBox>
#include "shutmgrwnd.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <windows.h>
#include <ntsecapi.h>
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "powrprof.lib")
//#include <iostream>

int getShutdownPrivilege();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ShutMgr_R_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }



    ShutMgrWnd w;
    w.show();
    return a.exec();
}

int getShutdownPrivilege(){
    //获取关机特权
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
   //获取进程标志
   if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
         return false;
    //获取关机特权的LUID
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,    &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    //获取这个进程的关机特权
    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    CloseHandle(hToken);

    return GetLastError();
}
