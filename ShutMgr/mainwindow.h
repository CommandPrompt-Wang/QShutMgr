#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTextCodec>

#include <stdlib.h>
#include <windows.h>
#include <powrprof.h>

#define and &&
#define or ||
#define not !

#pragma comment(lib, "PowrProf.lib")

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

    void on_StopShutdown_clicked();

    void on_WaitingTimeCheckBox_stateChanged(int arg1);

    void on_Force_Shutdown_stateChanged(int arg1);

    void on_WaitingTime_valueChanged(int arg1);

    void on_UseSystemUI_clicked();

    void on_RestartRegistedProgram_stateChanged(int arg1);

    void on_PrepareForHybrid_stateChanged(int arg1);

    void on_ToBIOS_stateChanged(int arg1);

    void on_ToAdvStartUp_stateChanged(int arg1);

    void on_About_linkActivated(const QString &link);

private:
    Ui::MainWindow *ui;
};
inline void replacestr(std::string& str,std::string find,std::string replace)
{
    size_t pos=str.find(find);
    str.erase(pos,find.length());
    str.insert(pos,replace);
}

#endif // MAINWINDOW_H
