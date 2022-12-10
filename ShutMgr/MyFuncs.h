#ifndef MYFUNCS_H
#define MYFUNCS_H

#include <QMainWindow>
#include <QDir>
#include <windows.h>
#include <QString>
#include <QSettings>
#include <time.h>

extern QList<QString> MachineNameList;

#define LOG_APPEND(x)\
    ui->StatusLabel->setText((x));\
    Log=((QString)MyFuncs::GetTime()+(QString)"\t"+x);\
    logWindow->on_Refresh_clicked();\
    ui->StatusLabel->setToolTip(x);\
    logLines++;

#define RunHide(commandline) WinExec((commandline),SW_HIDE);

namespace MyFuncs{//����������дҪlink ����
//    -------------------�Լ�д�ĺ���
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
//�޸���https://blog.csdn.net/qq_37529913/article/details/126767357

    bool getDarkModeStatus()
    {
        QSettings ConfigRead("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                                                 QSettings::NativeFormat);
        return ConfigRead.value("AppsUseLightTheme").toBool();
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


const char MainWindow_DarkModeStyle[]=
        "*{\n"
        "    background-color: rgb(29, 29, 31);\n"
        "}\n"
        "*:enabled{\n"
        "    color: rgb(251, 251, 253);\n"
        "}\n"
        "*:disabled{ \n"
        "    color: rgb(66, 66, 69);\n"
        "}\n"
        "QPushButton:enabled{\n"
        "    background-color: rgb(92, 92, 96);\n"
        "    border-radius:10 px;\n"
        "}\n"
        "QToolButton:disabled{\n"
        "    border-radius:10 px;\n"
        "}\n"
        "QToolButton:enabled{\n"
        "    background-color: rgb(92, 92, 96);\n"
        "    border-radius:10 px;\n"
        "}\n"
        "QComboBox:disabled{\n"
        "    background-color: rgb(29, 29, 31);\n"
        "    border-radius:10 px;\n"
        "    border-color: rgb(66, 66, 69);\n"
        "}\n"
        "QComboBox:ensabled{\n"
        "    background-color: rgb(92, 92, 96);\n"
        "    border-radius:10 px;\n"
        "    border-color: rgb(29, 29, 31);\n"
        "}\n"
        "QTextEdit:disabled{\n"
        "    background-color: rgb(92, 92, 96);\n"
        "    border-radius:10 px;\n"
        "}\n"
        "QSpinBox:enabled{\n"
        "    border:1px solid;\n"
        "    border-color: rgb(66, 66, 69);\n"
        "}\n"
        "QSpinBox:disabled{\n"
        "    border:1px solid;\n"
        "    border-color: rgb(29, 29, 31);\n"
        "}\n"
        "QSpinBox::up-button:enabled {\n"
        "    background-color: rgb(104, 104, 104);\n"
        "}\n"
        "QSpinBox::down-button:enabled {\n"
        "    background-color: rgb(66, 66, 69)\n"
        "}\n"
        "QSpinBox::up-button:disabled {\n"
        "    background-color: rgb(29, 29, 31);\n"
        "}\n"
        "QSpinBox::down-button:disabled {\n"
        "    background-color: rgb(29, 29, 31);\n"
        "}";

const char Logwindow_DarkModeStyle[]=
        "*{background-color: rgb(29, 29, 31);}*:enabled{color: rgb(251, 251, 253);}*:disabled{ color: rgb(66, 66, 69);}QPushButton:enabled{background-color: rgb(92, 92, 96);border-radius:10 px;}QToolButton:disabled{border-radius:10 px;}QToolButton:enabled{background-color: rgb(92, 92, 96);border-radius:10 px;}QTextEdit:enabled{background-color: rgb(92, 92, 96);border-radius:10 px;}";
const char ChoseMachine_DarkModeStyle[]=
        "*{background-color: rgb(29, 29, 31);}*:enabled{color: rgb(251, 251, 253);}*:disabled{ color: rgb(66, 66, 69);}QLineEdit{border:1px solid;border-color: rgb(66, 66, 69);}QListWidget{background-color: rgb(66, 66, 69);}QPushButton:enabled{background-color: rgb(92, 92, 96);border-radius:10 px;}";
#endif // MYFUNCS_H
