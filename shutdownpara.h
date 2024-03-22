#ifndef SHUTDOWNPARA_H
#define SHUTDOWNPARA_H

#include <QString>
#include <QHostInfo>
#include <windows.h>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

enum SHUTDOWN_TYPE : unsigned int {NONE, SHUTDOWN, RESTART, SLEEP, HIBERNATE,
                                   LOCK, LOGOUT, SHUTDOWN_P, CANCEL};

enum REASON_TYPE {PLANNED, UNPLANNED, CUSTOM, NOREASON};
class ShutdownPara : public QObject
{
    Q_OBJECT
private:

public:
    //模式
    SHUTDOWN_TYPE shutdownType;
    //计算机
    // bool bMultiMachine;
    QList<QString> computer;
    //参数：
    bool bForce;

    bool bTimeout;
    int iTimeout;

    bool bToFirmware;
    bool bFastBoot;
    bool bToAdvancedOpt;
    bool bAutoLogin;

    bool bComment;
    QString sComment;


    bool bReason;
    REASON_TYPE reasonType;
    std::uint8_t majorReason;
    std::uint16_t minorReason;

    QString condition;

    ShutdownPara();
    ShutdownPara(const ShutdownPara& p);

    TCHAR* getMachineName(int i);

    TCHAR* getMesssage();

    DWORD getTimeout();

    DWORD getFlags();

    DWORD getReason();

    DWORD getComputerListlength();

    void addMachine(QString n);

    void removeMachine(QString n);

    void set(const ShutdownPara *x);

    QJsonObject toJsonObject(QString name) const;

    QString fromJsonObject(const QJsonObject& obj);



signals:
    void machineListChanged(QList<QString>);

public slots:
    void updateMachineList(QList<QString> x){
        //QList深拷贝
        this->computer.clear();
        for(int i=0;i<x.length();i++)
        {
            this->computer.append(x[i]);
        }
    }

};


#endif // SHUTDOWNPARA_H
