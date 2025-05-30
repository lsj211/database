#pragma once
#ifndef LOG_H
#define LOG_H
#include<QString>
#include <QWidget>
#include "database.h"
#include <QMessageBox>


#include "mainwindow.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class Log;
}
QT_END_NAMESPACE

class Log : public QWidget
{
    Q_OBJECT
public:
    explicit Log(QWidget *parent = nullptr);
    ~Log();
    bool eventFilter(QObject *obj, QEvent *event) override;
    MainWindow *mainwindow;
private slots:
    void on_user_selectionChanged();

    void on_log_clicked();

    void on_newregister_clicked();

    void on_register_link_clicked();

    void on_log_link_clicked();

private:
    Ui::Log *ui;
};


#endif // LOG_H
