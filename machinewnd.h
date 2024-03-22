#ifndef MACHINEWND_H
#define MACHINEWND_H

#include <QWidget>
#include "ui_machinewnd.h"

namespace Ui {
class MachineWnd;
}

class MachineWnd : public QWidget
{
    Q_OBJECT

public:
    explicit MachineWnd(QWidget *parent = nullptr);
    ~MachineWnd();

private:
    Ui::MachineWnd *ui;
    QList<QString> machineList;

signals:
    void machineListChanged(QList<QString> x);

public slots:
    // void addMachine(QString x){
    //     if(not this->machineList.contains(x))
    //         this->machineList.append(x);
    // }
    void removeMachine(QString x){
        this->machineList.removeOne(x);
    }
    void updateMachineList(QList<QString> x)
    {
        //QList不能直接等，要深拷贝来着
        this->machineList.clear();
        this->ui->List_Computers->clear();
        for(int i=0;i<x.length();i++)
        {
            this->machineList.append(x[i]);
            this->ui->List_Computers->addItem(x[i]);

            // QModelIndex index=this->ui->List_Computers->indexAt(QPoint(2,i));
            // QToolButton * But_delete=new QToolButton;
            //连接按钮与删除操作
            // connect(But_delete,&(But_delete->clicked),this,[&](){
                // this->ui->List_Computers->takeItem(i);
            // });
            // But_delete->setText("×");
            // this->ui->List_Computers->setIndexWidget(index,But_delete);
        }
    }

private slots:
    void on_But_Ok_clicked();
    void on_But_removeSelected_clicked();
    void on_Edit_Computer_returnPressed();
    // void on_List_Computers_itemChanged(QListWidgetItem *item);
    void on_List_Computers_doubleClicked(const QModelIndex &index);
    void on_But_EnterLocalhost_clicked();
    void on_But_Add_clicked();
};

#endif // MACHINEWND_H
