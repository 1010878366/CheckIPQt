#include "mainwindow.h"
#include "ui_mainwindow.h"

// #ifdef _WIN32
// #define WIN32_LEAN_AND_MEAN
// #define _WINSOCKAPI_   // 防止 Windows.h 引入旧版 winsock.h

//#include <Windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <iphlpapi.h>
//#include <icmpapi.h>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QIcon>
#include <QAction>
#include <QCloseEvent>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromUtf8("IP连接监测 V2.0.5.0"));

    QDir().mkpath(CONFIG_DIR);      //创建配置目录
    QDir().mkpath(LOG_DIR);         //创建日志目录
    loadConfig();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    timer->start(m_interval*1000);

    trayMenu = new QMenu(this);
    trayIcon = new QSystemTrayIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon), this); //系统默认图标，后期更换
    QAction* restoreAction = new QAction("Restore", this);
    QAction* quitAction = new QAction("Quit", this);
    trayMenu->addAction(restoreAction);
    trayMenu->addAction(quitAction);

    connect(restoreAction, &QAction::triggered, this, &MainWindow::onRestore);
    connect(quitAction, &QAction::triggered, this, &MainWindow::onQuit);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);

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
    for(const QString& ip : m_ipList)
    {
        QtConcurrent::run(&MainWindow::checkIpWorker, this, ip);
    }
}

bool MainWindow::checkIP(const QString& ip)
{
#ifdef Q_OS_WIN

    QString program = "ping";
    QStringList arguments;

    arguments << "-n" << "1"
              << "-w" << "500"
              << ip;

#else

    QString program = "ping";
    QStringList arguments;

    arguments << "-c" << "1"
              << "-W" << "1"
              << ip;

#endif

    QProcess process;

    process.start(program, arguments);

    process.start(program, arguments);

    if(!process.waitForFinished(2000))
    {
        process.kill();
        process.waitForFinished();
        return false;
    }

    return process.exitCode() == 0;
}

//日志+显示
void MainWindow::addOneMsg(const QString& info)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString fullInfo = timestamp + "   " + info;

    //更新ListWidget
    ui->listWidget->addItem(fullInfo);
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);

    //按月/日写日志
    QString month = QDateTime::currentDateTime().toString("yyyy-MM");
    QString day = QDateTime::currentDateTime().toString("dd");
    addOneLog(month, day, fullInfo);
}

//写日志到文件
void MainWindow::addOneLog(const QString& month, const QString& day, const QString& info)
{
    /*//先不写日志
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
    */
}

//创建多级目录
bool MainWindow::makeDirectory(const QString& path)
{
    QDir dir;
    if (!dir.exists(path))
    {
        return dir.mkpath(path);    //自动创建多级目录
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
    QFile file(JSON_PATH);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject root = doc.object();

    // 读取 IP 列表
    m_ipList.clear();
    QJsonArray ipArray = root.value("ipList").toArray();

    for (auto v : ipArray)
    {
        QString ip = v.toObject().value("IP").toString().trimmed();
        if (!ip.isEmpty())
            m_ipList.append(ip);
    }

    if (m_ipList.isEmpty())
        m_ipList << DEFAULT_IP;

    // 时间写死
    m_interval = 2;
}

void MainWindow::reloadConfig()
{
    loadConfig();

    timer->stop();
    timer->start(m_interval * 1000);

    onTimeout();    //立刻检测一次

    logWithThread("配置更新");
    QString ipListStr = m_ipList.join(", ");
    addOneMsg(QString("配置已更新：IP=%1 间隔=%2秒")
              .arg(ipListStr)
              .arg(m_interval));
}

void MainWindow::logWithThread(const QString& info)
{
    QString threadId = QString::number((quintptr)QThread::currentThreadId());
    addOneMsg(QString("[TID:%1] %2").arg(threadId).arg(info));
}

void MainWindow::checkIpWorker(QString ip)
{
    QString threadId = QString::number((quintptr)QThread::currentThreadId());

    bool ok = checkIP(ip);

    QString msg = ok ? "连通" : "ERR，连接失败！";

    // 切回UI线程
    QMetaObject::invokeMethod(this,[=]()
                              {
                                addOneMsg(QString("[TID:%1] %2 %3").arg(threadId).arg(ip).arg(msg));
                              },
                              Qt::QueuedConnection);
}
