#include "mainwindow.h"
#include "log.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent,bool b,int id)
    : QMainWindow(parent),
    ismanager(b),userId(id), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->searchtext->installEventFilter(this);
    if(!ismanager){
        canteenPage();
        ui->addPort->hide();
        ui->adddish->hide();
        ui->addevaluation->hide();
        ui->deletedish->hide();
        ui->changedish->hide();
    }
    else
    {
        managePage(userId);
        ui->adddish->hide();
        ui->addevaluation->hide();
        ui->deletedish->hide();
        ui->changedish->hide();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->searchtext) {
        if (event->type() == QEvent::FocusIn) {
            ui->searchtext->clear();
            return false;
        }
        if (event->type() == QEvent::FocusOut) {
            if(ui->searchtext->text()=="")
            {
                ui->searchtext->setText("请输入菜名");
            }

            return false;
        }
    }
    // 其他事件交给父类处理
    return QMainWindow::eventFilter(object, event);
}

// void MainWindow::clearcontainer(QFrame *frame)
// {
//     if (!frame) return;

//     // 1. 清理布局中的所有项（关键）
//     QLayout* layout = frame->layout();
//     if (layout) {
//         QLayoutItem* item;
//         while ((item = layout->takeAt(0)) != nullptr) {  // 逐个取出布局项（从索引0开始）
//             // a. 处理部件项（QWidgetItem）
//             if (QWidget* widget = item->widget()) {
//                 delete widget;  // 删除部件（自动从父部件中移除）
//             }
//             // b. 处理子布局项（QLayoutItem）
//             else if (QLayout* childLayout = item->layout()) {
//                 clearcontainer(qobject_cast<QFrame*>(childLayout->parentWidget()));  // 递归清理子布局的容器（假设子布局的父是QFrame）
//                 delete childLayout;  // 删除子布局
//             }
//             // c. 处理间隔器项（QSpacerItem）等其他项
//             delete item;  // 删除布局项本身（释放内存）
//         }
//         // 可选：删除布局（若需要重置容器的布局）
//         delete layout;
//         // frame->setLayout(nullptr);
//     }

//     // 2. 清理容器的直接子部件（未通过布局添加的部件）
//     QList<QWidget*> children = frame->findChildren<QWidget*>();
//     for (QWidget* child : children) {
//         if (child != frame) {  // 避免删除容器自身
//             delete child;  // 直接删除子部件（自动从父部件中移除）
//         }
//     }
// }

// void MainWindow::clearcontainer(QFrame *frame)
// {
//     if (!frame) return;

//     // 1. 清理布局内容（安全删除所有子项）
//     if (QLayout* layout = frame->layout()) {
//         QLayoutItem* item;
//         while ((item = layout->takeAt(0)) != nullptr) {
//             // 处理子控件
//             if (QWidget* widget = item->widget()) {
//                 widget->deleteLater(); // 安全延迟删除
//             }
//             // 处理子布局
//             else if (QLayout* subLayout = item->layout()) {
//                 delete subLayout; // 直接删除子布局
//             }
//             // 处理间隔项
//             else if (QSpacerItem* spacer = item->spacerItem()) {
//                 delete spacer;
//             }
//             delete item; // 必须删除布局项
//         }
//         delete layout;   // 删除主布局
//     }

//     // 2. 清理未通过布局添加的直接子控件
//     const auto children = frame->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
//     for (QWidget* child : children) {
//         child->deleteLater(); // 安全删除
//     }

//     // 3. 确保容器无残留布局
//     frame->setLayout(nullptr);
// }


void MainWindow::clearcontainer(QFrame *frame)
{
    if (!frame) return;

    // 1. 清理布局（仅处理直接布局，不递归）
    if (QLayout* layout = frame->layout()) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (QWidget* widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
        delete layout;
        frame->setLayout(nullptr);
    }

    // 2. 清理未通过布局添加的直接子控件
    const auto children = frame->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget* child : children) {
        child->deleteLater();
    }
}


