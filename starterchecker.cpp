#include "starterchecker.h"
#include "qtimer.h"

StarterChecker::StarterChecker(QObject *parent)
    : QThread{parent}
{
    timer=new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(run()));
    timer->start(1);
}
