/*
 *  版权所有(C)2022 Command Prompt
 *本程序为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
 *对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）任一日后发行的版本。
 *本程序是基于使用目的而加以发布，然而不负任何担保责任；
 *亦无对适售性或特定目的适用性所为的默示性担保。
 *详情请参照GNU通用公共授权。
 */
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
//    qreal  cx = GetSystemMetrics(SM_CXSCREEN);
//    qreal scale = cx / 960;                // 960 = 1920 / 2
//    qputenv("QT_SCALE_FACTOR", QString::number(scale).toLatin1());
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