//主页面（顾客）
void MainWindow::canteenPage()
{
    clearcontainer(ui->dynamic);
    ui->addevaluation->hide();
    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);
    // db.transaction();
    QString search={
        "select 食堂编号,食堂名称,营业时间 from `食堂`"
    };
    query.prepare(search);
    query.exec();

    QFrame* container = ui->dynamic;
    QVBoxLayout* layout = new QVBoxLayout(container);  // 为容器设置布局，不影响centralwidget的其他组件
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        QString name = query.value("食堂名称").toString();    // 食堂名称
        QString time = query.value("营业时间").toString();     // 营业时间
        int id=query.value("食堂编号").toInt();
        // 创建CommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton(container);
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        button->setMaximumHeight(60);  // 可选，限制最大高度
        button->setText(name);                  // 主标题（食堂名称）
        button->setDescription("营业时间：" + time);           // 副标题（营业时间）
        // button->setIcon(QIcon(":/icons/canteen.png"));  // 可选：设置图标（需准备资源文件）
        // button->setStyleSheet("QCommandLinkButton { text-align: left; padding: 12px; }");  // 样式调整

        // 绑定点击事件（示例：点击时弹出食堂名称）
        connect(button, &QCommandLinkButton::clicked, this, [this, id]() {
            // QMessageBox::information(this, "食堂详情", "你点击了: " + name);
            // 这里可以扩展其他逻辑（如跳转到详情页、加载窗口数据等）
            portPage(id);
            this->canteenselect=id;
        });
        qDebug()<<name;
        qDebug()<<time;
        layout->addWidget(button, 0, Qt::AlignTop);  // 将按钮添加到布局
    }
}

//主页面（管理员）
void MainWindow::managePage(int id)
{
    ui->addPort->show();
    ui->adddish->hide();
    clearcontainer(ui->dynamic);
    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT 管理食堂编号
          FROM `管理员`
         WHERE 用户编号 = :userId
    )");
    query.bindValue(":userId", userId);
    if (!query.exec() || !query.next()) {
        // 没查到管理员记录，或者执行失败
        QMessageBox::warning(this, "错误", "无法获取您的食堂信息");
        return;
    }
    int canteenid = query.value("管理食堂编号").toInt();

    // —— 2. 查该食堂下的所有窗口 ——
    query.prepare(R"(
        SELECT 窗口编号, 窗口分类
          FROM `窗口`
         WHERE 所属食堂编号 = :canteenId
    )");
    query.bindValue(":canteenId", canteenid);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "查询窗口列表失败");
        return;
    }

    QFrame* container = ui->dynamic;
    QVBoxLayout* layout = new QVBoxLayout(container);  // 为容器设置布局，不影响centralwidget的其他组件
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        int id = query.value("窗口编号").toInt();    // 食堂名称
        QString category = query.value("窗口分类").toString();     // 营业时间
        // int id_1=query.value("食堂编号").toInt();
        // 创建CommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton(container);
        button->setMaximumHeight(100);
        button->setText("窗口");                  // 主标题（食堂名称）
        button->setDescription("窗口分类：" + category);           // 副标题（营业时间）
        // button->setIcon(QIcon(":/icons/canteen.png"));  // 可选：设置图标（需准备资源文件）
        // button->setStyleSheet("QCommandLinkButton { text-align: left; padding: 12px; }");  // 样式调整

        // 绑定点击事件（示例：点击时弹出食堂名称）
        connect(button, &QCommandLinkButton::clicked, this, [this, id]() {
            // QMessageBox::information(this, "食堂详情", "你点击了: " + name);
            // 这里可以扩展其他逻辑（如跳转到详情页、加载窗口数据等）
            //portPage(canteenid);
            this->dishPage(id);
            this->portselect=id;
            ui->adddish->show();
            ui->addPort->hide();
        });
        // qDebug()<<name;
        // qDebug()<<time;
        layout->addWidget(button);  // 将按钮添加到布局
    }
}



