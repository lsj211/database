/********************************************************************************
** Form generated from reading UI file 'log.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOG_H
#define UI_LOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Log
{
public:
    QLabel *label;
    QLineEdit *user;
    QLineEdit *password;
    QPushButton *log;
    QPushButton *newregister;
    QCommandLinkButton *register_link;
    QCheckBox *manage;
    QCommandLinkButton *log_link;

    void setupUi(QWidget *Log)
    {
        if (Log->objectName().isEmpty())
            Log->setObjectName("Log");
        Log->resize(590, 517);
        label = new QLabel(Log);
        label->setObjectName("label");
        label->setGeometry(QRect(190, 100, 181, 20));
        QFont font;
        font.setPointSize(16);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        user = new QLineEdit(Log);
        user->setObjectName("user");
        user->setGeometry(QRect(222, 180, 131, 20));
        password = new QLineEdit(Log);
        password->setObjectName("password");
        password->setGeometry(QRect(222, 230, 131, 20));
        log = new QPushButton(Log);
        log->setObjectName("log");
        log->setGeometry(QRect(230, 310, 120, 27));
        newregister = new QPushButton(Log);
        newregister->setObjectName("newregister");
        newregister->setGeometry(QRect(230, 310, 120, 27));
        register_link = new QCommandLinkButton(Log);
        register_link->setObjectName("register_link");
        register_link->setGeometry(QRect(180, 260, 111, 31));
        QFont font1;
        font1.setPointSize(7);
        register_link->setFont(font1);
        register_link->setStyleSheet(QString::fromUtf8("background-color: transparent; border: none;"));
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("accessories-calculator")));
        register_link->setIcon(icon);
        register_link->setIconSize(QSize(0, 0));
        manage = new QCheckBox(Log);
        manage->setObjectName("manage");
        manage->setGeometry(QRect(320, 265, 61, 18));
        log_link = new QCommandLinkButton(Log);
        log_link->setObjectName("log_link");
        log_link->setGeometry(QRect(180, 260, 111, 31));
        log_link->setFont(font1);
        log_link->setStyleSheet(QString::fromUtf8("background-color: transparent; border: none;"));
        log_link->setIconSize(QSize(0, 0));

        retranslateUi(Log);

        QMetaObject::connectSlotsByName(Log);
    } // setupUi

    void retranslateUi(QWidget *Log)
    {
        Log->setWindowTitle(QCoreApplication::translate("Log", "Log", nullptr));
        label->setText(QCoreApplication::translate("Log", "\351\243\237\345\240\202\346\216\250\350\215\220\347\263\273\347\273\237", nullptr));
        user->setText(QCoreApplication::translate("Log", "\350\257\267\350\276\223\345\205\245\350\264\246\345\217\267", nullptr));
        password->setText(QCoreApplication::translate("Log", "\350\257\267\350\276\223\345\205\245\345\257\206\347\240\201", nullptr));
        log->setText(QCoreApplication::translate("Log", "\347\231\273\345\275\225", nullptr));
        newregister->setText(QCoreApplication::translate("Log", "\346\263\250\345\206\214", nullptr));
        register_link->setText(QCoreApplication::translate("Log", "\346\226\260\347\224\250\346\210\267\350\257\267\347\202\271\345\207\273\346\255\244\346\263\250\345\206\214", nullptr));
        manage->setText(QCoreApplication::translate("Log", "\347\256\241\347\220\206\345\221\230", nullptr));
        log_link->setText(QCoreApplication::translate("Log", "\350\277\224\345\233\236\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Log: public Ui_Log {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOG_H
