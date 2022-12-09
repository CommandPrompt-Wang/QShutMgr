/*
 *  ��Ȩ����(C)2022 Command Prompt
 *������Ϊ������������������������������������GNUͨ�ù�����Ȩ���
 *�Ա������ٴη�����/���޸ģ����������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ���һ�պ��еİ汾��
 *�������ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε������Σ�
 *���޶������Ի��ض�Ŀ����������Ϊ��Ĭʾ�Ե�����
 *���������GNUͨ�ù�����Ȩ��
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
//#include <MyFuncs.h>
#include <QMainWindow>
//#include <QMessageBox>
#include <QTextCodec>

#include <stdlib.h>
#include <windows.h>
#include <powrprof.h>

#define and &&
#define or ||
#define not !

#pragma comment(lib, "PowrProf.lib")
#pragma comment(lib, "user32.lib")

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Shutdown_clicked();

    void on_Shutdown_p_clicked();

    void on_Restart_clicked();

    void on_Hibernate_clicked();

    void on_Sleep_clicked();

    void on_Logout_clicked();

    void on_Perform_clicked();

    void on_Quit_clicked();

    void on_Comment_stateChanged(int arg1);

//    void on_StopShutdown_clicked();

    void on_WaitingTimeCheckBox_stateChanged(int arg1);

    void on_Force_Shutdown_stateChanged(int arg1);

    void on_WaitingTime_valueChanged(int arg1);

    void on_UseSystemUI_clicked();

    void on_RestartRegistedProgram_stateChanged(int arg1);

    void on_PrepareForHybrid_stateChanged(int arg1);

    void on_ToBIOS_stateChanged(int arg1);

    void on_ToAdvStartUp_stateChanged(int arg1);

    void on_About_linkActivated(const QString &link);


    void on_ChooseMachine_but_clicked();

    void on_SetMachine_stateChanged(int arg1);

    void on_OpenLogWindow_linkActivated(const QString &link);

    void on_check_GiveReason_stateChanged(int arg1);

    void on_LockComputer_clicked();

    void on_AbortShutdown_clicked();

//    void on_AbortShutdown_clicked(bool checked);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