// 添加窗口
void MainWindow::on_addPort_clicked()
{
    clearcontainer(ui->dynamic);
    // QSqlDatabase &db=Database::getInstance().getDb();
    // QSqlQuery query(db);
    // db.transaction();

    QFrame* container = ui->dynamic;
    QVBoxLayout* mainLayout = new QVBoxLayout(container);

    // 窗口编号组
    QHBoxLayout* portNumberLayout = new QHBoxLayout();
    portNumberLayout->setSpacing(2); // 标签和输入框间距2像素
    QLabel *portnumber_label = new QLabel("窗口编号:",container);
    QLineEdit *portnumber = new QLineEdit("请输入窗口编号",container);
    portNumberLayout->addWidget(portnumber_label);
    portNumberLayout->addWidget(portnumber);
    portNumberLayout->setContentsMargins(0,0,0,0);

    // 窗口分类组
    QHBoxLayout* portClassLayout = new QHBoxLayout();
    portClassLayout->setSpacing(2); // 标签和输入框间距2像素
    QLabel *portclass_label = new QLabel("窗口分类:",container);
    QLineEdit *portclass = new QLineEdit("请输入窗口分类",container);
    portClassLayout->addWidget(portclass_label);
    portClassLayout->addWidget(portclass);



    QHBoxLayout* buttonclass = new QHBoxLayout();     // 子水平布局
    buttonclass->setSpacing(2); // 标签和输入框间距2像素
    QPushButton *add = new QPushButton("添加",container);
    QPushButton *back = new QPushButton("返回",container);

    connect(add,&QPushButton::clicked,this,[=](){
        QSqlDatabase &db=Database::getInstance().getDb();
        QSqlQuery query(db);
        db.transaction();
        int id=portnumber->text().toInt();
        QString class_name=portclass->text();

        QString querySql=R"(select 管理食堂编号 from `管理员` where 用户编号 = :canteen_id)";
        query.prepare(querySql);
        query.bindValue(":canteen_id", this->userId);
        query.exec();
        query.next();
        int canteen_id=query.value("管理食堂编号").toInt();

        QString insertUserSql = R"(
        INSERT INTO `窗口` (窗口编号, 窗口分类,所属食堂编号)
        VALUES (:port_id, :class,:canteen_id)
    )";
        query.prepare(insertUserSql);
        query.bindValue(":port_id", id);
        query.bindValue(":class", class_name); // 实际项目中密码需加密存储（如 MD5/SHA256）
        query.bindValue(":canteen_id", canteen_id);
        if (!query.exec()) {
            db.rollback(); // 插入失败，回滚事务
            QMessageBox::critical(this, "注册失败", "用户名已存在或数据库错误：" + query.lastError().text());
            return;
        }
        else
        {
            db.commit();
            QMessageBox::information(this, "创建成功", "新窗口已添加");
        }

    });

    connect(back,&QPushButton::clicked,this,[=](){
        this->managePage(this->userId);
    });
    buttonclass->addWidget(add);
    buttonclass->addWidget(back);

    // 将子布局添加到主布局
    mainLayout->addLayout(portNumberLayout);
    mainLayout->addLayout(portClassLayout);
    mainLayout->addLayout(buttonclass);
    mainLayout->setSpacing(5); // 组与组之间的间距10像素
    mainLayout->setContentsMargins(10, 10, 10, 10); // 容器边距
}



//窗口页面
void MainWindow::portPage(int id)
{
    if(this->ismanager)
    {
        ui->addPort->show();
        ui->adddish->hide();
    }
    clearcontainer(ui->dynamic);
    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);
    // db.transaction();
    QString search={
        "SELECT 窗口编号, 窗口分类 FROM `窗口` WHERE 所属食堂编号 = :canteenId"
    };

    query.prepare(search);
    query.bindValue(":canteenId", id);  // 类型自动适配
    query.exec();

    QFrame* container = ui->dynamic;
    QVBoxLayout* layout = new QVBoxLayout(container);  // 为容器设置布局，不影响centralwidget的其他组件
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        QString name = query.value("窗口编号").toString();    // 食堂名称
        QString category = query.value("窗口分类").toString();     // 营业时间
        int id_now=query.value("窗口编号").toInt();

        // 创建CommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton();
        button->setMaximumHeight(100);
        button->setText("窗口"+name);                  // 主标题（食堂名称）
        button->setDescription("窗口分类：" + category);           // 副标题（营业时间）
        // button->setIcon(QIcon(":/icons/canteen.png"));  // 可选：设置图标（需准备资源文件）
        // button->setStyleSheet("QCommandLinkButton { text-align: left; padding: 12px; }");  // 样式调整

        // 绑定点击事件（示例：点击时弹出食堂名称）
        connect(button, &QCommandLinkButton::clicked, this, [this, id_now]() {
            // QMessageBox::information(this, "食堂详情", "你点击了: " + name);
            // 这里可以扩展其他逻辑（如跳转到详情页、加载窗口数据等）
            // portPage(id);
            this->dishPage(id_now);
            this->portselect=id_now;
        });
        layout->addWidget(button);  // 将按钮添加到布局
    }
}


