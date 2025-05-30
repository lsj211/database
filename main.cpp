#include "mainwindow.h"
#include <QtSql>
#include <QApplication>
#include "database.h"
#include "log.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // Widget c;
    // c.show();
    // w.show();
    Log *g;
    g=new Log();
    g->show();
    return a.exec();
}
