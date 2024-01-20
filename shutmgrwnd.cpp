#include "shutmgrwnd.h"
#include "logwnd.h"
#include "ui_shutmgrwnd.h"
#include <qdatetime.h>
#include <shutprocess.h>
#include <QScreen>
#include <QMoveEvent>

#define APPEND_LOG(X) logWindow->appendlog(X)

enum SHUTDOWN_TYPE : unsigned int {NONE, SHUTDOWN, RESTART, SLEEP, HIBERNATE,
                                   LOCK, LOGOUT, SHUTDOWN_P, CANCEL};

SHUTDOWN_TYPE shutdownType=NONE;


ShutMgrWnd::ShutMgrWnd(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ShutMgrWnd)
{
    ui->setupUi(this);
    logWindow=new LogWnd;
    APPEND_LOG(tr("主程序启动"));

}

ShutMgrWnd::~ShutMgrWnd()
{
    delete ui;
    delete logWindow;
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
    ShutProcess* sprocess=new ShutProcess("shutdown");
    auto processFinished=&ShutProcess::finished;
    auto logappend=&LogWnd::appendlog;
    connect(sprocess,processFinished,logWindow,logappend);
    sprocess->run();
}

void ShutMgrWnd::on_RBShutdown_clicked()
{
    shutdownType=SHUTDOWN;
}


void ShutMgrWnd::on_RBRestart_clicked()
{
    shutdo
}
