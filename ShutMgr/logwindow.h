#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>
#include <QThread>
#include "ui_logwindow.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace Ui {
class LogWindow;
}

class LogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = nullptr);
    ~LogWindow();
    void /*static */on_Refresh_clicked();
private slots:
    void on_Close_clicked();



    void on_Save_clicked();

private:
    Ui::LogWindow *ui;
};

#endif // LOGWINDOW_H