void MainWindow::dishPage(int id)
{
    qDebug()<<id;
    if(this->ismanager)
    {
        ui->addPort->hide();
        ui->adddish->show();
    }
    clearcontainer(ui->dynamic);
    QSqlDatabase &db=Database::getInstance().getDb();
    QSqlQuery query(db);
    QString search={
        "select 菜品编号,菜品名称,价格,标签 from `菜品` where 窗口编号=:portid"
    };
    query.prepare(search);
    query.bindValue(":portid",id);
    if (!query.exec()) {
        qDebug() << "查询执行失败:" << query.lastError().text();
        return;
    }
    QFrame* container = ui->dynamic;
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    bool hasData = false;
    while (query.next()) {
        // qDebug()<<100000000000;
        hasData = true;
        QString name = query.value("菜品名称").toString();
        float price = query.value("价格").toFloat();
        QString label = query.value("标签").toString();
        int dish_id=query.value("菜品编号").toInt();
        // 创建CommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton(container);
        button->setMaximumHeight(100);
        button->setText(name);                  // 主标题（食堂名称）
        button->setDescription("标签：" + label+ "  "+ "价格："+QString::number(price));           // 副标题（营业时间）
        // button->setIcon(QIcon(":/icons/canteen.png"));  // 可选：设置图标（需准备资源文件）
        // button->setStyleSheet("QCommandLinkButton { text-align: left; padding: 12px; }");  // 样式调整

        connect(button, &QCommandLinkButton::clicked, this, [this, dish_id]() {
            this->evaluationPage(dish_id);
            this->dishselect=dish_id;
        });
        layout->addWidget(button);  // 将按钮添加到布局
    }

}

void MainWindow::evaluationPage(int id)
{
    if(!this->ismanager)
    {
        ui->addevaluation->show();
    }
    else
    {
        ui->changedish->show();
        ui->deletedish->show();
    }
    ui->adddish->hide();
    clearcontainer(ui->dynamic);
    QSqlDatabase &db=Database::getInstance().getDb();
    QSqlQuery query(db);
    QString search = R"(
    SELECT 评分, 评价, 评价时间, 顾客.昵称, 顾客.用户编号
    FROM 评价
    INNER JOIN 顾客 ON 评价.用户编号 = 顾客.用户编号
    WHERE 评价.菜品编号 = :dishid
    ORDER BY 评价时间 DESC
    )";

    query.prepare(search);
    query.bindValue(":dishid",id);
    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError().text();
        return;
    }

    QFrame* container = ui->dynamic;
    QVBoxLayout* layout = new QVBoxLayout(container);  // 为容器设置布局，不影响centralwidget的其他组件
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);
    bool hasData = false;
    QListWidget *listWidget = new QListWidget(container);

    while (query.next()) {
        hasData = true;
        int score = query.value("评分").toInt();
        QString evaluation = query.value("评价").toString();
        QString username = query.value("昵称").toString();
        int uer_id=query.value("用户编号").toInt();
        QString time = query.value("评价时间").toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString displayText = QString("用户：%1\n评分：%2\n评价：%3\n时间：%4")
                                  .arg(username)
                                  .arg(score)
                                  .arg(evaluation)
                                  .arg(time);

        listWidget->addItem(displayText);
    }
    if(hasData)
    layout->addWidget(listWidget);
    else
    {
        QLabel *text=new QLabel("暂无评价");
        layout->addWidget(text);
    }
}



