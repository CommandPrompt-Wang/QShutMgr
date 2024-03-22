#include "floatingbutton.h"
#include "ui_floatingbutton.h"

FloatingButton::FloatingButton(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FloatingButton)
{
    ui->setupUi(this);
}

FloatingButton::~FloatingButton()
{
    delete ui;
}

void FloatingButton::on_But_openMain_clicked()
{
    emit mainWindowShowUp();
}

