#include "shutdownpara.h"

ShutdownPara::ShutdownPara()
{
    shutdownType=SHUTDOWN_TYPE::NONE;
    //计算机，默认为本机
    // bMultiMachine=false;
    // computer.append();
    //参数：
    bForce=false;
    bTimeout=false;
    iTimeout=60;
    bToFirmware=false;
    bFastBoot=false;
    bToAdvancedOpt=false;
    bAutoLogin=false;
    bComment=false;
    sComment="";
    bReason=false;
    reasonType=REASON_TYPE::NOREASON;
    majorReason=false;
    minorReason=false;

    condition="";
}

ShutdownPara::ShutdownPara(const ShutdownPara& p)
{
    shutdownType=p.shutdownType;
    //计算机，默认为本机
    // bMultiMachine=false;
    // computer.append();
    //参数：
    bForce=p.bForce;
    bTimeout=p.bTimeout;
    iTimeout=p.iTimeout;
    bToFirmware=p.bToFirmware;
    bFastBoot=p.bFastBoot;
    bToAdvancedOpt=p.bToAdvancedOpt;
    bAutoLogin=p.bAutoLogin;
    bComment=p.bComment;
    sComment=p.sComment;
    bReason=p.bReason;
    reasonType=p.reasonType;
    majorReason=p.majorReason;
    minorReason=p.minorReason;
    condition=p.condition;
}


TCHAR* ShutdownPara::getMachineName(int i){
    static TCHAR retval[256];
    memcpy(retval,computer[i].toStdWString().data(),256*sizeof(TCHAR));
    return retval;
}
TCHAR* ShutdownPara::getMesssage(){
    static TCHAR retval[512];
    memcpy(retval,
           bComment?sComment.toStdWString().data():L"",
           512*sizeof(TCHAR));
    return retval;
}
DWORD ShutdownPara::getTimeout()
{
    return bTimeout?iTimeout:60;
}
DWORD ShutdownPara::getFlags(){
    DWORD dwFlag=0;
    if(shutdownType==SHUTDOWN)
        dwFlag|=SHUTDOWN_POWEROFF;
    if(shutdownType==RESTART)
        dwFlag|=SHUTDOWN_RESTART;
    if(bForce)
        dwFlag|=SHUTDOWN_FORCE_OTHERS;
    if(bFastBoot)
        dwFlag|=SHUTDOWN_HYBRID;
    if(bToFirmware)
    {
        //待实现
        ;
    }
    if(bToAdvancedOpt)
        dwFlag|=0x400;
    if(bAutoLogin)
        dwFlag|=SHUTDOWN_RESTARTAPPS;
    return dwFlag;
}
DWORD ShutdownPara::getReason(){
    DWORD retval=0;
    if(bReason==false)
        return 0;
    else
    {
        switch(reasonType){
        case PLANNED:
            retval|=SHTDN_REASON_FLAG_PLANNED;
            break;
        case CUSTOM:
            retval|=SHTDN_REASON_FLAG_USER_DEFINED;
        case NOREASON:
        case UNPLANNED:
            break;
        };
        retval|=majorReason;
        retval|=minorReason;
    }
    return retval;
}

DWORD ShutdownPara::getComputerListlength(){
    return this->computer.length();
}

void ShutdownPara::addMachine(QString n){
    if(not this->computer.contains(n))
    {
        this->computer.append(n);
    }
    emit this->machineListChanged(computer);
}

void ShutdownPara::removeMachine(QString n){
    {
        this->computer.removeOne(n);
    }
    emit machineListChanged(computer);
}

void ShutdownPara::set(const ShutdownPara* x){
    shutdownType=x->shutdownType;
    //计算机，默认为本机
    // bMultiMachine=false;
    // computer.append();
    //参数：
    bForce=x->bForce;
    bTimeout=x->bTimeout;
    iTimeout=x->iTimeout;
    bToFirmware=x->bToFirmware;
    bFastBoot=x->bFastBoot;
    bToAdvancedOpt=x->bToAdvancedOpt;
    bAutoLogin=x->bAutoLogin;
    bComment=x->bComment;
    sComment=QString(x->sComment);
    bReason=x->bReason;
    reasonType=x->reasonType;
    majorReason=x->majorReason;
    minorReason=x->minorReason;


    this->computer.clear();
    //深拷贝QList
    for (int i=0;i<x->computer.length();i++)
    {
        this->computer.append(x->computer[i]);
    }

    this->condition=x->condition;
}

QJsonObject ShutdownPara::toJsonObject(QString name) const {
    QJsonObject obj;
    obj["presetName"] = name;
    obj["shutdownType"] = int(shutdownType);
    obj["computer"] = QJsonArray::fromStringList(computer);
    obj["bForce"] = bForce;
    obj["bTimeout"] = bTimeout;
    obj["iTimeout"] = iTimeout;
    obj["bToFirmware"] = bToFirmware;
    obj["bFastBoot"] = bFastBoot;
    obj["bToAdvancedOpt"] = bToAdvancedOpt;
    obj["bAutoLogin"] = bAutoLogin;
    obj["bComment"] = bComment;
    obj["sComment"] = sComment;
    obj["bReason"] = bReason;
    obj["reasonType"] = reasonType;
    obj["majorReason"] = majorReason;
    obj["minorReason"] = minorReason;
    obj["condition"] = condition;
    return obj;
}

//返回预配置的名称
QString ShutdownPara::fromJsonObject(const QJsonObject& obj) {
    shutdownType = (SHUTDOWN_TYPE)(obj["shutdownType"].toInt());

    //computer拷贝
    QJsonArray jsComputer=obj["computer"].toArray();
    for(int i=0 ; i< jsComputer.count() ; i++)
    {
        computer.append(jsComputer[i].toString());
    }

    bForce = obj["bForce"].toBool();
    bTimeout = obj["bTimeout"].toBool();
    iTimeout = obj["iTimeout"].toInt();
    bToFirmware = obj["bToFirmware"].toBool();
    bFastBoot = obj["bFastBoot"].toBool();
    bToAdvancedOpt = obj["bToAdvancedOpt"].toBool();
    bAutoLogin = obj["bAutoLogin"].toBool();
    bComment = obj["bComment"].toBool();
    sComment = obj["sComment"].toString();
    bReason = obj["bReason"].toBool();
    reasonType = static_cast<REASON_TYPE>(obj["reasonType"].toInt());
    majorReason = obj["majorReason"].toInt();
    minorReason = obj["minorReason"].toInt();
    condition = obj["condition"].toString();
    return obj["presetName"].toString();
}
