#include "configdlg.h"
#include "ui_configdlg.h"

#include <QIntValidator>

ConfigDlg::ConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDlg)
{
    ui->setupUi(this);

    m_strIniPath = INI_PATH;
    m_strJsonPath = JSON_PATH;
    QDir().mkpath(CONFIG_DIR);  //如果不存在，创建配置目录
    ui->lineEditTime->setValidator(new QIntValidator(1, 9999, this));   //时间间隔输入框只接受1-9999整数

    this->setWindowTitle(QString::fromUtf8("配置"));  //窗口标题

    initDisplay();  //初始化显示
}

ConfigDlg::~ConfigDlg()
{
    delete ui;
}

void ConfigDlg::initDisplay()
{
    QFile file(m_strJsonPath);
    if (!file.exists()) {
        QJsonObject defaultConfig;
        QJsonObject configSubObject;
        configSubObject.insert("IP", DEFAULT_IP);
        defaultConfig.insert("config", configSubObject);

        QJsonObject timerSettingSubObject;
        timerSettingSubObject.insert("TimerInterval", DEFAULT_INTERVAL);
        defaultConfig.insert("TimerSetting", timerSettingSubObject);

        QJsonDocument doc(defaultConfig);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "Could not create JSON file" << m_strJsonPath;
            return;
        }
        file.write(doc.toJson());
        file.close();
    }

    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning()<<"打开json文件失败！"<<m_strJsonPath;
        return;
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseErr;
    QJsonDocument doc = QJsonDocument::fromJson(data,&parseErr);
    if(parseErr.error != QJsonParseError::NoError)
    {
        qWarning() << "JSON parse error:" << parseErr.errorString();
        return;
    }

    QJsonObject obj = doc.object();

    //读取IP和时间间隔
    QString ip = obj.value("config").toObject().value("IP").toString(DEFAULT_IP);
    int time = obj.value("TimerSetting").toObject().value("TimerInterval").toInt(DEFAULT_INTERVAL);

    // 显示到UI
    ui->lineEditIP->setText(ip);
    ui->lineEditTime->setText(QString::number(time));
}

/*
 * ini方式
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
*/

void ConfigDlg::on_btnOK_clicked()
{
    // 创建一个QJsonObject来存储配置信息
    QJsonObject configObject;

    // 创建config子对象并添加IP配置
    QJsonObject configSubObject;
    QString ip = ui->lineEditIP->text();
    configSubObject.insert("IP", ip);
    configObject.insert("config", configSubObject);

    // 创建TimerSetting子对象并添加时间间隔配置
    QJsonObject timerSettingSubObject;
    QString strTime = ui->lineEditTime->text().trimmed();
    bool ok = false;
    int nTime = strTime.toInt(&ok);
    if (!ok || nTime <= 0) {
        nTime = DEFAULT_INTERVAL;
    }
    timerSettingSubObject.insert("TimerInterval", nTime);
    configObject.insert("TimerSetting", timerSettingSubObject);

    // 将QJsonObject转换为QJsonDocument
    QJsonDocument doc(configObject);

    // 打开JSON文件进行写入
    QFile file(m_strJsonPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Could not open JSON file" << m_strJsonPath;
        return;
    }
    // 将JSON数据写入文件
    file.write(doc.toJson());
    file.close();

    // 通知主窗口配置已更改
    emit configChanged();
    // 关闭当前窗口
    this->close();
}

void ConfigDlg::on_btnCancel_clicked()
{
    this->close();
}

//从JSON文件读取配置
QJsonObject readJsonConfig(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open JSON file" << filePath;
        return QJsonObject();
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error!= QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return QJsonObject();
    }

    return doc.object();
}
// 将配置写入JSON文件
bool writeJsonConfig(const QString& filePath, const QJsonObject& config)
{
    QJsonDocument doc(config);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Could not open JSON file" << filePath;
        return false;
    }
    file.write(doc.toJson());
    file.close();
    return true;
}
