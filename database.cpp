#include "database.h"

// Database::Database() {
//     db = QSqlDatabase::addDatabase("QMYSQL");
//     db.setHostName("localhost");
//     db.setPort(3306);
//     db.setDatabaseName("test1");
//     db.setUserName("root");
//     db.setPassword("20050721");
// }

Database::Database()
    : db(QSqlDatabase::addDatabase("QMYSQL", "SingletonConnection"))
{

    const_cast<QSqlDatabase&>(db).setHostName("localhost");
    const_cast<QSqlDatabase&>(db).setPort(3306);
    const_cast<QSqlDatabase&>(db).setDatabaseName("test1");
    const_cast<QSqlDatabase&>(db).setUserName("root");
    const_cast<QSqlDatabase&>(db).setPassword("20050721");
    if (!db.open()) {
        qCritical() << "数据库连接失败:" << db.lastError().text();

    }
}


 Database& Database::getInstance() {
    static Database instance;
    return instance;
}
