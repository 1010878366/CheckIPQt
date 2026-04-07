#include "configdlg.h"
#include "ui_configdlg.h"

#include <QIntValidator>

ConfigDlg::ConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDlg)
{
    ui->setupUi(this);

    m_strIniPath = INI_PATH;
    QDir().mkpath(CONFIG_DIR);
    ui->lineEditTime->setValidator(new QIntValidator(1, 9999, this));

    this->setWindowTitle(QString::fromUtf8("配置"));

    initDisplay();
}

ConfigDlg::~ConfigDlg()
{
    delete ui;
}

void ConfigDlg::initDisplay()
{
    QSettings settings(m_strIniPath,QSettings::IniFormat);

    //读取配置
    QString ip = settings.value("config/IP",DEFAULT_IP).toString();
    int time = settings.value("TimerSetting/TimerInterval",DEFAULT_INTERVAL).toInt();

    //显示到UI
    ui->lineEditIP->setText(ip);
    ui->lineEditTime->setText(QString::number(time));
}

void ConfigDlg::on_btnOK_clicked()
{
    QSettings settings(m_strIniPath, QSettings::IniFormat);

    QString ip = ui->lineEditIP->text();
    QString strTime = ui->lineEditTime->text().trimmed();
    bool ok = false;
    int nTime = strTime.toInt(&ok);
    if (!ok || nTime <= 0)
    {
        nTime = DEFAULT_INTERVAL;
    }

    settings.setValue("config/IP", ip);
    settings.setValue("TimerSetting/TimerInterval", nTime);

    settings.sync(); // 确保写入

    emit configChanged();   //通知主窗口

    this->close();          //关闭窗口
}

void ConfigDlg::on_btnCancel_clicked()
{
    this->close();
}
