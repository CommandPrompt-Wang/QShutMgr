#ifndef SHUTMGRWND_H
#define SHUTMGRWND_H

#include <QMainWindow>
#include "logwnd.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class ShutMgrWnd;
}
QT_END_NAMESPACE



class ShutMgrWnd : public QMainWindow
{
    Q_OBJECT

public:
    ShutMgrWnd(QWidget *parent = nullptr);
    ~ShutMgrWnd();

private slots:

    void on_BExit_clicked();

    void on_BViewLog_clicked();

    void on_RBShutdown_clicked();

    void on_RBRestart_clicked();

    void on_BExecute_clicked();

    void moveEvent(QMoveEvent *event) override;

private:
    Ui::ShutMgrWnd *ui;
    LogWnd * logWindow;
};
#endif // SHUTMGRWND_H
