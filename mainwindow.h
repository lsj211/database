#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QCommandLinkButton>
#include <QMainWindow>
#include<QVBoxLayout>
#include<QPushButton>
#include<QMessageBox>
#include "database.h"
#include<QLineEdit>
#include<QLabel>
#include<QListWidget>
#include<QTextEdit>
#include<QInputDialog>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class Log;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    bool ismanager=false;
    int userId;
    int canteenselect;
    int portselect;
    int dishselect;
    MainWindow(QWidget *parent = nullptr,bool b=false,int id=0);
    ~MainWindow();
    bool eventFilter(QObject *object,QEvent *event) override;
    void clearcontainer(QFrame *frame);
    void canteenPage();
    void portPage(int id);
    void managePage(int id);
    void dishPage(int id);
    void evaluationPage(int id);
    Log *log;


private slots:
    void on_searchbutton_clicked();

    void on_addPort_clicked();

    void on_adddish_clicked();

    void on_addevaluation_clicked();


    void on_profile_clicked();

    void on_changedish_clicked();

    void on_homepage_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
