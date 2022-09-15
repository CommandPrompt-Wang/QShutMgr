#ifndef MYFUNCS_H
#define MYFUNCS_H

#include <QMainWindow>
#include <QDir>
#include <windows.h>
#include <QString>
#include <time.h>

extern QList<QString> MachineNameList;
namespace MyFuncs{
//-------------------自己写的函数
inline void replacestr(std::string& str,std::string find,std::string replace)
{
    size_t pos=str.find(find);
    str.erase(pos,find.length());
    str.insert(pos,replace);
}
inline const char* GetTime(void)
{
    time_t Unix_Time=time(NULL);
    tm Tm;
    static char StrTime[114];
    localtime_s(&Tm,&Unix_Time);
    asctime_s(StrTime,sizeof(StrTime),&Tm);
    return StrTime;
}

inline bool CheckExt(void)
{
    return !_waccess(L"ShutMgr.Ext.exe",04);    //04->可读取
    //返回值0 -> 可以     返回值-1 -> 不可以
}

}

#define LOG_APPEND(x)\
    ui->StatusLabel->setText((x));\
    Log+=((QString)MyFuncs::GetTime()+(QString)"\t"+x);\
    Log+="\n";\
    logWindow->on_Refresh_clicked();\
    ui->StatusLabel->setToolTip(x)

#define RunHide(commandline) WinExec((commandline),SW_HIDE);
/*\
switch (WinExec(commandline,SW_HIDE)) {\
case 0:\
    QMessageBox::critical(this,"错误","内存不足");\
    break;\
case ERROR_BAD_FORMAT:\
    QMessageBox::critical(this,\
                          "错误","ShutMgr.Ext不是有效的程序!\n"\
                                 "请检查它是否损坏或无权限访问, 或到下面的网址重新下载安装\n"\
                                 "https://github.com/CommandPrompt-Wang/shutdown-manager/releases");\
    break;\
case ERROR_FILE_NOT_FOUND:\
case ERROR_PATH_NOT_FOUND:\
    QMessageBox::critical(this,\
                          "错误","找不到ShutMgr.Ext!\n"\
                                 "这可能是它需要特殊的权限，或在主程序运行过程中被删除\n"\
                                 "请检查它是否损坏或无权限访问, 或到下面的网址重新下载安装\n"\
                                 "https://github.com/CommandPrompt-Wang/shutdown-manager/releases");\
default:\
    break;\
}*/
#endif // MYFUNCS_H