//搜索菜品
void MainWindow::on_searchbutton_clicked()
{
    if(ui->searchtext->text()=="请输入菜名")
    {
        QMessageBox::information(this, "搜索失败", "搜索内容不能为空");
        return;
    }
    QString dish=ui->searchtext->text();
    QSqlDatabase &db=Database::getInstance().getDb();
    QSqlQuery query(db);
    clearcontainer(ui->dynamic);
    QString search = R"(
    SELECT
        菜品.菜品编号,
        菜品.菜品名称,
        菜品.价格,
        菜品.标签,
        窗口.窗口编号,
        食堂.食堂名称
    FROM 菜品
    INNER JOIN 窗口 ON 菜品.窗口编号 = 窗口.窗口编号
    INNER JOIN 食堂 ON 窗口.所属食堂编号 = 食堂.食堂编号
    WHERE 菜品.菜品名称 LIKE :dish
)";


    query.prepare(search);
    query.bindValue(":dish", "%" + dish + "%");

    if (!query.exec()) {
        qDebug() << "查询失败:" << query.lastError().text();
        return;
    }
    QFrame* container = ui->dynamic;
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    bool hasData = false;
    while (query.next()) {
        // qDebug()<<100000000000;
        hasData = true;
        QString name = query.value("菜品名称").toString();
        float price = query.value("价格").toFloat();
        QString label = query.value("标签").toString();
        int dish_id=query.value("菜品编号").toInt();
        qDebug()<<dish_id;
        int windowId = query.value("窗口编号").toInt();
        QString canteenName = query.value("食堂名称").toString();
        // 创建CommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton(container);
        button->setMaximumHeight(100);
        button->setText(name);                  // 主标题（食堂名称）
        button->setDescription("所在食堂:"+canteenName+"  "+"所在窗口："+QString::number(windowId)+"  "+"标签：" + label+ "  "+ "价格："+QString::number(price));           // 副标题（营业时间）
        // button->setIcon(QIcon(":/icons/canteen.png"));  // 可选：设置图标（需准备资源文件）
        // button->setStyleSheet("QCommandLinkButton { text-align: left; padding: 12px; }");  // 样式调整

        connect(button, &QCommandLinkButton::clicked, this, [this, dish_id]() {
            this->evaluationPage(dish_id);
            this->dishselect=dish_id;
        });
        layout->addWidget(button);  // 将按钮添加到布局
    }
    if(!hasData)
    {
        QLabel *text=new QLabel("暂无菜品");
        layout->addWidget(text);
    }
}



void MainWindow::on_adddish_clicked()
{
    clearcontainer(ui->dynamic);
    QFrame* container = ui->dynamic;
    QVBoxLayout* mainLayout = new QVBoxLayout(container);

    // 窗口编号组
    QHBoxLayout* dishNameLayout = new QHBoxLayout();
    dishNameLayout->setSpacing(2); // 标签和输入框间距2像素
    QLabel *dishname_label = new QLabel("菜品名称:",container);
    QLineEdit *dish_name = new QLineEdit("请输入菜品名称",container);
    dishNameLayout->addWidget(dishname_label);
    dishNameLayout->addWidget(dish_name);
    dishNameLayout->setContentsMargins(0,0,0,0);

    // 窗口分类组
    QHBoxLayout* dishPriceLayout = new QHBoxLayout();
    dishPriceLayout->setSpacing(2); // 标签和输入框间距2像素
    QLabel *price_label = new QLabel("菜品价格:",container);
    QLineEdit *price = new QLineEdit("请输入菜品价格",container);
    dishPriceLayout->addWidget(price_label);
    dishPriceLayout->addWidget(price);

    //菜品标签
    QHBoxLayout* dishlabelLayout = new QHBoxLayout();
    dishlabelLayout->setSpacing(2); // 标签和输入框间距2像素
    QLabel *dish_label = new QLabel("菜品标签:",container);
    QLineEdit *dish = new QLineEdit("请输入菜品标签",container);
    dishlabelLayout->addWidget(dish_label);
    dishlabelLayout->addWidget(dish);


    QHBoxLayout* buttonclass = new QHBoxLayout();     // 子水平布局
    buttonclass->setSpacing(2); // 标签和输入框间距2像素
    QPushButton *add = new QPushButton("添加",container);
    QPushButton *back = new QPushButton("返回",container);

    connect(add,&QPushButton::clicked,this,[=](){
        QSqlDatabase &db=Database::getInstance().getDb();
        QSqlQuery query(db);
        query.exec("SET NAMES 'utf8mb4'");
        db.transaction();
        QString name=dish_name->text();
        float price_dish=price->text().toFloat();
        QString label=dish->text();

        QString checkSql = R"(
        SELECT COUNT(*) FROM 菜品
        WHERE 窗口编号 = :port_id
          AND 菜品名称 = :name
    )";
        query.prepare(checkSql);
        query.bindValue(":port_id", this->portselect);
        query.bindValue(":name", name);

        if (!query.exec()) {  // 检查查询执行失败处理
            db.rollback();
            QMessageBox::critical(this, "数据库错误", "重复检查查询失败：" + query.lastError().text());
            return;
        }

        if (query.next() && query.value(0).toInt() > 0) {  // 存在重复记录
            db.rollback();
            QMessageBox::warning(this, "重复菜品", "当前窗口已存在同名菜品，请修改名称后重试");
            return;
        }



        QString insertUserSql = R"(
        INSERT INTO `菜品` (窗口编号, 菜品名称,价格,标签)
        VALUES (:port_id, :name,:price,:label)
    )";
        query.prepare(insertUserSql);
        query.bindValue(":port_id", this->portselect);
        query.bindValue(":name", name); // 实际项目中密码需加密存储（如 MD5/SHA256）
        query.bindValue(":price", price_dish);
        query.bindValue(":label", label);
        if (!query.exec()) {
            db.rollback(); // 插入失败，回滚事务
            QMessageBox::critical(this, "添加失败", "菜品价格不能为0");
            return;
        }
        else
        {
            db.commit();
            QMessageBox::information(this, "创建成功", "新菜品已添加");
            return;
        }

    });

    connect(back,&QPushButton::clicked,this,[=](){
        this->dishPage(this->portselect);

    });
    buttonclass->addWidget(add);
    buttonclass->addWidget(back);

    // 将子布局添加到主布局
    mainLayout->addLayout(dishNameLayout);
    mainLayout->addLayout(dishPriceLayout);
    mainLayout->addLayout(dishlabelLayout);
    mainLayout->addLayout(buttonclass);
    mainLayout->setSpacing(5); // 组与组之间的间距10像素
    mainLayout->setContentsMargins(10, 10, 10, 10); // 容器边距
}






