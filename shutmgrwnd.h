#ifndef SHUTMGRWND_H
#define SHUTMGRWND_H

#include <QMainWindow>
#include "MachineWnd.h"
#include "shutdownpara.h"
#include "logwnd.h"
#include "qradiobutton.h"
#include "shutdowntask.h"
#include "starterchecker.h"
#include <nightmodechecker.h>
#include <floatingbutton.h>




QT_BEGIN_NAMESPACE
namespace Ui {
class ShutMgrWnd;
}
QT_END_NAMESPACE



class ShutMgrWnd : public QMainWindow
{
    Q_OBJECT

public:
    ShutMgrWnd(QWidget *parent = nullptr);
    ~ShutMgrWnd();
public slots:
    void setDarkMode(bool isDark);

private slots:

    void on_BExit_clicked();

    void on_BViewLog_clicked();

    void on_BExecute_clicked();

    void moveEvent(QMoveEvent *event) override;

    void on_Check_timeout_stateChanged(int arg1);

    void on_Spin_timeout_valueChanged(int arg1);

    void on_Check_force_stateChanged(int arg1);

    void on_Check_toAdvanceOpt_stateChanged(int arg1);

    void on_Check_toFirmware_stateChanged(int arg1);

    // void on_Check_machine_stateChanged(int arg1);

    void on_Check_fastboot_stateChanged(int arg1);

    void on_Check_autoLogin_stateChanged(int arg1);

    void on_Check_reason_stateChanged(int arg1);

    void on_Combo_reasonType_currentIndexChanged(int index);

    void on_Spin_majorReason_valueChanged(int arg1);

    void on_Spin_minorReason_valueChanged(int arg1);

    void on_Check_comment_stateChanged(int arg1);

    void on_Edit_Comment_textChanged();


    void on_But_machine_clicked();

    // void on_Spin_majorReason_textChanged(const QString &arg1);

    void on_List_tasks_doubleClicked(const QModelIndex &index);

    void removeTask(ShutdownTask * t);

    void on_Line_Condition_textChanged(const QString &arg1);

    // void on_BDarkMode_triggered(QAction *arg1);

    void on_BDarkMode_clicked();

    void on_BLightMode_clicked();

    // void on_BAutoDark_clicked();

    void openFloatingBut();

    void on_But_SavePreset_clicked();

    void setPreset(int, QString name , ShutdownPara* p);

    void applyPreset(int);

    // void on_But_Preset1_clicked();

private:
    Ui::ShutMgrWnd *ui;
    LogWnd * logWindow;
    MachineWnd * machineWindow;
    //选项按钮
    QRadioButton* ShutdownButtons;
    ShutdownPara* para;
    QList<ShutdownTask*> tasks;
    NightModeChecker* checker;
    // QString condition;
    QMenu* presetSlotMenu;


    QString qssList[2];

    //深色模式
    StarterChecker* startChk;
    bool isDark;

    //开始菜单悬浮窗
    FloatingButton* floatBut;

    //预置槽相关
    ShutdownPara presets[4];
    QPushButton* But_Presets[4];
    QString strPresetNames[4];


    void loadPreset();

protected:
    void closeEvent(QCloseEvent *event);


};

#endif // SHUTMGRWND_H
