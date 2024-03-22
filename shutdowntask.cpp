#include "shutdowntask.h"

ShutdownTask::ShutdownTask(QObject *parent)
    : QThread{parent}
{
    timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(execShutdown()));
}

void ShutdownTask::set(const ShutdownPara *para)
{
    // this->para.condition=condition;
    this->para.set(para);
}

void ShutdownTask::run()
{
    emit taskBegin(this->para.condition,this->para,this);
    QString description;
    description.append(QString("执行任务%1中...")
                           .arg((uintptr_t)this))
        .append("\n");
    switch(this->para.shutdownType){
    case SHUTDOWN:
        description.append("类型：关机 ");
        break;
    case RESTART:
        description.append("类型：重启");
        break;
    case SLEEP:
        description.append("类型：睡眠");
        break;
    case HIBERNATE:
        description.append("类型：休眠");
        break;
    case LOCK:
        description.append("类型：锁定");
        break;
    case LOGOUT:
        description.append("类型：登出");
        break;
    case SHUTDOWN_P:
        description.append("类型：立即关闭");
        break;
    case CANCEL:
    {
        //这个不要延时
        for(int i=0;i<para.computer.length();i++)
        {
            DWORD errcode=AbortSystemShutdown(para.getMachineName(i));
            emit appendLog(QString("执行函数AbortSystemShutdown(\"%1\")\n返回%2")
                               .arg(para.getMachineName(i))
                               .arg(errcode));
        }
    }
        return;
        break;

    default:
        description.append("类型：未知");
    };
    emit appendLog(description);
    timer->start((this->para.bTimeout?this->para.iTimeout*1000:0));

}

void ShutdownTask::execShutdown()
{
    int iComputerAmount=para.getComputerListlength();

    timer->stop();

    //条件判断

    if(not this->para.condition.trimmed().isEmpty())
    {
        QProcess conditionProcess;
        QStringList conditionPara;
        conditionPara.append("/c");
        conditionPara.append(this->para.condition);

        conditionProcess.start("cmd",conditionPara);
        conditionProcess.waitForFinished(2000);



        QString ret;

        if(conditionProcess.state()==QProcess::NotRunning)
        {
            ret=conditionProcess.readAll();

        }
        else
        {
            conditionProcess.kill();
            ret="1";
            emit appendLog(QString("表达式「%1」超时，已忽略").arg(this->para.condition));
        }
        ret=ret.trimmed();

        if(ret!="1")
        {
            timer->start(2000);
        }
    }



    switch(this->para.shutdownType)
    {
    case SHUTDOWN:
    case RESTART:
    {
        for(int i=0;i<iComputerAmount;i++)
        {
#ifdef DEMO_MODE
            DWORD errcode=0;
#else
            DWORD errcode=InitiateShutdown(para.getMachineName(i),para.getMesssage(),
                                            0,para.getFlags(),para.getReason());
#endif
            emit appendLog(QString("执行函数InitiateShutdown(\"%1\" , %2 , %3 , %4 , %5)\n返回%6")
                               .arg(para.getMachineName(i))
                               .arg(para.getMesssage())
                               .arg("~")
                               .arg(para.getFlags())
                               .arg(para.getReason())
                               .arg(errcode));
        }
        break;
    }
    case SLEEP:
    case HIBERNATE:
    {
        bool isHibernate=this->para.shutdownType==HIBERNATE?true:false;
#ifdef DEMO_MODE
        DWORD errcode=0;
#else
            DWORD errcode=SetSuspendState(
            isHibernate,
            0,false);
#endif
        emit appendLog(QString("执行函数SetSuspendState(%1,0,0)\n返回%2")
                               .arg(isHibernate).arg(errcode));
        break;
    }

    case LOCK:
    {
#ifdef DEMO_MODE
        DWORD errcode=0;
#else
        DWORD errcode=LockWorkStation();
#endif
        emit appendLog(QString("执行函数LockWorkStation()\n返回%1")
                           .arg(errcode));
        break;
    }

    case LOGOUT:
    {
#ifdef DEMO_MODE
        DWORD errcode=0;
#else
        DWORD errcode=ExitWindowsEx(EWX_LOGOFF,para.getReason());
#endif
        emit appendLog(QString("执行函数ExitWindowsEx(EWX_LOGOFF,%1)\n返回%2")
                           .arg(para.getReason()).arg(errcode));


        break;
    }
    case SHUTDOWN_P:
        QMessageBox::warning(NULL,"警告","立即关机已弃用!\n请改用关机");
        break;
    default:
        QMessageBox::information(NULL,"提示","您干了什么？！");
        // return;
    }

    emit taskEnd(this);
}

QString ShutdownTask::getDesc(){
    QString description;
    description.append(QString("id=%1\n").arg((uintptr_t)this));
    switch(this->para.shutdownType){
    case SHUTDOWN:
        description.append("类型：关机 ");
        break;
    case RESTART:
        description.append("类型：重启 ");
        break;
    case SLEEP:
        description.append("类型：睡眠 ");
        break;
    case HIBERNATE:
        description.append("类型：休眠 ");
        break;
    case LOCK:
        description.append("类型：锁定 ");
        break;
    case LOGOUT:
        description.append("类型：登出 ");
        break;
    case SHUTDOWN_P:
        description.append("类型：立即关闭 ");
        break;
    default:
        description.append("类型：未知 ");
    };
    if(this->para.bTimeout)
    {
        auto time=(this->para.bTimeout?this->para.iTimeout:0);
        description.append(QString("执行时间 %1").
                           arg((QDateTime::currentDateTime().addSecs(time)).toString())
                               .append("\n"));
    }
    description.append("涉及计算机：").append(this->para.computer[0])
        .append(QString("等%1台\n").arg(this->para.getComputerListlength()));
    if(this->para.bForce)
        description.append("强制 ");
    if(this->para.bToFirmware)
        description.append("转到固件 ");
    if(this->para.bFastBoot)
        description.append("快速启动 ");
    if(this->para.bToAdvancedOpt)
        description.append("高级启动 ");
    if(this->para.bAutoLogin)
        description.append("自动登录 ");
    description.append("\n");
    if(this->para.bReason){
        QString strReasonType;
        switch (para.reasonType) {
        case PLANNED:
            strReasonType="计划中";
            break;
        case UNPLANNED:
            strReasonType="计划外";
            break;
        case CUSTOM:
            strReasonType="自定义";
            break;
        default:
            strReasonType="未知";
            break;
    }
        description.append(QString("原因代码 %1 %2 %3")
                               .arg(strReasonType)
                               .arg(para.majorReason).arg(para.minorReason)
                           );
    }
    return description;
}
