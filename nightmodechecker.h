#ifndef NIGHTMODECHECKER_H
#define NIGHTMODECHECKER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <windows.h>

class NightModeWorker : public QThread
{
    Q_OBJECT

public:
    NightModeWorker() {}

public slots:
    void run() {
        bool isNightModeEnabled = false;

        // 注册表键路径，用于存储夜间模式的设置
        const wchar_t* registryPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
        HKEY hKey = nullptr;

        // 尝试打开注册表键
        if (RegOpenKeyExW(HKEY_CURRENT_USER, registryPath, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
            // 读取夜间模式设置
            DWORD value = 0;
            DWORD size = sizeof(value);

            if (RegQueryValueExW(hKey, L"AppsUseLightTheme", nullptr, nullptr, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
                // 如果值为1，表示夜间模式开启
                isNightModeEnabled = (value == 0);
            }

            RegCloseKey(hKey);
        }

        emit nightModeChanged(isNightModeEnabled);
    }

signals:
    void nightModeChanged(bool enabled);
};

class NightModeChecker : public QThread
{
    Q_OBJECT

public:
    NightModeChecker(QObject *parent = nullptr) : QThread(parent), worker(new NightModeWorker)
    {
        worker->moveToThread(this);
        connect(this, &NightModeChecker::started, worker, &NightModeWorker::run);
        connect(worker, &NightModeWorker::nightModeChanged, this, &NightModeChecker::nightModeChanged);

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, worker, &NightModeWorker::run);
    }

    ~NightModeChecker()
    {
        quit();
        wait();
        delete worker;
    }

public slots:
    void startCheck() {
        if (!isRunning()) {
            start(); // 启动线程
        }
        timer->start(2000); // 开始定时器
    }

    void stopCheck() {
        timer->stop(); // 停止定时器
    }

signals:
    void nightModeChanged(bool enabled);

private:
    NightModeWorker *worker;
    QTimer *timer;
};

#endif // NIGHTMODECHECKER_H
