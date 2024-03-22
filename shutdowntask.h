#ifndef SHUTDOWNTASK_H
#define SHUTDOWNTASK_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QMessageBox>
#include <shutprocess.h>
#include <shutdownpara.h>
#include <Windows.h>
#include <QDateTime>
extern "C"
{
#include <powrprof.h>
}



class ShutdownTask : public QThread
{
    Q_OBJECT
public:
    explicit ShutdownTask(QObject *parent = nullptr);
    void set(const ShutdownPara* para);
    void run() override;

    QString getDesc();

private:
    // QString condition;
    ShutdownPara para;
    QTimer* timer;

signals:
    void taskBegin(QString condition, ShutdownPara paras, ShutdownTask *);
    void taskEnd(ShutdownTask*);
    void appendLog(QString);

public slots:
    void taskTerminate(){
        emit appendLog("任务被终止。\n");

        emit taskEnd(this);

        // timer->stop();
    }
    void execShutdown();

};

#endif // SHUTDOWNTASK_H
