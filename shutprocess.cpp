#include "shutprocess.h"


void ShutProcess::run()
{
    QProcess *process=new QProcess();
    process->start(strImageName);
    process->waitForFinished();
    process->waitForReadyRead();

    QByteArray qba  = process->readAll();

    //解决中文乱码问题
    QTextCodec* pTextCodec = QTextCodec::codecForName("System");
    assert(pTextCodec != nullptr);
    QString str = pTextCodec->toUnicode(qba);
    str=str.remove(QRegExp("\\s").pattern());
    if(process->exitCode()==0 and str.isEmpty())
        retStr=QString(tr("命令 %1 成功执行")).arg(this->strImageName);
    else
    {
        //省略命令帮助
        if (str.contains("shutdown [/i | /l "))
        {

            str=tr("(输出了命令帮助)\n"
                   "这可能是没有键入参数导致的。");
        }
        retStr=QString(tr("命令 %1 执行出错，返回值 %2，输出:\n %3")).arg(this->strImageName).arg(process->exitCode()).arg(str);
    }
    emit finished(retStr);
    return;
}
