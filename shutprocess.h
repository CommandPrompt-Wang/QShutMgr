#ifndef SHUTPROCESS_H
#define SHUTPROCESS_H

#include <QProcess>
#include <QThread>
#include <QRegExp>
#include <QTextCodec>

class ShutProcess:public QThread
{
    Q_OBJECT
private:
    QString strImageName;
    QString retStr;
public:
    explicit ShutProcess(QString cmdline): QThread(){
        this->strImageName=cmdline;
    }

    void run();

    QString retstr(){
            return retStr;
    }
signals:
    void finished(QString str) ;
};

#endif // SHUTPROCESS_H