void MainWindow::on_addevaluation_clicked()
{

    clearcontainer(ui->dynamic);

    QFrame* container = ui->dynamic;
    QVBoxLayout* mainLayout = new QVBoxLayout(container);

    // 窗口编号组
    QHBoxLayout* scoreLayout = new QHBoxLayout();
    scoreLayout->setSpacing(2); // 标签和输入框间距2像素
    QLabel *score_label = new QLabel("请选择评分:",container);
    QComboBox *scoreBox = new QComboBox(this);
    for (int i = 0; i <= 5; ++i) {
        scoreBox->addItem(QString::number(i));
    }
    scoreLayout->addWidget(score_label);
    scoreLayout->addWidget(scoreBox);
    scoreLayout->setContentsMargins(0,0,0,0);

    // 窗口分类组
    QHBoxLayout* evaluationLayout = new QHBoxLayout();
    evaluationLayout->setSpacing(2); // 标签和输入框间距2像素
    QLabel *evaluation_label = new QLabel("评价:",container);
    QTextEdit *evaluation = new QTextEdit(container);
    evaluation->setPlaceholderText("请输入评价内容");
    evaluationLayout->addWidget(evaluation_label);
    evaluationLayout->addWidget(evaluation);

    QHBoxLayout* buttonclass = new QHBoxLayout();     // 子水平布局
    buttonclass->setSpacing(2); // 标签和输入框间距2像素
    QPushButton *add = new QPushButton("添加",container);
    QPushButton *back = new QPushButton("返回",container);


    connect(add, &QPushButton::clicked, this, [=]() {
        QSqlDatabase &db = Database::getInstance().getDb();
        QSqlQuery query(db);

        // 开启事务
        if (!db.transaction()) {
            QMessageBox::critical(this, "错误", "启动事务失败：" + db.lastError().text());
            return;
        }

        // 假设你有对应的控件或者变量获取这些值
        int userId = this->userId;  // 当前用户编号
        int dishId = this->dishselect;  // 当前菜品编号
        int score = scoreBox->currentText().toInt();// 评分，假设是 QSpinBox
        QString evaluationText = evaluation->toPlainText().trimmed();

        // 简单校验评价不能为空
        // if (evaluationText.isEmpty()) {
        //     QMessageBox::warning(this, "输入错误", "评价内容不能为空");
        //     return;
        // }
        // // 简单校验评分范围0~5
        // if (score < 0 || score > 5) {
        //     QMessageBox::warning(this, "输入错误", "评分必须在0到5之间");
        //     return;
        // }

        QString insertSql = R"(
        INSERT INTO 评价 (用户编号, 菜品编号, 评分, 评价)
        VALUES (:user_id, :dish_id, :score, :evaluation)
    )";

        query.prepare(insertSql);
        query.bindValue(":user_id", userId);
        query.bindValue(":dish_id", dishId);
        query.bindValue(":score", score);
        query.bindValue(":evaluation", evaluationText);

        if (!query.exec()) {
            db.rollback();
            QMessageBox::critical(this, "数据库错误", "添加评价失败：" + query.lastError().text());
            return;
        }

        // 计算该菜品的平均评分
        QString avgScoreSql = R"(
            SELECT AVG(评分) FROM 评价 WHERE 菜品编号 = :dish_id
         )";

        query.prepare(avgScoreSql);
        query.bindValue(":dish_id", dishId);

        if (!query.exec()) {
            db.rollback();
            QMessageBox::critical(this, "数据库错误", "计算平均评分失败：" + query.lastError().text());
            return;
        }

        // 获取平均评分
        float avgScore = 0.0;
        if (query.next()) {
            avgScore = query.value(0).toFloat();
        }

        // 更新菜品评分
        QString updateDishSql = R"(
            UPDATE 菜品 SET 菜品评分 = :avg_score WHERE 菜品编号 = :dish_id
        )";

        query.prepare(updateDishSql);
        query.bindValue(":avg_score", avgScore);
        query.bindValue(":dish_id", dishId);

        if (!query.exec()) {
            db.rollback();
            QMessageBox::critical(this, "数据库错误", "更新菜品评分失败：" + query.lastError().text());
            return;
        }

        // 提交事务
        if (!db.commit()) {
            QMessageBox::critical(this, "数据库错误", "提交事务失败：" + db.lastError().text());
            return;
        }

        QMessageBox::information(this, "成功", "评价已成功添加，菜品评分已更新");
    });


    connect(back,&QPushButton::clicked,this,[=](){
        this->dishPage(this->portselect);
        ui->addevaluation->hide();

    });
    buttonclass->addWidget(add);
    buttonclass->addWidget(back);

    // 将子布局添加到主布局
    mainLayout->addLayout(scoreLayout);
    mainLayout->addLayout(evaluationLayout);
    mainLayout->addLayout(buttonclass);
    mainLayout->setSpacing(5); // 组与组之间的间距10像素
    mainLayout->setContentsMargins(10, 10, 10, 10); // 容器边距
}



