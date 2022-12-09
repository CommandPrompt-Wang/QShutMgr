#ifndef MYFUNCS_H
#define MYFUNCS_H

#include <QMainWindow>
#include <QDir>
#include <windows.h>
#include <QString>
#include <time.h>

extern QList<QString> MachineNameList;

#define LOG_APPEND(x)\
    ui->StatusLabel->setText((x));\
    Log=((QString)MyFuncs::GetTime()+(QString)"\t"+x);\
    logWindow->on_Refresh_clicked();\
    ui->StatusLabel->setToolTip(x);\
    logLines++;

#define RunHide(commandline) WinExec((commandline),SW_HIDE);

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

void abort_Shutdown(bool toFirmware)
{
    if(toFirmware) //这里是“取消引导(not to)至固件” 所以有-fw
    {
        RunHide("ShutMgr.Ext.exe -a -fw");
    }
    else
    {
        RunHide("ShutMgr.Ext.exe -a");
    }
}

#define CHECK_ADMIN()\
if(AdminRequired&&(!IsUserAnAdmin()))\
{\
    LOG_APPEND("没有管理员权限，正在申请...");\
    int ret=QMessageBox::warning(this,tr("警告"),\
                                 tr("某项功能只能在管理员权限下可用！\n"\
                                    "是否以管理员身份重启？"),\
                                 QMessageBox::Ok|QMessageBox::Cancel);\
    if(ret==QMessageBox::Ok)/*确定->获取权限*/\
    {\
        if(UAC::runAsAdmin())\
        {\
            exit(0);\
        }\
    }\
    /*如果点了cancel或者没获取到admin*/\
    ui->ToBIOS->setChecked(false);\
    QMessageBox::information(this,tr("提示"),\
                             tr("似乎没有获取到管理员权限\n"\
                                "已禁用不可用的功能\n"\
                                "要再次启用，请重试或手动将程序以管理员权限运行\n"\
                                "\n要继续，请再次点击“执行”"),\
                             QMessageBox::Ok);\
    LOG_APPEND("未获取到管理员权限，停止执行");\
    return;\
}\

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
