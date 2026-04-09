#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>
#include <QSettings>
#include <QDir>
#include <QInputDialog>
#include "global.h"

namespace Ui {
class ConfigDlg;
}

class ConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDlg(QWidget *parent = nullptr);
    ~ConfigDlg();

    void initDisplay(); //初始化显示
    void saveConfigToJson();    //保存到json文件里
    void loadConfigFromJson();  //加载json配置
    void updateTableWidget();   //更新控件

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

    void on_btn_Add_clicked();

    void on_btn_Modify_clicked();

    void on_btn_Delete_clicked();

    void on_btn_Save_clicked();

private:
    Ui::ConfigDlg *ui;


    QJsonObject getNewConfigFromDialog();   //获取到json


    QString m_strIniPath;   //ini文件路径
    QString m_strJsonPath;  //json文件路径
    QList<QJsonObject> ipConfigs; // 存储IP配置信息的列表

signals:
    void configChanged();   //配置更改信号，用于通知主窗口配置已改变

//private slots:


};

#endif // CONFIGDLG_H
