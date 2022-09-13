#include "chosemachine.h"
#include "ui_chosemachine.h"
#include "ui_mainwindow.h"
#include <iostream>

extern QList<QString> MachineNameList;              //ȫ�ֱ������ڴ���ͨѶ

ChoseMachine::ChoseMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChoseMachine)
{
    ui->setupUi(this);
    for(int i=0;i<MachineNameList.length();i++)     //��ȡ(֮ǰ)ָ���ļ����
    {
        ui->MachineList->addItem(MachineNameList[i]);
    }
    ui->MachineEntering->setFocus();    //���������Ϊ���㣬�����û�����
}

ChoseMachine::~ChoseMachine()
{
    delete ui;
}

void ChoseMachine::on_AddMachine_clicked()
{
    QString AddMachineName=ui->MachineEntering->text();
    if(AddMachineName.size()==0)
    {
        QMessageBox::critical(this, tr("����"),
                              tr("���������Ӧ�ǿյ�"),
                              QMessageBox::Ok);
        return;
    }
    if(AddMachineName.indexOf(" ",Qt::CaseInsensitive)!=-1)
    {
        QMessageBox::critical(this, tr("����"),
                              tr("���������Ӧ�����հ�"),
                              QMessageBox::Ok);
        return;
    }
    if(!ui->MachineList->findItems(AddMachineName,Qt::MatchCaseSensitive).isEmpty())
    {
        QMessageBox::warning(this, tr("����"),
                              tr("�����ָ���ļ������"),
                              QMessageBox::Ok);
        return;
    }
    ui->MachineList->addItem(AddMachineName);
    ui->MachineEntering->clear();
    ui->MachineList->sortItems();
    ui->MachineEntering->setFocus();    //���������Ϊ���㣬�����û�����
}

void ChoseMachine::on_RemoveMachine_clicked()
{
    delete ui->MachineList->currentItem();
    ui->MachineEntering->setFocus();    //���������Ϊ���㣬�����û�����
}

void ChoseMachine::on_pushButton_clicked() //OK (����������)
{
    if(ui->MachineList->count()==0)
    {
        QMessageBox::warning(this, tr("����"),
                              tr("δָ���κ�һ�������"),
                              QMessageBox::Ok);
    }
    MachineNameList.clear();
    for(int i=0;i<(ui->MachineList->count());i++)
    {

        MachineNameList.append(ui->MachineList->item(i)->text());
//        QMessageBox::information(this,tr(""),
//                                 ui->MachineList->item(i)->text());
//        ���������
    }
//    Ui::MainWindow::SetMachine
    close();
}
