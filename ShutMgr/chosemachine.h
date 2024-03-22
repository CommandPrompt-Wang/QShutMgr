#ifndef CHOSEMACHINE_H
#define CHOSEMACHINE_H

#include <QWidget>
#include <QMessageBox>

#pragma execution_character_set("utf-8")

namespace Ui {
class ChoseMachine;
}

class ChoseMachine : public QWidget
{
    Q_OBJECT

public:
    explicit ChoseMachine(QWidget *parent = nullptr);
    ~ChoseMachine();

private slots:
    void on_pushButton_clicked();

    void on_AddMachine_clicked();

    void on_RemoveMachine_clicked();

private:
    Ui::ChoseMachine *ui;
};

#endif // CHOSEMACHINE_H
