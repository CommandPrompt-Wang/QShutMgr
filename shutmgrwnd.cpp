#include "shutmgrwnd.h"
#include "logwnd.h"
#include "ui_shutmgrwnd.h"
// #include "shutprocess.h"
#include <QScreen>
#include <QMoveEvent>
#include <QMessageBox>
#include <Windows.h>
#include <qdatetime.h>
#include <darkqss.h>
#include <QMenu>
#include <QInputDialog>
#include <QFile>
// #include <starterchecker.h>

#define APPEND_LOG(X) logWindow->appendlog(X)

const char* SHUTDOWN_RADIOBUTTON_TEXT[8]=
{"关机","重启","睡眠(本机)","休眠（本机）","锁定（本机）","注销（本机）","立即关机","取消"};


ShutMgrWnd::ShutMgrWnd(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ShutMgrWnd)
{
    ui->setupUi(this);
    logWindow=new LogWnd;
    machineWindow=new MachineWnd;
    APPEND_LOG(tr("主程序启动"));

    floatBut=new FloatingButton;

    qssList[1]=qss_Dark;
    qssList[0]=qss_Light;

    para=new ShutdownPara;

    //获得按钮
    ShutdownButtons=new QRadioButton[8];

    //遍历，初始化按钮
    for(SHUTDOWN_TYPE i=SHUTDOWN;i<=CANCEL;i=(SHUTDOWN_TYPE)(i+1))
    {
        QString textureOnEachButton= QT_TR_NOOP(SHUTDOWN_RADIOBUTTON_TEXT[i-1]);
        textureOnEachButton+=
                "["
                //如：关机[1]
                +QString::number(i)+
                "]";
        ShutdownButtons[i-1].setText(textureOnEachButton);

        //设置快捷键
        ShutdownButtons[i-1].setShortcut(QKeySequence(QString::number(i)));
        connect(&ShutdownButtons[i-1],&QRadioButton::clicked,[=](){
            this->para->shutdownType=i;

            if(i==SHUTDOWN or i==RESTART)
            {
// #warning here not finished
            }
            else
            {

            }
        });
        //4列2行布局
        this->ui->Grid_Basic->addWidget(&ShutdownButtons[i-1],
                //分拆8个按钮
                ((uint)(i)<=4?1:2),((uint)(i)<=4?i:i-4));
    }

    //连接计算机窗口和主窗口
    connect(this->para,SIGNAL(machineListChanged(QList<QString>)),machineWindow,SLOT(updateMachineList(QList<QString>)));
    connect(machineWindow,SIGNAL(machineListChanged(QList<QString>)),this->para,SLOT(updateMachineList(QList<QString>)));


    //自动添加本机
    APPEND_LOG(QString().append("识别到计算机").append(QHostInfo::localHostName()));
    this->para->addMachine(QHostInfo::localHostName());

    //夜间模式
    this->isDark=false;

    checker=new NightModeChecker;
    connect(ui->BAutoDark, &QPushButton::clicked, [&](){
        checker->startCheck();
    });
    connect(checker, SIGNAL(nightModeChanged(bool)), this, SLOT(setDarkMode(bool)));
    connect(ui->BDarkMode, &QPushButton::clicked, [&](){
        checker->stopCheck();
    });
    connect(ui->BLightMode, &QPushButton::clicked, [&](){
        checker->stopCheck();
    });

    checker->startCheck();

    //开始菜单联动（失败品）
    startChk=new StarterChecker;
    connect(startChk,SIGNAL(startMenuOpen()),this,SLOT(openFloatingBut()));

    //设置预设菜单
    //获取四个按钮地址，方便调用
    But_Presets[0]=this->ui->But_Preset1;
    But_Presets[1]=this->ui->But_Preset2;
    But_Presets[2]=this->ui->But_Preset3;
    But_Presets[3]=this->ui->But_Preset4;

    //创建菜单
    presetSlotMenu=new QMenu;
    for (int i=1; i<=4; i++)
    {
        QAction *eachOne=presetSlotMenu->addAction(QString("预设%1").arg(i));
        // QAction::toggled(bool)
        connect(eachOne,&QAction::triggered,this,
                [=]{
                    QString name=QInputDialog::getText(this,"设置名称","请输入预设名称");
                    setPreset(i,name,this->para);
                });
    }
    ui->But_SavePreset->setMenu(presetSlotMenu);

    for(int i=1;i<=4;i++)
    {

        connect(But_Presets[i-1],&QPushButton::clicked,this,
                [=]{
                    this->applyPreset(i);
                });
    }

    //正确设置焦点
    this->ui->Group_Basic->setFocus();

    loadPreset();

}

