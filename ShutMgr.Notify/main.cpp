#include "mainwindow.h"
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QApplication>
#include <windows.h>

std::string GbkToUtf8(const char* src_str);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QSystemTrayIcon *trayIcon=new QSystemTrayIcon;
    if(argc<=3)
    {
        MessageBoxA(NULL,
                "语法:\n"
                "Notification [Title] [Context] [Icon]\n"
                "Title\t消息标题\n"
                "Context\t消息内容\n"
                "Icon\t消息图标(路径)\n"
                "请用“NULL”来“留空”参数;\n"
                "使用“\\n”表示换行, 如「Notification NULL 提示\n内容 NULL\n"
                "似乎必须指定正确的Icon路径(任何图片)才能通知","错误: 参数不足",MB_OK);
        delete trayIcon;
        return -1;
    }

    if(strcmp(argv[3],"NULL")==0)//Icon==NULL
        trayIcon->setIcon(QIcon(":/new/prefix1/resources/Programico.ico")); //默认
    else
        trayIcon->setIcon(QIcon(argv[3]));
    trayIcon->show();
    std::string messageContext=argv[2];
    int temp;
    while((temp=messageContext.find("\\n"))!=-1)
    {
        messageContext.replace(temp,2,"\n");
    }
    trayIcon->showMessage((0==strcmp(argv[1],"NULL"))?"":GbkToUtf8(argv[1]).c_str(),
                          (0==strcmp(argv[2],"NULL"))?"":GbkToUtf8(messageContext.c_str()).c_str());

    trayIcon->hide();
    delete trayIcon;
    return 0;
//    return a.exec();
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
