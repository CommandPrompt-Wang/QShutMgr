/*
 *  ��Ȩ����(C)2022 Command Prompt
 *������Ϊ������������������������������������GNUͨ�ù�����Ȩ���
 *�Ա������ٴη�����/���޸ģ����������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ���һ�պ��еİ汾��
 *�������ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε������Σ�
 *���޶������Ի��ض�Ŀ����������Ϊ��Ĭʾ�Ե�����
 *���������GNUͨ�ù�����Ȩ��
 */
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