ShutMgrWnd::~ShutMgrWnd()
{
    delete ui;
    delete logWindow;
    delete[] ShutdownButtons;
    delete checker;
}

void ShutMgrWnd::moveEvent(QMoveEvent *event){
    //窗口跟随
    if(logWindow->isVisible())
    {
        int deltax=event->pos().x()-event->oldPos().x();
        int deltay=event->pos().y()-event->oldPos().y();
        int logWindowx=logWindow->pos().x();
        int logWindowy=logWindow->pos().y();
        logWindow->move(logWindowx+deltax,logWindowy+deltay);
    }
}

void ShutMgrWnd::on_BExit_clicked()
{
    exit(0);
}

void ShutMgrWnd::on_BViewLog_clicked()
{
    //没显示就显示，显示了就隐藏
    if(not logWindow->isVisible())
    {
        //窗口位置
        QPoint mainWindowPosition=this->pos();
        //获取屏幕大小
        QScreen* screen = QGuiApplication::primaryScreen();
        int screenWidth=screen->geometry().width();

        //使日志窗口出现在主窗口右侧
        //logwindow x pos
        int x=mainWindowPosition.x()+this->width();
        //logwindow y pos
        int y=mainWindowPosition.y();

        //窗口右侧不要超过屏幕
        while((x+(logWindow->width())) > screenWidth)
        {
            //如果超过，尝试左移100px
            this->move(mainWindowPosition.x()-100,mainWindowPosition.y());
            mainWindowPosition=this->pos();
            x=mainWindowPosition.x()+this->width();
            y=mainWindowPosition.y();
        }


        logWindow->move(QPoint(x,y));
        logWindow->show();
    }
    else
    {
        logWindow->close();
    }
}


void ShutMgrWnd::on_BExecute_clicked()
{
    //连接任务
    this->tasks.append(new ShutdownTask);
    ShutdownTask* p=tasks[tasks.length()-1];
    p->set(para);
    connect(p,SIGNAL(appendLog(QString)),logWindow,SLOT(appendlog(QString)));
    connect(p,SIGNAL(taskEnd(ShutdownTask*)),this,SLOT(removeTask(ShutdownTask*)));
    ui->List_tasks->addItem(p->getDesc());
    p->run();
    // ShutProcess* sprocess=new ShutProcess("");

    // //将命令输出写入到日志
    // auto processFinished=&ShutProcess::finished;
    // auto logappend=&LogWnd::appendlog;
    // connect(sprocess,processFinished,logWindow,logappend);

    // if(para->shutdownType!=CANCEL && isShutdownScheduled(this->para->computer))
    // {
    //     QMessageBox::critical(this,tr("错误"),tr("系统已经计划关机，请先取消之！"));
    //     return;
    // }

    // task->set("",para);

    // task->run();
//    QString command;

//    switch(this->para->shutdownType){
//case NONE:
//    QMessageBox::critical(this,tr("错误"),tr("没有指定任何模式！"));
//    return;
//    break;

//case SHUTDOWN:
//    if(this->para)

//    break;

//case RESTART:
//    command="shutdown -r";
//    break;

//case SLEEP:
//    command=""
//    break;

//case HIBERNATE:

//    break;
//case LOCK:

//    break;

//case LOGOUT:

//    break;

//case SHUTDOWN_P:

//    break;

//case CANCEL:

//    break;
//}
//    sprocess->run();

}



//------------------按钮槽函数--------------------
#define BUT_CHECKED 2
#define BUT_UNCHECKED 0

//与另一个选项冲突，因而取消勾选
#define CONFLICT_WITH(x) ui->x->setChecked(false)

//延时关机
void ShutMgrWnd::on_Check_timeout_stateChanged(int arg1)
{
    if(arg1==BUT_CHECKED)
    {
        this->para->bTimeout=true;
        this->para->iTimeout=ui->Spin_timeout->value();
    }
    else
    {
        this->para->bTimeout=false;
    }
}

//设置超时
void ShutMgrWnd::on_Spin_timeout_valueChanged(int arg1)
{
    this->para->iTimeout=arg1;
    ui->Check_timeout->setChecked(true);
}


void ShutMgrWnd::on_Check_force_stateChanged(int arg1)
{
    this->para->bForce=arg1;
}


void ShutMgrWnd::on_Check_toAdvanceOpt_stateChanged(int arg1)
{
    if(arg1==BUT_CHECKED)
    {
        this->para->bToAdvancedOpt=true;
        CONFLICT_WITH(Check_autoLogin);
    }
    else
    {
        this->para->bToAdvancedOpt=false;
    }
}


