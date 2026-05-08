// #ifndef GLOBAL_H
// #define GLOBAL_H


// #include <QString>
// #include <QFile>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>

// static const QString CONFIG_DIR = "E:/CheckIP/config/";
// static const QString INI_PATH = "E:/CheckIP/config/IPconfig.ini";
// static const QString JSON_PATH = "E:/CheckIP/config/IPconfig.json";
// static const QString LOG_DIR  = "E:/CheckIP/log/";

// #define DEFAULT_IP          "192.168.7.11"
// #define DEFAULT_INTERVAL    2


// #endif // GLOBAL_H



#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>

// 程序配置根目录
static const QString APP_DIR =
    QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

// 配置目录
static const QString CONFIG_DIR =
    APP_DIR + "/config/";

// 日志目录
static const QString LOG_DIR =
    APP_DIR + "/log/";

// 配置文件
static const QString JSON_PATH =
    CONFIG_DIR + "IPconfig.json";

#define DEFAULT_IP          "192.168.7.11"
#define DEFAULT_INTERVAL    3

#endif // GLOBAL_H
