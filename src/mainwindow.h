#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QString>
#include <QThread>
#include <QSystemTrayIcon>
#include "configdlg.h"
#include "global.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTimeout();           //定时器超时时间处理
    void onHideToTray();        //隐藏到托盘
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason); //托盘点击恢复
    void onRestore();           //从托盘恢复
    void onQuit();              //托盘退出

    void on_btnHide_clicked();

    void on_btnConfig_clicked();

private:
    QTimer* timer;
    QSystemTrayIcon* trayIcon;  //系统托盘图标指针
    QMenu* trayMenu;            //托盘菜单指针
    ConfigDlg* m_pConfigDlg;

    bool checkIP(const QString& ip);                //检查IP连通性
    void addOneMsg(const QString& info);            //更新ListWidget + 写日志
    void addOneLog(const QString& month, const QString& day, const QString& info);  //写入文件
    bool makeDirectory(const QString& path);        //创建多级目录
    void loadConfig();                              //加载配置
    void reloadConfig();                            //重新加载配置
    void logWithThread(const QString& info);        //带线程ID记录日志的函数

private:
    Ui::MainWindow *ui;

protected:
    void closeEvent(QCloseEvent *event) override;   //重写关闭事件处理函数

public:
    QString m_ip;          //当前监测IP
    int m_interval = 1;    //监测时间间隔/秒
};
#endif // MAINWINDOW_H
