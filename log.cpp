#include "log.h"
#include "ui_log.h"

Log::Log(QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::Log)
{
    ui->setupUi(this);
    this->setWindowTitle("登录");
    this->setFixedSize(590,517);
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();
    ui->user->installEventFilter(this);
    ui->password->installEventFilter(this);
    ui->newregister->hide();
    ui->log_link->hide();
    // mainwindow=new MainWindow();
}


Log::~Log()
{
    delete ui;
}

void Log::on_user_selectionChanged()
{
    if(ui->user->hasFocus())
    {
        ui->user->clear();
    }
}



bool Log::eventFilter(QObject *obj, QEvent *event) {
    // 1. 检查事件是否来自目标文本框 ui->user
    if (obj == ui->user) {
        if (event->type() == QEvent::FocusIn) {
                ui->user->clear();
            return false;
        }
        if (event->type() == QEvent::FocusOut) {
            if(ui->user->text()=="")
            {
                ui->user->setText("请输入账号");
            }

            return false;
        }
    }
    if (obj == ui->password) {
        if (event->type() == QEvent::FocusIn) {
            ui->password->clear();
            return false;
        }
        if (event->type() == QEvent::FocusOut) {
            if(ui->password->text()=="")
            {
                ui->password->setText("请输入密码");
            }

            return false;
        }
    }
    // 其他事件交给父类处理
    return QWidget::eventFilter(obj, event);
}


void Log::on_log_clicked()
{
    QString username=ui->user->text().trimmed();
    QString Password=ui->password->text().trimmed();
    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);
    if(ui->manage->isChecked())
    {
        QSqlQuery query(db);
        QString sql = R"(
        SELECT 用户编号,密码 FROM `用户`
        WHERE 用户名 = :username
    )";
        query.prepare(sql);
        query.bindValue(":username", username);

        qDebug()<<1;
        if (!query.exec()) {
            QMessageBox::critical(this, "查询失败", "错误：" + query.lastError().text());
            return;
        }

        bool passwordCorrect = false;
        if (query.next()) { // 查询到用户（用户名存在）
            QString storedPassword = query.value("密码").toString(); // 数据库存储的密码
            passwordCorrect = (Password == storedPassword); // 明文比对（实际需加密）
        }
        int userID=query.value("用户编号").toInt();
        QString sql2 = R"(
        SELECT 姓名,管理食堂编号 FROM `管理员`
        WHERE 用户编号 = :username
    )";
        query.prepare(sql2);
        query.bindValue(":username", userID);
        query.exec();
        // 步骤 7：处理验证结果
        if (passwordCorrect) {
            if(query.next())
            {
                QString name=query.value("姓名").toString();
                QMessageBox::information(this, "登录成功", "欢迎管理员，" + name + "！");
            // 跳转到主界面或执行其他登录后操作
                this->close();
                mainwindow=new MainWindow(nullptr,true,userID);
                mainwindow->show();
            }
            else
            {
                QMessageBox::information(this, "登录失败", " 对不起，您不是管理员 ！");
            }
        } else {
            if (query.at() == -1) { // 查询结果为空（用户名不存在）
                QMessageBox::warning(this, "登录失败", "用户名不存在！");
            } else {
                QMessageBox::warning(this, "登录失败", "密码错误！");
            }
        }
    }
    else{

        QSqlQuery query(db);
        QString sql = R"(
        SELECT 用户编号,密码 FROM `用户`
        WHERE 用户名 = :username
    )";
        query.prepare(sql);
        query.bindValue(":username", username);


        if (!query.exec()) {
            QMessageBox::critical(this, "查询失败", "错误：" + query.lastError().text());
            return;
        }
        bool passwordCorrect = false;
        if (query.next()) { // 查询到用户（用户名存在）

            QString storedPassword = query.value("密码").toString(); // 数据库存储的密码
            passwordCorrect = (Password == storedPassword); // 明文比对（实际需加密）
        }
        // 步骤 7：处理验证结果
        if (passwordCorrect) {
            QMessageBox::information(this, "登录成功", "欢迎，" + username + "！");
            // 跳转到主界面或执行其他登录后操作
            int userID=query.value("用户编号").toInt();

            mainwindow=new MainWindow(nullptr,false,userID);
            // mainwindow->ismanager=false;
            mainwindow->show();
            this->close();
        } else {
            if (query.at() == -1) { // 查询结果为空（用户名不存在）
                QMessageBox::warning(this, "登录失败", "用户名不存在！");
            } else {
                QMessageBox::warning(this, "登录失败", "密码错误！");
            }
        }
    }
}


void Log::on_newregister_clicked()
{

    QString username=ui->user->text().trimmed();
    QString Password=ui->password->text().trimmed();
    QSqlDatabase& db = Database::getInstance().getDb();

    QSqlQuery query(db);
    if (username.isEmpty() || Password.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "用户名和密码不能为空！");
        return;
    }

    // 3. 开启事务（确保用户和顾客插入原子性）
    db.transaction();



    query.prepare("SELECT 用户编号 FROM `用户` WHERE 用户名 = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "查询失败：" + query.lastError().text());
        return;
    }

    if (query.next()) {
        QMessageBox::warning(this, "注册失败", "用户名已存在！");
        return;
    }

    // 4. 向 `用户` 表插入新用户

    QString insertUserSql = R"(
        INSERT INTO `用户` (用户名, 密码)
        VALUES (:username, :password)
    )";
    query.prepare(insertUserSql);
    query.bindValue(":username", username);
    query.bindValue(":password", Password); // 实际项目中密码需加密存储（如 MD5/SHA256）

    if (!query.exec()) {
        db.rollback(); // 插入失败，回滚事务
        QMessageBox::critical(this, "注册失败", "用户名已存在或数据库错误：" + query.lastError().text());
        return;
    }

    // 5. 获取自增的 `用户编号`（必须在插入后立即获取）
    query.exec("SELECT LAST_INSERT_ID()"); // MySQL 函数，返回最后插入的自增 ID
    query.next();
    int userId = query.value(0).toInt();


    // 6. 向 `顾客` 表插入新顾客（使用自增的用户编号）
    QString insertCustomerSql = R"(
        INSERT INTO `顾客` (用户编号, 昵称)
        VALUES (:userId, :nickname)
    )";
    query.prepare(insertCustomerSql);
    query.bindValue(":userId", userId);
    query.bindValue(":nickname", username); // 昵称可选，允许 NULL

    if (!query.exec()) {
        db.rollback();
        QMessageBox::critical(this, "注册失败", "顾客信息插入失败：" + query.lastError().text());
        return;
    }

    // 7. 提交事务，注册成功
    db.commit();
    QMessageBox::information(this, "注册成功", "用户注册完成！用户编号：" + QString::number(userId));
}


void Log::on_register_link_clicked()
{
    ui->label->hide();
    ui->log->hide();
    ui->newregister->show();
    ui->register_link->hide();
    ui->manage->hide();
    ui->log_link->show();
}


void Log::on_log_link_clicked()
{
    ui->label->show();
    ui->log->show();
    ui->newregister->hide();
    ui->register_link->show();
    ui->manage->show();
    ui->log_link->hide();
}

