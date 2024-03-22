#ifndef LOGWND_H
#define LOGWND_H

#include <QDialog>
#include <QObject>
#include <qdatetime.h>
#include "ui_logwnd.h"

namespace Ui {
class LogWnd;
}

class LogWnd : public QDialog
{
    Q_OBJECT

public:
    explicit LogWnd(QWidget *parent = nullptr);
    ~LogWnd();
    // Ui::LogWnd *ui;
public slots:
    void appendlog(QString x){
//        ui->TLog->append();
        ui->TLog->append(QDateTime::currentDateTime().toString()+tr("\t"));
        ui->TLog->append(x+tr("\n"));
    }
private slots:
    void on_PBSaveLog_clicked();

    void on_PBCancelLog_clicked();

private:
    Ui::LogWnd *ui;
};

#endif // LOGWND_H
