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

void abort_Shutdown(bool toFirmware)
{
    if(toFirmware) //�����ǡ�ȡ������(not to)���̼��� ������-fw
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
    LOG_APPEND("û�й���ԱȨ�ޣ���������...");\
    int ret=QMessageBox::warning(this,tr("����"),\
                                 tr("ĳ���ֻ���ڹ���ԱȨ���¿��ã�\n"\
                                    "�Ƿ��Թ���Ա���������"),\
                                 QMessageBox::Ok|QMessageBox::Cancel);\
    if(ret==QMessageBox::Ok)/*ȷ��->��ȡȨ��*/\
    {\
        if(UAC::runAsAdmin())\
        {\
            exit(0);\
        }\
    }\
    /*�������cancel����û��ȡ��admin*/\
    ui->ToBIOS->setChecked(false);\
    QMessageBox::information(this,tr("��ʾ"),\
                             tr("�ƺ�û�л�ȡ������ԱȨ��\n"\
                                "�ѽ��ò����õĹ���\n"\
                                "Ҫ�ٴ����ã������Ի��ֶ��������Թ���ԱȨ������\n"\
                                "\nҪ���������ٴε����ִ�С�"),\
                             QMessageBox::Ok);\
    LOG_APPEND("δ��ȡ������ԱȨ�ޣ�ִֹͣ��");\
    return;\
}\

/*\
switch (WinExec(commandline,SW_HIDE)) {\
case 0:\
    QMessageBox::critical(this,"����","�ڴ治��");\
    break;\
case ERROR_BAD_FORMAT:\
    QMessageBox::critical(this,\
                          "����","ShutMgr.Ext������Ч�ĳ���!\n"\
                                 "�������Ƿ��𻵻���Ȩ�޷���, ���������ַ�������ذ�װ\n"\
                                 "https://github.com/CommandPrompt-Wang/shutdown-manager/releases");\
    break;\
case ERROR_FILE_NOT_FOUND:\
case ERROR_PATH_NOT_FOUND:\
    QMessageBox::critical(this,\
                          "����","�Ҳ���ShutMgr.Ext!\n"\
                                 "�����������Ҫ�����Ȩ�ޣ��������������й����б�ɾ��\n"\
                                 "�������Ƿ��𻵻���Ȩ�޷���, ���������ַ�������ذ�װ\n"\
                                 "https://github.com/CommandPrompt-Wang/shutdown-manager/releases");\
default:\
    break;\
}*/
#endif // MYFUNCS_H
