/********************************************************************************
** Form generated from reading UI file 'configdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGDLG_H
#define UI_CONFIGDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ConfigDlg
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEditTime;
    QLabel *label_2;
    QLineEdit *lineEditIP;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOK;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QDialog *ConfigDlg)
    {
        if (ConfigDlg->objectName().isEmpty())
            ConfigDlg->setObjectName(QString::fromUtf8("ConfigDlg"));
        ConfigDlg->resize(436, 286);
        verticalLayout_2 = new QVBoxLayout(ConfigDlg);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(50, 50, 50, 50);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(ConfigDlg);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEditTime = new QLineEdit(ConfigDlg);
        lineEditTime->setObjectName(QString::fromUtf8("lineEditTime"));

        gridLayout->addWidget(lineEditTime, 1, 1, 1, 1);

        label_2 = new QLabel(ConfigDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEditIP = new QLineEdit(ConfigDlg);
        lineEditIP->setObjectName(QString::fromUtf8("lineEditIP"));

        gridLayout->addWidget(lineEditIP, 0, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(horizontalSpacer_4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnOK = new QPushButton(ConfigDlg);
        btnOK->setObjectName(QString::fromUtf8("btnOK"));

        horizontalLayout->addWidget(btnOK);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        btnCancel = new QPushButton(ConfigDlg);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(ConfigDlg);

        QMetaObject::connectSlotsByName(ConfigDlg);
    } // setupUi

    void retranslateUi(QDialog *ConfigDlg)
    {
        ConfigDlg->setWindowTitle(QCoreApplication::translate("ConfigDlg", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("ConfigDlg", "IP", nullptr));
        label_2->setText(QCoreApplication::translate("ConfigDlg", "\346\227\266\351\227\264\351\227\264\351\232\224", nullptr));
        btnOK->setText(QCoreApplication::translate("ConfigDlg", "\347\241\256\345\256\232", nullptr));
        btnCancel->setText(QCoreApplication::translate("ConfigDlg", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigDlg: public Ui_ConfigDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGDLG_H