void MainWindow::on_profile_clicked()
{
    clearcontainer(ui->dynamic);
    // 获取用户信息（假设已经有用户的ID和昵称）
    int userId = this->userId;  // 获取当前用户的ID
    QSqlDatabase &db = Database::getInstance().getDb();
    QSqlQuery query(db);
    QString search;
    if(this->ismanager)
    {
        search=R"(select 姓名 from 管理员 where 用户编号=:user_id)";
    }
    else
    {
        search=R"(select 昵称 from 顾客 where 用户编号=:user_id)";
    }
    query.prepare(search);
    query.bindValue(":user_id",userId);
    query.exec();
    query.next();
    QString nickname;
    if(this->ismanager)
    {
        nickname = query.value("姓名").toString();  // 获取当前用户的昵称
    }
    else
    {
        nickname = query.value("昵称").toString();
    }

    // qDebug()<<userId;
    // 创建容器和布局
    QFrame* container = ui->dynamic;
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    // 显示用户编号和昵称
    QLabel* userIdLabel = new QLabel(QString("用户编号: %1").arg(userId), container);
    QLabel* nicknameLabel = new QLabel(QString("昵称: %1").arg(nickname), container);
    layout->addWidget(userIdLabel);
    layout->addWidget(nicknameLabel);

    // 修改昵称按钮
    QPushButton* modifyNicknameButton = new QPushButton("修改昵称", container);
    layout->addWidget(modifyNicknameButton);

    // 删除账号按钮
    QPushButton* deleteAccountButton = new QPushButton("删除账号", container);
    layout->addWidget(deleteAccountButton);

    // 退出登录按钮
    QPushButton* logoutButton = new QPushButton("退出登录", container);
    layout->addWidget(logoutButton);

    // 修改昵称按钮点击事件
    connect(modifyNicknameButton, &QPushButton::clicked, this, [=]() {
        bool ok;
        QString newNickname = QInputDialog::getText(this, "修改昵称", "请输入新的昵称:", QLineEdit::Normal, nickname, &ok);
        if (ok && !newNickname.isEmpty()) {
            QSqlDatabase &db = Database::getInstance().getDb();
            QSqlQuery query(db);
            query.prepare("UPDATE 顾客 SET 昵称 = :nickname WHERE 用户编号 = :userId");
            query.bindValue(":nickname", newNickname);
            query.bindValue(":userId", userId);
            if (query.exec()) {
                nicknameLabel->setText(QString("昵称: %1").arg(newNickname));  // 更新界面上的昵称
                QMessageBox::information(this, "修改成功", "昵称修改成功！");
            } else {
                QMessageBox::critical(this, "修改失败", "修改昵称失败：" + query.lastError().text());
            }
        }
    });

    // 删除账号按钮点击事件
    connect(deleteAccountButton, &QPushButton::clicked, this, [=]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "确认删除", "确定删除该账号吗？此操作不可恢复。",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QSqlDatabase &db = Database::getInstance().getDb();
            QSqlQuery query(db);

            // 开启事务
            if (!db.transaction()) {
                QMessageBox::critical(this, "删除失败", "启动事务失败：" + db.lastError().text());
                return;
            }

            // 删除该用户的评论
            query.prepare("DELETE FROM 评价 WHERE 用户编号 = :userId");
            query.bindValue(":userId", userId);
            if (!query.exec()) {
                db.rollback();  // 如果删除评论失败，回滚事务
                QMessageBox::critical(this, "删除失败", "删除评论失败：" + query.lastError().text());
                return;
            }

            // 删除用户
            query.prepare("DELETE FROM 顾客 WHERE 用户编号 = :userId");
            query.bindValue(":userId", userId);
            if (!query.exec()) {
                db.rollback();  // 如果删除用户失败，回滚事务
                QMessageBox::critical(this, "删除失败", "删除账号失败：" + query.lastError().text());
                return;
            }

            query.prepare("DELETE FROM 用户 WHERE 用户编号 = :userId");
            query.bindValue(":userId", userId);
            if (!query.exec()) {
                db.rollback();  // 如果删除用户数据失败，回滚事务
                QMessageBox::critical(this, "删除失败", "删除用户数据失败：" + query.lastError().text());
                return;
            }

            // 提交事务
            if (!db.commit()) {
                QMessageBox::critical(this, "删除失败", "提交事务失败：" + db.lastError().text());
                return;
            }

            // 删除成功后的处理
            QMessageBox::information(this, "删除成功", "账号及评论删除成功！");
            // 执行注销操作，跳转回登录界面等
            log=new Log();
            log->show();
            this->close();            // 退出登录函数
            delete this;
        }
    });


    // 退出登录按钮点击事件
    connect(logoutButton, &QPushButton::clicked, this, [=]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "确认退出", "确定要退出登录吗？",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            log=new Log();
            log->show();
            this->close();            // 退出登录函数
            delete this;
        }
    });
}


void MainWindow::on_changedish_clicked()
{
    QSqlDatabase &db = Database::getInstance().getDb();
    QSqlQuery query(db);
    QString search=R"(select 价格 from 菜品 where 菜品编号=:dish_id;)";
    query.prepare(search);
    query.bindValue(":dish_id",this->dishselect);
    query.exec();
    query.next();
    float now=query.value("价格").toFloat();
    qDebug()<<this->dishselect;
    bool ok;
    float price = QInputDialog::getText(this, "修改价格", "请输入新的价格:", QLineEdit::Normal, QString::number(now), &ok).toFloat();
    if (ok) {

        query.prepare("UPDATE 菜品 SET 价格 = :price WHERE 菜品编号 = :dishId");
        query.bindValue(":price", price);
        query.bindValue(":dishId", this->dishselect);
        if (query.exec()) {
            QMessageBox::information(this, "修改成功", "价格修改成功！");
        } else {
            QMessageBox::critical(this, "修改失败", "修改价格失败：" + query.lastError().text());
        }
    }
}


void MainWindow::on_homepage_clicked()
{
    if(this->ismanager)
    {
        this->managePage(this->userId);

    }
    else
    {
        this->canteenPage();
    }
}

