#include "logwnd.h"
#include <QMessageBox>
#include <qfiledialog.h>


LogWnd::LogWnd(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LogWnd)
{
    ui->setupUi(this);
}

LogWnd::~LogWnd()
{
    delete ui;
}

void LogWnd::on_PBSaveLog_clicked()
{
    QString saveFilename=QFileDialog::getSaveFileName(this,
                                                      tr("保存日志..."),"",
                                                      tr("日志文件 (*.log)"));
    if(!saveFilename.isNull())
    {
        QFile* saveFile=new QFile(saveFilename);
        bool retval=saveFile->open(QFile::WriteOnly);
        //文件打开失败
        if(retval==false)
        {
            QMessageBox::critical(this,tr("错误"),tr("无法写入文件"));
            return;
        }

        //文件打开成功
        QTextStream fout(saveFile);
        fout<<ui->TLog->toPlainText();
        saveFile->close();
        appendlog(tr("保存日志到:")+saveFilename);
    }
}


void LogWnd::on_PBCancelLog_clicked()
{
    this->close();
}

