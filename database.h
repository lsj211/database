#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>
// class Database
// {
// public:
//     QSqlDatabase db;
//     Database();


// };
class Database {
public:
    static Database& getInstance();
    QSqlDatabase& getDb() { return db; }

private:

    Database();
    QSqlDatabase db;
};

#endif // DATABASE_H
