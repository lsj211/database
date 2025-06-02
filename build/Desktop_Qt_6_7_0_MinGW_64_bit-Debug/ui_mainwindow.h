/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *searchbutton;
    QLineEdit *searchtext;
    QPushButton *profile;
    QPushButton *homepage;
    QFrame *dynamic;
    QPushButton *addPort;
    QPushButton *adddish;
    QPushButton *addevaluation;
    QPushButton *changedish;
    QPushButton *deletedish;
    QScrollArea *dynamic1;
    QWidget *widget;
    QLabel *information;
    QScrollArea *recommend;
    QWidget *scrollAreaWidgetContents;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(745, 602);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        searchbutton = new QPushButton(centralwidget);
        searchbutton->setObjectName("searchbutton");
        searchbutton->setGeometry(QRect(350, 30, 101, 21));
        searchtext = new QLineEdit(centralwidget);
        searchtext->setObjectName("searchtext");
        searchtext->setGeometry(QRect(190, 30, 151, 20));
        profile = new QPushButton(centralwidget);
        profile->setObjectName("profile");
        profile->setGeometry(QRect(380, 520, 80, 40));
        homepage = new QPushButton(centralwidget);
        homepage->setObjectName("homepage");
        homepage->setGeometry(QRect(130, 520, 80, 40));
        dynamic = new QFrame(centralwidget);
        dynamic->setObjectName("dynamic");
        dynamic->setGeometry(QRect(610, 470, 20, 131));
        dynamic->setFrameShape(QFrame::StyledPanel);
        dynamic->setFrameShadow(QFrame::Raised);
        addPort = new QPushButton(centralwidget);
        addPort->setObjectName("addPort");
        addPort->setGeometry(QRect(10, 70, 80, 30));
        adddish = new QPushButton(centralwidget);
        adddish->setObjectName("adddish");
        adddish->setGeometry(QRect(10, 70, 80, 30));
        addevaluation = new QPushButton(centralwidget);
        addevaluation->setObjectName("addevaluation");
        addevaluation->setGeometry(QRect(10, 70, 80, 30));
        changedish = new QPushButton(centralwidget);
        changedish->setObjectName("changedish");
        changedish->setGeometry(QRect(10, 70, 80, 30));
        deletedish = new QPushButton(centralwidget);
        deletedish->setObjectName("deletedish");
        deletedish->setGeometry(QRect(10, 110, 80, 30));
        dynamic1 = new QScrollArea(centralwidget);
        dynamic1->setObjectName("dynamic1");
        dynamic1->setGeometry(QRect(110, 70, 441, 431));
        dynamic1->setWidgetResizable(true);
        widget = new QWidget();
        widget->setObjectName("widget");
        widget->setGeometry(QRect(0, 0, 439, 429));
        dynamic1->setWidget(widget);
        information = new QLabel(centralwidget);
        information->setObjectName("information");
        information->setGeometry(QRect(110, 50, 91, 21));
        recommend = new QScrollArea(centralwidget);
        recommend->setObjectName("recommend");
        recommend->setGeometry(QRect(570, 70, 151, 261));
        recommend->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 149, 259));
        recommend->setWidget(scrollAreaWidgetContents);
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(570, 50, 71, 16));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 745, 17));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        searchbutton->setText(QCoreApplication::translate("MainWindow", "\346\220\234\347\264\242", nullptr));
        searchtext->setText(QCoreApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\350\217\234\345\220\215", nullptr));
        profile->setText(QCoreApplication::translate("MainWindow", "\344\270\252\344\272\272\344\277\241\346\201\257", nullptr));
        homepage->setText(QCoreApplication::translate("MainWindow", "\350\277\224\345\233\236\344\270\273\351\241\265", nullptr));
        addPort->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\347\252\227\345\217\243", nullptr));
        adddish->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\350\217\234\345\223\201", nullptr));
        addevaluation->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\350\257\204\350\256\272", nullptr));
        changedish->setText(QCoreApplication::translate("MainWindow", "\344\277\256\346\224\271\350\217\234\345\223\201\344\277\241\346\201\257", nullptr));
        deletedish->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244\350\257\245\350\217\234\345\223\201", nullptr));
        information->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "\350\217\234\345\223\201\346\216\250\350\215\220\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
