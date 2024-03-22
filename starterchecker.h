#ifndef STARTERCHECKER_H
#define STARTERCHECKER_H

#include <QObject>
#include <QThread>
#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <tchar.h>


class StarterChecker : public QThread
{
    Q_OBJECT

private slots:
    void run(){
        bool ret=isStartMenuWindowActive();
        if(ret)
        {
            emit startMenuOpen();
        }
    }


    bool isStartMenuWindowActive() {
        HWND hForegroundWindow = GetForegroundWindow();
        if (hForegroundWindow == NULL) {
            return false; // 如果没有前台窗口，返回false
        }

        DWORD dwForegroundPID = 0;
        GetWindowThreadProcessId(hForegroundWindow, &dwForegroundPID);

        // 检查进程是否为explorer.exe
        if (dwForegroundPID == 0) {
            return false; // 如果进程ID为0，可能是无效的进程ID
        }

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwForegroundPID);
        if (hProcess == NULL) {
            return false; // 无法打开进程
        }

        // 检查进程名称是否为explorer.exe
        TCHAR processName[MAX_PATH];
        if (!GetModuleBaseName(hProcess, NULL, processName, sizeof(processName) / sizeof(TCHAR))) {
            CloseHandle(hProcess);
            return false; // 获取进程名称失败
        }

        if (_tcsicmp(processName, TEXT("explorer.exe")) != 0) {
            CloseHandle(hProcess);
            return false; // 进程名称不匹配
        }

        // 检查窗口标题是否为“开始”
        TCHAR windowTitle[256];
        if (!GetWindowText(hForegroundWindow, windowTitle, sizeof(windowTitle) / sizeof(TCHAR))) {
            CloseHandle(hProcess);
            return false; // 获取窗口标题失败
        }

        if (_tcsicmp(windowTitle, TEXT("开始")) != 0) {
            CloseHandle(hProcess);
            return false; // 窗口标题不匹配
        }

        CloseHandle(hProcess); // 关闭进程句柄
        return true; // 所有条件都满足，返回true
    }

public:
    explicit StarterChecker(QObject *parent = nullptr);

signals:
    void startMenuOpen();

private:
    QTimer * timer;
};

#endif // STARTERCHECKER_H
