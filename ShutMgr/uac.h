/*
 *  版权所有(C)2022 Command Prompt
 *本程序为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
 *对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。
 *本程序是基于使用目的而加以发布，然而不负任何担保责任；
 *亦无对适售性或特定目的适用性所为的默示性担保。
 *详情请参照GNU通用公共授权。
 */
//from https://blog.csdn.net/zyhse/article/details/108166181
#ifndef UAC_H
#define UAC_H

#include <ShlObj_core.h>
#include <QCoreApplication>

// for IsUserAnAdmin()
#pragma comment (lib, "Shell32.lib")

namespace UAC
{
    // 以管理员权限启动一个新实例
    // true-启动了新实例
    // false-未启动新实例
    bool runAsAdmin()
    {
        if (IsUserAnAdmin())
        {
            return false; // 当前程序正以管理员权限运行
        }

        QStringList args = QCoreApplication::arguments(); // 获取命令行参数
        if (args.count() < 2 || args[1] != "runas") // 不带参数或参数不为"runas"时，即直接运行
        {
            // 获取应用程序可执行文件的路径
            QString filePath = QCoreApplication::applicationFilePath();

            // 以管理员权限，执行exe程序
            HINSTANCE ins = ShellExecuteA(nullptr, "runas", filePath.toStdString().c_str(),
                                          "runas", nullptr, SW_SHOWNORMAL);
            if (ins > (HINSTANCE)32)
            {
                return true; // 程序新实例启动成功
            }
        }
        return false;
    }
};

#endif // UAC_H
