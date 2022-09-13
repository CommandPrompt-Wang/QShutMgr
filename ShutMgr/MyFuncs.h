#ifndef MYFUNCS_H
#define MYFUNCS_H

#include <QMainWindow>
#include <QDir>
#include <windows.h>
#include <QString>
#include <time.h>

extern QList<QString> MachineNameList;
namespace MyFuncs{
//-------------------�Լ�д�ĺ���
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
    return !_waccess(L"ShutMgr.Ext.exe",04);    //04->�ɶ�ȡ
    //����ֵ0 -> ����     ����ֵ-1 -> ������
}

}

#define LOG_APPEND(x)\
    ui->StatusLabel->setText((x));Log+=((QString)MyFuncs::GetTime()+(QString)"\t"+x);Log+="\n";logWindow->on_Refresh_clicked()
#define RunHide(commandline) WinExec(commandline,SW_HIDE)
#endif // MYFUNCS_H
