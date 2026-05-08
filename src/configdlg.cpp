#include "configdlg.h"
#include "ui_configdlg.h"

#include <QIntValidator>
#include <QList>
#include <QSpinBox>
#include <QMessageBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QHeaderView>

ConfigDlg::ConfigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDlg)
{
    ui->setupUi(this);

    //m_strIniPath = INI_PATH;
    m_strJsonPath = JSON_PATH;
    QDir().mkpath(CONFIG_DIR);  //如果不存在，创建配置目录
    //ui->lineEditTime->setValidator(new QIntValidator(1, 9999, this));   //时间间隔输入框只接受1-9999整数

    this->setWindowTitle(QString::fromUtf8("配置"));  //窗口标题

    initDisplay();  //初始化显示
    loadConfigFromJson();
    updateTableWidget();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    //默认不可编辑
    m_isEditing = false;
    setTableEditable(false);
}

ConfigDlg::~ConfigDlg()
{
    delete ui;
}

void ConfigDlg::initDisplay()
{
    QFile file(m_strJsonPath);

    if (!file.exists())
    {
        QJsonObject root;

        // global
        QJsonObject global;
        global.insert("TimerInterval", 1);
        root.insert("global", global);

        // ipList
        QJsonArray ipArray;

        QJsonObject obj1;
        obj1.insert("IP", DEFAULT_IP);
        ipArray.append(obj1);

        root.insert("ipList", ipArray);

        QJsonDocument doc(root);

        if (!file.open(QIODevice::WriteOnly))
        {
            qWarning() << "create json failed";
            return;
        }

        file.write(doc.toJson());
        file.close();
    }
}

void ConfigDlg::on_btnOK_clicked()
{

    // 创建一个QJsonObject来存储配置信息
    QJsonObject configObject;

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

void ConfigDlg::loadConfigFromJson()
{
    QFile file(m_strJsonPath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    ipConfigs.clear();

    QJsonObject root = doc.object();
    QJsonArray ipArray = root.value("ipList").toArray();

    for (auto v : ipArray)
    {
        QJsonObject obj = v.toObject();

        QJsonObject config;
        config.insert("IP", obj.value("IP").toString());

        ipConfigs.append(config);
    }
}

void ConfigDlg::updateTableWidget()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(ipConfigs.size());
    ui->tableWidget->setColumnCount(1);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "IP地址");

    for (int i = 0; i < ipConfigs.size(); ++i)
    {
        QString ip = ipConfigs.at(i).value("IP").toString();

        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(ip));
    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ConfigDlg::saveConfigToJson()
{
    QJsonObject root;

    // global
    QJsonObject global;
    global.insert("TimerInterval", 1);  //先写死，后面再改
    root.insert("global", global);

    // ipList
    QJsonArray ipArray;

    for (const QJsonObject& config : ipConfigs)
    {
        QJsonObject obj;
        obj.insert("IP", config.value("IP").toString());
        ipArray.append(obj);
    }

    root.insert("ipList", ipArray);

    QJsonDocument doc(root);

    QFile file(m_strJsonPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(doc.toJson());
        file.close();
    }
}

QJsonObject ConfigDlg::getNewConfigFromDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加IP");

    QFormLayout layout(&dialog);

    QLineEdit ipEdit;
    layout.addRow("IP地址:", &ipEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout.addWidget(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return QJsonObject();

    QString ip = ipEdit.text().trimmed();

    if(ip.isEmpty())
        return QJsonObject();

    QJsonObject obj;
    obj.insert("IP", ip);

    return obj;
}

void ConfigDlg::on_btn_Add_clicked()
{
    QJsonObject newConfig = getNewConfigFromDialog();
    if(newConfig.isEmpty())
        return;

    QString newIp = newConfig.value("IP").toString();

    // 防止重复
    for(const QJsonObject& cfg : ipConfigs)
    {
        if(cfg.value("IP").toString() == newIp)
        {
            QMessageBox::warning(this, "提示", "IP已存在！");
            return;
        }
    }

    ipConfigs.append(newConfig);
    updateTableWidget();
}


void ConfigDlg::on_btn_Modify_clicked()
{
    m_isEditing = !m_isEditing;

    setTableEditable(m_isEditing);

}


void ConfigDlg::on_btn_Delete_clicked()
{
    int row = ui->tableWidget->currentRow();

    if(row < 0)
    {
        QMessageBox::warning(this, "提示", "请先选中一行");
        return;
    }

    //删数据
    if(row < ipConfigs.size())
    {
        ipConfigs.removeAt(row);
    }

    //删UI
    ui->tableWidget->removeRow(row);
}


void ConfigDlg::on_btn_Save_clicked()
{
    syncTableToData();   // UI → 数据
    saveConfigToJson();  // 数据 → JSON
    //QMessageBox::information(this, "提示", "保存成功");
    m_isEditing = false;
    setTableEditable(false);
    emit configChanged();   // 通知主窗口
    this->accept();

}

void ConfigDlg::on_btn_Cancel_clicked()
{
    this->accept();
}

void ConfigDlg::syncTableToData()
{
    ipConfigs.clear();

    int rowCount = ui->tableWidget->rowCount();

    for(int i = 0; i < rowCount; ++i)
    {
        QTableWidgetItem* item = ui->tableWidget->item(i, 0);
        if(!item) continue;

        QString ip = item->text().trimmed();
        if(ip.isEmpty()) continue;

        QJsonObject obj;
        obj.insert("IP", ip);

        ipConfigs.append(obj);
    }
}

void ConfigDlg::setTableEditable(bool editable)
{
    if(editable)
    {
        ui->tableWidget->setEditTriggers(
            QAbstractItemView::DoubleClicked);
    }
    else
    {
        ui->tableWidget->setEditTriggers(
            QAbstractItemView::NoEditTriggers);
    }

    refreshTableColor(editable);
}

void ConfigDlg::refreshTableColor(bool editable)
{
    QColor bgColor;

    if(editable)
        bgColor = Qt::white;
    else
        bgColor = QColor(240,240,240);

    for(int row = 0; row < ui->tableWidget->rowCount(); ++row)
    {
        for(int col = 0; col < ui->tableWidget->columnCount(); ++col)
        {
            QTableWidgetItem* item =
                ui->tableWidget->item(row, col);

            if(item)
            {
                item->setBackground(bgColor);
            }
        }
    }
}
