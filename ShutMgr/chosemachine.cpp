#include "chosemachine.h"
#include "ui_chosemachine.h"
#include "ui_mainwindow.h"
#include <iostream>

extern QList<QString> MachineNameList;              //全局变量用于窗口通讯

ChoseMachine::ChoseMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChoseMachine)
{
    ui->setupUi(this);
    for(int i=0;i<MachineNameList.length();i++)     //读取(之前)指定的计算机
    {
        ui->MachineList->addItem(MachineNameList[i]);
    }
    ui->MachineEntering->setFocus();    //设置输入框为焦点，方便用户输入
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
        QMessageBox::critical(this, tr("错误"),
                              tr("计算机名不应是空的"),
                              QMessageBox::Ok);
        return;
    }
    if(AddMachineName.indexOf(" ",Qt::CaseInsensitive)!=-1)
    {
        QMessageBox::critical(this, tr("错误"),
                              tr("计算机名不应包含空白"),
                              QMessageBox::Ok);
        return;
    }
    if(!ui->MachineList->findItems(AddMachineName,Qt::MatchCaseSensitive).isEmpty())
    {
        QMessageBox::warning(this, tr("警告"),
                              tr("已添加指定的计算机名"),
                              QMessageBox::Ok);
        return;
    }
    ui->MachineList->addItem(AddMachineName);
    ui->MachineEntering->clear();
    ui->MachineList->sortItems();
    ui->MachineEntering->setFocus();    //设置输入框为焦点，方便用户输入
}

void ChoseMachine::on_RemoveMachine_clicked()
{
    delete ui->MachineList->currentItem();
    ui->MachineEntering->setFocus();    //设置输入框为焦点，方便用户输入
}

void ChoseMachine::on_pushButton_clicked() //OK (忘改名字了)
{
    if(ui->MachineList->count()==0)
    {
        QMessageBox::warning(this, tr("警告"),
                              tr("未指定任何一个计算机"),
                              QMessageBox::Ok);
    }
    MachineNameList.clear();
    for(int i=0;i<(ui->MachineList->count());i++)
    {

        MachineNameList.append(ui->MachineList->item(i)->text());
//        QMessageBox::information(this,tr(""),
//                                 ui->MachineList->item(i)->text());
//        ↑调试语句
    }
//    Ui::MainWindow::SetMachine
    close();
}
