#ifndef CONFIGDLG_H
#define CONFIGDLG_H

#include <QDialog>
#include <QSettings>
#include <QDir>
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
private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

private:
    Ui::ConfigDlg *ui;


    QString m_strIniPath;  //ini文件路径

signals:
    void configChanged();   //

//private slots:


};

#endif // CONFIGDLG_H
