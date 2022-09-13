#include "logwindow.h"
#include "ui_logwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

extern QString Log;


LogWindow::LogWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);
    ui->LogText->setText(Log);
    this->setAttribute(Qt::WA_QuitOnClose, false);
}

LogWindow::~LogWindow()
{
    delete ui;
}

void LogWindow::on_Close_clicked()
{
    close();
}


void LogWindow::on_Refresh_clicked()
{
    ui->LogText->setText(Log);
}

void LogWindow::on_Save_clicked()
{
    on_Refresh_clicked();
    QString FileName=QFileDialog::getSaveFileName(this,
                                                  tr("保存日志"),
                                                  "",tr("日志文件(*.log)"));
    if(FileName.isEmpty())
        return;
    QFile File(FileName);
    if(! File.open( QIODevice::WriteOnly ))   //无法打开文件
    {
        QMessageBox::critical(this, tr("错误"),
                              tr("无法写入！请检查文件名称是否合法或是否具有权限。"));
        return;
    }
    File.write(Log.toStdString().c_str());
    File.close();
}

