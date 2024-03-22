#include "machinewnd.h"
#include "ui_machinewnd.h"
#include <QHostInfo>

MachineWnd::MachineWnd(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MachineWnd)
{
    ui->setupUi(this);
    ui->List_Computers->setCurrentRow(0);
    ui->Edit_Computer->setFocus();
}

MachineWnd::~MachineWnd()
{
    delete ui;
}

void MachineWnd::on_But_Ok_clicked()
{
    this->close();
}


void MachineWnd::on_But_removeSelected_clicked()
{

    if(this->ui->List_Computers->currentRow()!=-1)
    {
        int row=this->ui->List_Computers->currentRow();
        this->ui->List_Computers->takeItem(row);
        this->machineList.removeAt(row);

        emit machineListChanged(machineList);
    }
}


void MachineWnd::on_Edit_Computer_returnPressed()
{
    QString mname=this->ui->Edit_Computer->text();
    mname=mname.simplified();
    this->ui->Edit_Computer->clear();

    if((not mname.isEmpty()) and (not this->machineList.contains(mname)))
    {
        this->ui->List_Computers->addItem(mname);
        this->machineList.append(mname);
    }

    emit machineListChanged(machineList);
}


void MachineWnd::on_List_Computers_doubleClicked(const QModelIndex &index)
{
    QString name=this->ui->List_Computers->currentItem()->text();
    this->ui->List_Computers->takeItem(index.row());
    this->machineList.removeAt(index.row());
    this->ui->Edit_Computer->setText(name);
    this->ui->Edit_Computer->setFocus();
}


void MachineWnd::on_But_EnterLocalhost_clicked()
{
    this->ui->Edit_Computer->setText(QHostInfo::localHostName());
}


void MachineWnd::on_But_Add_clicked()
{
    on_Edit_Computer_returnPressed();
    this->ui->Edit_Computer->setFocus();

}

