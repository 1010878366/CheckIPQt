#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_   // 防止 Windows.h 引入旧版 winsock.h

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QIcon>
#include <QAction>
#include <QCloseEvent>
#include <QtDebug>
//#include <QTextCodec>
//#pragma execution_character_set("utf-8")

#pragma comment(lib, "iphlpapi.lib")
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromUtf8("IP连接监测 V2.0.1"));

    qApp->setQuitOnLastWindowClosed(false);

    QDir().mkpath(CONFIG_DIR);
    QDir().mkpath(LOG_DIR);
    loadConfig();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    timer->start(m_interval*1000);

    trayMenu = new QMenu(this);
    trayIcon = new QSystemTrayIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon), this); //系统默认图标，后期更换
    trayIcon->setToolTip(QString::fromUtf8("IP监测程序"));
    QAction* restoreAction = new QAction("Restore", this);
    QAction* quitAction = new QAction("Quit", this);
    trayMenu->addAction(restoreAction);
    trayMenu->addAction(quitAction);

    connect(restoreAction, &QAction::triggered, this, &MainWindow::onRestore);
    connect(quitAction, &QAction::triggered, this, &MainWindow::onQuit);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    //connect(m_pConfigDlg, &ConfigDlg::configChanged,this, &MainWindow::reloadConfig);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    m_pConfigDlg = nullptr;

    logWithThread("程序启动。");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTimeout()
{
    QString ip = m_ip;
    //int time = m_interval;

    bool ok = checkIP(ip);

    QString msg = ok ? "连通" : "ERR，连接失败！";

    addOneMsg(ip + " " + msg);
}

bool MainWindow::checkIP(const QString& ip)
{
#ifdef _WIN32
    HANDLE hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE)
        return false;

    sockaddr_in sa{};
    if (InetPton(AF_INET, ip.toStdWString().c_str(), &sa.sin_addr.s_addr) != 1)
    {
        IcmpCloseHandle(hIcmpFile);
        return false;
    }

    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + 8;
    std::vector<char> buffer(replySize);

    DWORD ret = IcmpSendEcho(
        hIcmpFile,
        sa.sin_addr.s_addr,
        nullptr,
        0,
        nullptr,
        buffer.data(),
        replySize,
        500);

    IcmpCloseHandle(hIcmpFile);

    return ret != 0;
#else
    return false;
#endif
}

// 日志 + 显示
void MainWindow::addOneMsg(const QString& info)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString fullInfo = timestamp + "   " + info;

    // 更新ListWidget
    ui->listWidget->addItem(fullInfo);
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);

    // 按月/日写日志
    QString month = QDateTime::currentDateTime().toString("yyyy-MM");
    QString day = QDateTime::currentDateTime().toString("dd");
    addOneLog(month, day, fullInfo);
}

// 写日志到文件
void MainWindow::addOneLog(const QString& month, const QString& day, const QString& info)
{
    QString basePath = QString(LOG_DIR) + month + "/";
    makeDirectory(basePath);

    QString filePath = basePath + day + "log.txt";

    QFile file(filePath);
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << info << "\r\n";
        file.close();
    }
}

// 创建多级目录
bool MainWindow::makeDirectory(const QString& path)
{
    QDir dir;
    if (!dir.exists(path))
    {
        return dir.mkpath(path); // 自动创建多级目录
    }
    return true;
}

void MainWindow::onHideToTray()
{

    this->hide();
    //trayIcon->showMessage(QString::fromUtf8("IP监测"), QString::fromUtf8("程序已最小化到托盘"), QSystemTrayIcon::Information, 2000);

    qDebug() << "hide called";
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) // 双击恢复
    {
        this->show();
        this->activateWindow();
    }
}

void MainWindow::onRestore()
{
    this->show();
    this->activateWindow();
}

void MainWindow::onQuit()
{
    trayIcon->hide();
    qApp->quit();
}


void MainWindow::on_btnHide_clicked()
{
    logWithThread("程序隐藏到托盘");
    onHideToTray();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    logWithThread("程序退出");
    trayIcon->hide();
    event->accept();
}

void MainWindow::on_btnConfig_clicked()
{
    logWithThread("打开配置窗口");
    if(m_pConfigDlg == nullptr)
    {
        m_pConfigDlg = new ConfigDlg(this);
        connect(m_pConfigDlg, &ConfigDlg::configChanged,this, &MainWindow::reloadConfig);
    }
    m_pConfigDlg->show();
    m_pConfigDlg->raise();
    m_pConfigDlg->activateWindow();
}

void MainWindow::loadConfig()
{
    QString iniPath = INI_PATH;
    QSettings ini(iniPath,QSettings::IniFormat);

    // 读取IP
    m_ip = ini.value("config/IP",DEFAULT_IP).toString().trimmed();
    if(m_ip.isEmpty())
        m_ip = DEFAULT_IP;
    bool ok = false;
    int nInterval = ini.value("TimerSetting/TimerInterval", 3).toInt(&ok);
    if(!ok || nInterval <= 0)
        nInterval = DEFAULT_INTERVAL;
    m_interval = nInterval;

}

void MainWindow::reloadConfig()
{
    loadConfig();

    timer->stop();
    timer->start(m_interval * 1000);

    onTimeout();    //立刻检测一次

    logWithThread("配置更新");
    addOneMsg(QString("配置已更新：IP=%1 间隔=%2秒")
              .arg(m_ip)
              .arg(m_interval));
}

void MainWindow::logWithThread(const QString& info)
{
    QString threadId = QString::number((quintptr)QThread::currentThreadId());
    addOneMsg(QString("[TID:%1] %2").arg(threadId).arg(info));
}
