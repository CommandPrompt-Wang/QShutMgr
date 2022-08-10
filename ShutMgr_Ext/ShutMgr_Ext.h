#ifndef SHUTMGR_EXT_H
#define SHUTMGR_EXT_H

#include <iostream>
#include <string>
using namespace std;
#define MAX_TIME 315360000
#define and &&
#define or ||
#define not !

bool ArgCmp(std::string a, std::string b)
{
    if(a[0] != '-' and a[0] != '/')
    { return false; }
    if(b[0] != '-' and b[0] != '/')
    { return false; }
    if(a.length()!=b.length())
    { return false; }
    for(int it=1;it<a.length();it++)
    {
        if(tolower(a[it])!=tolower(b[it])) return false;
    }
    return true;
}

//modified from: https://blog.csdn.net/lanzhihui_10086/article/details/39995869
int Atoi(const char *str)	//positive number only
{
    int s=0;

    while(*str==' ')
    {
        str++;
    }

    if(not(*str>='0'&&*str<='9'))
    {
        return -1;
    }

    while(*str>='0'&&*str<='9')
    {
        s=s*10+*str-'0';
        str++;
        if(s<0)
        {
            s=2147483647;
            break;
        }
    }
    if(s>MAX_TIME)
        return -1;
    else
        return s;
}

void HelpFunc(void)
{
    cout<<"用法:\n"
              "ShutMgr.Ext [-h(ibernate)|-Sleep|-l(ogout)] [-t xxx]\n"
              "			   [-a]\n"
              "详细说明参见shutdown.exe -?\n";
}

#endif // SHUTMGR_EXT_H
