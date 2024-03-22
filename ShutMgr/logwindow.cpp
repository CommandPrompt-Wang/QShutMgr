#include "logwindow.h"
#include "ui_logwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

extern QString Log;
extern int logLines;   //��־������������� ����6��ʼ��

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
    ui->LogText->append(Log);
    Log="";
//    ui->LogText->scroll(0,-16*logLines);
//    ui->LogText->set
}

void LogWindow::on_Save_clicked()
{
    on_Refresh_clicked();
    QString FileName=QFileDialog::getSaveFileName(this,
                                                  tr("������־"),
                                                  "",tr("��־�ļ�(*.log)"));
    if(FileName.isEmpty())
        return;
    QFile File(FileName);
    if(! File.open( QIODevice::WriteOnly ))   //�޷����ļ�
    {
        QMessageBox::critical(this, tr("����"),
                              tr("�޷�д�룡�����ļ������Ƿ�Ϸ����Ƿ����Ȩ�ޡ�"));
        return;
    }
    File.write(ui->LogText->toPlainText().toStdString().c_str());
    File.close();
}