void ShutMgrWnd::on_Check_toFirmware_stateChanged(int arg1)
{
    if(arg1==BUT_CHECKED)
    {
        this->para->bToFirmware=true;
        // //与"指定计算机"冲突
        // CONFLICT_WITH(Check_machine);
    }
    else
    {
        this->para->bToFirmware=false;
    }
}


// void ShutMgrWnd::on_Check_machine_stateChanged(int arg1)
// {
//     if(arg1==BUT_CHECKED)
//     {
//         // this->para->bMultiMachine=true;

//         //与"转到固件"冲突
//         CONFLICT_WITH(Check_toFirmware);
//     }
//     else
//     {
//         // this->para->bMultiMachine=false;
//     }
// }


void ShutMgrWnd::on_Check_fastboot_stateChanged(int arg1)
{
    if(arg1==BUT_CHECKED)
    {
        this->para->bFastBoot=true;
        CONFLICT_WITH(Check_autoLogin);
    }
    else
    {
        this->para->bFastBoot=false;
    }
}


void ShutMgrWnd::on_Check_autoLogin_stateChanged(int arg1)
{
    if(arg1==BUT_CHECKED)
    {
        this->para->bAutoLogin=true;
        CONFLICT_WITH(Check_toAdvanceOpt);
        CONFLICT_WITH(Check_fastboot);
    }
    else
    {
        this->para->bAutoLogin=false;
    }
}


void ShutMgrWnd::on_Check_reason_stateChanged(int arg1)
{
    if(arg1==BUT_CHECKED)
    {
        this->para->bReason=true;
    }
    else
    {
        this->para->bReason=false;
    }
}


void ShutMgrWnd::on_Combo_reasonType_currentIndexChanged(int index)
{
    //0->planned 1->unplanned 2->custom 3->noreason
    this->para->reasonType=(REASON_TYPE)index;
    ui->Check_reason->setChecked(true);
}


void ShutMgrWnd::on_Spin_majorReason_valueChanged(int arg1)
{
    this->para->majorReason=arg1;
    ui->Check_reason->setChecked(true);
}


void ShutMgrWnd::on_Spin_minorReason_valueChanged(int arg1)
{
    this->para->minorReason=arg1;
    ui->Check_reason->setChecked(true);
}


void ShutMgrWnd::on_Check_comment_stateChanged(int arg1)
{
    if(arg1==BUT_CHECKED)
    {
        this->para->bComment=true;
    }
    else
    {
        this->para->bComment=false;
    }
}


void ShutMgrWnd::on_Edit_Comment_textChanged()
{
    this->para->sComment=ui->Edit_Comment->toPlainText();
    ui->Check_comment->setChecked(true);
}


void ShutMgrWnd::on_But_machine_clicked()
{
    machineWindow->show();
}

void ShutMgrWnd::on_List_tasks_doubleClicked(const QModelIndex &index)
{
    int choice=QMessageBox::question(this,"",tr("是否结束任务？"));
    if(choice==QMessageBox::No)
        return;
    //获取描述，以获取id
    // QString s=ui->List_tasks->item(index.row())->text();
    // uintptr_t id=atoll(s.mid(s.indexOf("id=")+3).toStdString().data());
    // ShutdownTask* p=(ShutdownTask*)id;

    ShutdownTask* p=tasks[index.row()];

    p->taskTerminate();


// #error
}

void ShutMgrWnd::removeTask(ShutdownTask *t){
    int index=tasks.indexOf(t);

    ui->List_tasks->takeItem(index);
    ShutdownTask *p=tasks.takeAt(index);
    delete p;
}

void ShutMgrWnd::on_Line_Condition_textChanged(const QString &arg1)
{
    this->para->condition=arg1;
}


void ShutMgrWnd::on_BDarkMode_clicked()
{
    this->setStyleSheet(qssList[1]);
}


void ShutMgrWnd::on_BLightMode_clicked()
{
    this->setStyleSheet(qssList[0]);
}


void ShutMgrWnd::setDarkMode(bool isDark){
    if(this->isDark!=isDark)
        if(isDark){
            this->isDark=true;
            this->setStyleSheet(qssList[1]);
        }
        else
        {
            this->isDark=false;
            this->setStyleSheet(qssList[0]);
        }
}

void ShutMgrWnd::openFloatingBut(){
    floatBut->show();
}

void ShutMgrWnd::on_But_SavePreset_clicked()
{
    ui->But_SavePreset->showMenu();
}

