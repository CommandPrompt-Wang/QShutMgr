#ifndef FLOATINGBUTTON_H
#define FLOATINGBUTTON_H

#include <QWidget>

namespace Ui {
class FloatingButton;
}

class FloatingButton : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingButton(QWidget *parent = nullptr);
    ~FloatingButton();

private slots:
    void on_But_openMain_clicked();

private:
    Ui::FloatingButton *ui;

signals:
    void mainWindowShowUp();
};

#endif // FLOATINGBUTTON_H