void ShutMgrWnd::setPreset(int index, QString name, ShutdownPara *p){

    if(p->shutdownType==NONE)
    {
        QMessageBox::critical(this,"错误","请选择关机模式！");
        return;
    }

    //p2避免自己拷贝自己
    ShutdownPara p2;
    p2.set(p);

    presets[index-1].set(&p2);
    strPresetNames[index-1]=name;
    this->But_Presets[index-1]->setText(name.append(QString(" [Alt+%1]").arg(index)));
    this->But_Presets[index-1]->setShortcut(QKeySequence(QString("Alt+%1").arg(index)));



    //同时保存
    QFile PresetFile(QString("preset-%1.json").arg(index));
    if(PresetFile.open(QIODevice::ReadWrite | QIODevice::Truncate)){
        QJsonDocument jsonDoc(p2.toJsonObject(strPresetNames[index-1]));
        PresetFile.write(jsonDoc.toJson().toStdString().c_str());
    }
    else
    {
        QMessageBox::warning(this,"警告","配置槽文件打开失败，文件不能保存\n请检查应用的安装位置，重启再试。");
        return;
    }
}


void ShutMgrWnd::applyPreset(int i){
    ShutdownPara p;
    p.set(&presets[i-1]);
    if(p.shutdownType!=NONE)
        ShutdownButtons[p.shutdownType-1].click();
    else
    {
        QMessageBox::critical(this,"错误","槽是空的");
        return;
    }


    //强制
    if(p.bForce)
        ui->Check_force->setChecked(true);
    else
        ui->Check_force->setChecked(false);

    //延时
    if(p.bTimeout)
    {
        ui->Check_timeout->setChecked(true);
        ui->Spin_timeout->setValue(p.iTimeout);
    }
    else
    {
        ui->Check_timeout->setChecked(false);
    }

    //转到固件
    if(p.bToFirmware)
        ui->Check_toFirmware->setChecked(true);
    else
        ui->Check_toFirmware->setChecked(false);

    //转到固件
    if(p.bFastBoot)
        ui->Check_fastboot->setChecked(true);
    else
        ui->Check_fastboot->setChecked(false);

    //高级启动
    if(p.bToAdvancedOpt)
        ui->Check_toAdvanceOpt->setChecked(true);
    else
        ui->Check_toAdvanceOpt->setChecked(false);

    //自动登录
    if(p.bAutoLogin)
        ui->Check_autoLogin->setChecked(true);
    else
        ui->Check_autoLogin->setChecked(false);

    //注释
    if(p.bComment)
    {
        ui->Check_comment->setChecked(true);
        ui->Edit_Comment->setText(p.sComment);
    }
    else
        ui->Check_comment->setChecked(false);

    //原因
    if(p.bReason)
    {
        ui->Check_reason->setChecked(true);
        ui->Combo_reasonType->setCurrentIndex(p.reasonType);
        ui->Spin_majorReason->setValue(p.majorReason);
        ui->Spin_minorReason->setValue(p.minorReason);
    }
    else
        ui->Check_reason->setChecked(false);

    ui->Line_Condition->setText(p.condition.trimmed());

    this->para->updateMachineList(p.computer);
    emit this->para->machineListChanged(p.computer);

}

void ShutMgrWnd::loadPreset(){
    for( int i=1 ; i<=4 ; i++ )
    {
        QFile presetFile(QString("preset-%1.json").arg(i));
        if(presetFile.open(QIODevice::ReadOnly))
        {
            QJsonDocument jsonPre;
            QJsonParseError err;
            QString strJson=presetFile.readAll();

            //空文件跳过
            if(strJson.trimmed()=="")
                continue;

            jsonPre=QJsonDocument::fromJson(strJson.toStdString().data(),&err);

            if(err.error!=err.NoError)
            {
                QMessageBox::critical(this,"错误",QString("读取preset-%1.json时出现错误%2\n将清空该文件，请重新保存配置").arg(i).arg(err.errorString()));
                if(not presetFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
                {
                    APPEND_LOG(QString("警告：槽%1预设文件或无法清空").arg(i));
                }
                continue;
            }

            ShutdownPara p;
            strPresetNames[i-1] = p.fromJsonObject(jsonPre.object());

            presets[i-1].set(&p);
        }
        else
        {
            APPEND_LOG(QString("警告：找不到槽%1预设文件或无法读取,尝试创建").arg(i));
            if(not presetFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                APPEND_LOG(QString("警告：找不到槽%1预设文件或无法读取,且无法创建").arg(i));
            }
        }
    }
    //应用设置，优先设置1
    for(int i=1 ; i<=4 ;i++){
        if(presets[i-1].shutdownType!=NONE)
            setPreset(i,strPresetNames[i-1], presets+i-1);
    }
}

void ShutMgrWnd::closeEvent(QCloseEvent *event){
    event->accept();
    exit(0);
}
