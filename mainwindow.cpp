#include "mainwindow.h"
#include "log.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent,bool b,int id)
    : QMainWindow(parent),
    ismanager(b),userId(id), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dynamic->hide();
    ui->information->setText("主页");
    ui->searchtext->installEventFilter(this);
    this->recommend();
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


void MainWindow::clearcontainer(QScrollArea *scrollArea)
{
    if (!scrollArea) return;

    // 1. 清理 QScrollArea 中的 widget（内容区域）
    QWidget* contentWidget = scrollArea->widget();
    if (contentWidget) {
        // 删除内容区域的子控件
        const auto children = contentWidget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
        for (QWidget* child : children) {
            child->deleteLater();
        }

        // 删除布局
        QLayout* layout = contentWidget->layout();
        if (layout) {
            QLayoutItem* item;
            while ((item = layout->takeAt(0)) != nullptr) {
                if (QWidget* widget = item->widget()) {
                    widget->deleteLater();
                }
                delete item;
            }
            delete layout;
        }

        // 删除 contentWidget（也即是 QScrollArea 中的 widget）
        scrollArea->setWidget(nullptr);
        contentWidget->deleteLater();
    }
}

//主页面（顾客）
void MainWindow::canteenPage()
{
    clearcontainer(ui->dynamic1);
    ui->addevaluation->hide();
    ui->information->setText("主页");
    // 数据库查询操作
    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);
    QString search = {
        "select 食堂编号,食堂名称,营业时间 from `食堂`"
    };
    query.prepare(search);
    query.exec();

    // 创建一个 QWidget 作为 QScrollArea 的容器
    QWidget* scrollWidget = new QWidget();  // 创建 QWidget 作为 scrollArea 的容器
    QVBoxLayout* layout = new QVBoxLayout(scrollWidget);  // 为该 QWidget 设置布局
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);  // 设置控件之间的间距

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        QString name = query.value("食堂名称").toString();    // 食堂名称
        QString time = query.value("营业时间").toString();   // 营业时间
        int id = query.value("食堂编号").toInt();

        // 创建 CommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton(scrollWidget);  // 使用 QWidget 作为父级
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        button->setMaximumHeight(60);  // 限制按钮的最大高度
        button->setText(name);         // 主标题（食堂名称）
        button->setDescription("营业时间：" + time);  // 副标题（营业时间）

        // 绑定点击事件（点击按钮时，跳转到食堂详情页面）
        connect(button, &QCommandLinkButton::clicked, this, [this, id]() {
            portPage(id);  // 跳转到食堂详情页面
            this->canteenselect = id;  // 设置选中的食堂编号
        });

        layout->addWidget(button);  // 将按钮添加到布局
    }

    QScrollArea* scrollArea = ui->dynamic1;
    scrollArea->setWidget(scrollWidget);

    // 禁用自动调整，确保 QScrollArea 根据内容进行滚动
    scrollArea->setWidgetResizable(true);
    layout->addStretch();
    // 如果没有数据，显示提示
    if (!hasData) {
        QLabel* noDataLabel = new QLabel("没有食堂数据", scrollWidget);
        layout->addWidget(noDataLabel);  // 将提示信息添加到布局
    }
}


//主页面（管理员）
void MainWindow::managePage(int id)
{
    ui->addPort->show();
    ui->adddish->hide();
    ui->changedish->hide();
    ui->deletedish->hide();
    clearcontainer(ui->dynamic1);  // 清空界面内容
    ui->information->setText("主页");
    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);

    // 获取当前用户管理的食堂编号
    query.prepare(R"(
        SELECT 管理食堂编号
          FROM `管理员`
         WHERE 用户编号 = :userId
    )");
    query.bindValue(":userId", userId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "无法获取您的食堂信息");
        return;
    }
    int canteenid = query.value("管理食堂编号").toInt();

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

    // 创建 QScrollArea 的容器
    QWidget* scrollWidget = new QWidget();  // 使用 QWidget 作为容器
    QVBoxLayout* layout = new QVBoxLayout(scrollWidget);  // 为 QWidget 设置布局
    layout->setContentsMargins(10, 10, 10, 10);  // 设置外边距
    layout->setSpacing(8);  // 设置控件间距

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        int windowId = query.value("窗口编号").toInt();  // 窗口编号
        QString category = query.value("窗口分类").toString();  // 窗口分类

        QCommandLinkButton* button = new QCommandLinkButton(scrollWidget);
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        button->setMaximumHeight(100);
        button->setText("窗口:"+QString::number(windowId));
        button->setDescription("窗口分类：" + category);


        connect(button, &QCommandLinkButton::clicked, this, [this, windowId]() {
            this->dishPage(windowId);
            this->portselect = windowId;
            ui->adddish->show();
            ui->addPort->hide();
        });

        layout->addWidget(button);
    }


    if (!hasData) {
        QLabel* noDataLabel = new QLabel("没有窗口数据", scrollWidget);
        layout->addWidget(noDataLabel);
    }


    QScrollArea* scrollArea = ui->dynamic1;
    scrollArea->setWidget(scrollWidget);

    scrollArea->setWidgetResizable(true);
    layout->addStretch();
}




// 添加窗口
void MainWindow::on_addPort_clicked()
{
    clearcontainer(ui->dynamic1);

    QWidget* container = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(container);  // 设置主布局

    // 创建窗口编号输入框
    QHBoxLayout* portNumberLayout = new QHBoxLayout();
    portNumberLayout->setSpacing(10);
    QLabel* portnumber_label = new QLabel("窗口编号:", container);
    QLineEdit* portnumber = new QLineEdit("请输入窗口编号", container);
    portNumberLayout->addWidget(portnumber_label);
    portNumberLayout->addWidget(portnumber);
    portNumberLayout->setContentsMargins(0, 0, 0, 0);

    // 创建窗口分类输入框
    QHBoxLayout* portClassLayout = new QHBoxLayout();
    portClassLayout->setSpacing(10);  // 标签和输入框的间距
    QLabel* portclass_label = new QLabel("窗口分类:", container);
    QLineEdit* portclass = new QLineEdit("请输入窗口分类", container);
    portClassLayout->addWidget(portclass_label);
    portClassLayout->addWidget(portclass);

    // 创建按钮布局
    QHBoxLayout* buttonClassLayout = new QHBoxLayout();
    buttonClassLayout->setSpacing(10);  // 按钮之间的间距
    QPushButton* add = new QPushButton("添加", container);
    QPushButton* back = new QPushButton("返回", container);

    // 连接添加按钮的点击事件
    connect(add, &QPushButton::clicked, this, [=]() {
        QSqlDatabase& db = Database::getInstance().getDb();
        QSqlQuery query(db);
        db.transaction();  // 开启事务

        int id = portnumber->text().toInt();
        QString class_name = portclass->text();

        // 查询管理的食堂编号
        QString querySql = R"(
            select 管理食堂编号
            from `管理员`
            where 用户编号 = :canteen_id
        )";
        query.prepare(querySql);
        query.bindValue(":canteen_id", this->userId);
        query.exec();
        query.next();
        int canteen_id = query.value("管理食堂编号").toInt();

        // 插入窗口数据到数据库
        QString insertUserSql = R"(
            INSERT INTO `窗口` (窗口编号, 窗口分类, 所属食堂编号)
            VALUES (:port_id, :class, :canteen_id)
        )";
        query.prepare(insertUserSql);
        query.bindValue(":port_id", id);
        query.bindValue(":class", class_name);  // 设置窗口分类
        query.bindValue(":canteen_id", canteen_id);  // 设置食堂编号

        if (!query.exec()) {
            db.rollback();  // 如果执行失败，回滚事务
            QMessageBox::critical(this, "注册失败", "用户名已存在或数据库错误：" + query.lastError().text());
            return;
        } else {
            db.commit();  // 成功插入后提交事务
            QMessageBox::information(this, "创建成功", "新窗口已添加");
        }
    });

    // 连接返回按钮的点击事件
    connect(back, &QPushButton::clicked, this, [=]() {
        this->managePage(this->userId);  // 返回到管理员页面
    });

    // 将按钮添加到布局
    buttonClassLayout->addWidget(add);
    buttonClassLayout->addWidget(back);

    // 将所有控件添加到主布局
    mainLayout->addLayout(portNumberLayout);
    mainLayout->addLayout(portClassLayout);
    mainLayout->addLayout(buttonClassLayout);
    mainLayout->setSpacing(15);  // 控件之间的间距
    mainLayout->setContentsMargins(10, 10, 10, 10);  // 容器的边距


    QScrollArea* scrollArea = ui->dynamic1;
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);
}




//窗口页面
void MainWindow::portPage(int id)
{

    if (this->ismanager)
    {
        ui->addPort->show();
        ui->adddish->hide();
    }
    clearcontainer(ui->dynamic1);

    // 使用 QScrollArea 包裹内容
    QScrollArea* scrollArea = ui->dynamic1;
    scrollArea->setWidgetResizable(true);  // 设置滚动区域的大小自适应
    QWidget* container = new QWidget();
    scrollArea->setWidget(container);

    QVBoxLayout* layout = new QVBoxLayout(container);  // 为容器设置布局
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);

    QString info = R"(
        SELECT 食堂名称
        FROM `食堂`
        WHERE 食堂编号 = :canteenId
    )";

    query.prepare(info);
    query.bindValue(":canteenId", id);
    query.exec();
    query.next();
    QString canteen=query.value("食堂名称").toString();
    ui->information->setText(canteen+":");

    QString search = R"(
        SELECT 窗口编号, 窗口分类
        FROM `窗口`
        WHERE 所属食堂编号 = :canteenId
    )";

    query.prepare(search);
    query.bindValue(":canteenId", id);
    query.exec();

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        QString name = query.value("窗口编号").toString();  // 窗口编号
        QString category = query.value("窗口分类").toString();  // 窗口分类
        int id_now = query.value("窗口编号").toInt();

        // 创建 QCommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton(container);
        button->setMaximumHeight(100);
        button->setText("窗口:" + name);  // 主标题
        button->setDescription("窗口分类：" + category);  // 副标题

        // 绑定点击事件
        connect(button, &QCommandLinkButton::clicked, this, [this, id_now]() {
            this->dishPage(id_now);
            this->portselect = id_now;
        });
        layout->addWidget(button);  // 将按钮添加到布局
    }

    // 如果没有数据，可以添加一个提示
    if (!hasData) {
        QLabel* noDataLabel = new QLabel("暂无窗口数据", container);
        layout->addWidget(noDataLabel);
    }
    layout->addStretch();
}



void MainWindow::dishPage(int id)
{

    if (this->ismanager)
    {
        ui->addPort->hide();
        ui->adddish->show();
        ui->changedish->hide();
        ui->deletedish->hide();
    }
    ui->information->setText("窗口:"+QString::number(id));
    clearcontainer(ui->dynamic1);

    // 使用 QScrollArea 包裹内容
    QScrollArea* scrollArea = ui->dynamic1;
    scrollArea->setWidgetResizable(true);
    QWidget* container = new QWidget();
    scrollArea->setWidget(container);

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);
    QString search = R"(
        SELECT 菜品编号, 菜品名称, 价格, 标签,菜品评分
        FROM `菜品`
        WHERE 窗口编号 = :portid
    )";

    query.prepare(search);
    query.bindValue(":portid", id);
    if (!query.exec()) {
        qDebug() << "查询执行失败:" << query.lastError().text();
        return;
    }

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        QString name = query.value("菜品名称").toString();
        float price = query.value("价格").toFloat();
        float score = query.value("菜品评分").toFloat();
        QString label = query.value("标签").toString();
        int dish_id = query.value("菜品编号").toInt();

        // 创建 QCommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton(container);
        button->setMaximumHeight(100);
        button->setText(name);  // 主标题
        button->setDescription("标签：" + label + "  " + "价格：" + QString::number(price)+ "  " +"评分"+QString::number(score));  // 副标题

        connect(button, &QCommandLinkButton::clicked, this, [this, dish_id]() {
            this->evaluationPage(dish_id);
            this->dishselect = dish_id;
        });
        layout->addWidget(button);
    }

    if (!hasData) {
        QLabel* noDataLabel = new QLabel("暂无菜品数据", container);
        layout->addWidget(noDataLabel);
    }
    layout->addStretch();
}


void MainWindow::evaluationPage(int id)
{
    ui->information->setText("当前评价:");
    if (!this->ismanager)
    {
        ui->addevaluation->show();
    }
    else
    {
        ui->changedish->show();
        ui->deletedish->show();
        ui->addPort->hide();
        ui->adddish->hide();

    }
    ui->adddish->hide();
    clearcontainer(ui->dynamic1);

    // 使用 QScrollArea 包裹内容
    QScrollArea* scrollArea = ui->dynamic1;
    scrollArea->setWidgetResizable(true);
    QWidget* container = new QWidget();
    scrollArea->setWidget(container);

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);
    QString search = R"(
        SELECT 评分, 评价, 评价时间, 顾客.昵称, 顾客.用户编号
        FROM 评价
        INNER JOIN 顾客 ON 评价.用户编号 = 顾客.用户编号
        WHERE 评价.菜品编号 = :dishid
        ORDER BY 评价时间 DESC
    )";

    query.prepare(search);
    query.bindValue(":dishid", id);
    if (!query.exec()) {
        qDebug() << "查询失败：" << query.lastError().text();
        return;
    }

    bool hasData = false;
    QListWidget* listWidget = new QListWidget(container);
    listWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // 设置大小策略
    listWidget->setSpacing(5);  // 设置条目之间的间距，单位为像素

    while (query.next()) {
        hasData = true;
        int score = query.value("评分").toInt();
        QString evaluation = query.value("评价").toString();
        QString username = query.value("昵称").toString();
        int user_id = query.value("用户编号").toInt();
        QString time = query.value("评价时间").toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString displayText = QString("用户：%1\n评分：%2\n评价：%3\n时间：%4")
                                  .arg(username)
                                  .arg(score)
                                  .arg(evaluation)
                                  .arg(time);

        listWidget->addItem(displayText);
    }

    if (hasData) {
        layout->addWidget(listWidget);
    } else {
        QLabel* noDataLabel = new QLabel("暂无评价", container);
        layout->addWidget(noDataLabel);
    }

}






//搜索菜品
void MainWindow::on_searchbutton_clicked()
{
    if (ui->searchtext->text() == "请输入菜名")
    {
        QMessageBox::information(this, "搜索失败", "搜索内容不能为空");
        return;
    }
    ui->information->setText("搜索结果:");
    QString dish = ui->searchtext->text();
    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);
    clearcontainer(ui->dynamic1);
    if(this->ismanager)
    {

    }
    else
    {
    // QString search = R"(
    //     SELECT
    //         菜品.菜品编号,
    //         菜品.菜品名称,
    //         菜品.价格,
    //         菜品.标签,
    //         菜品.菜品评分,
    //         窗口.窗口编号,
    //         食堂.食堂名称
    //     FROM 菜品
    //     INNER JOIN 窗口 ON 菜品.窗口编号 = 窗口.窗口编号
    //     INNER JOIN 食堂 ON 窗口.所属食堂编号 = 食堂.食堂编号
    //     WHERE 菜品.菜品名称 LIKE :dish
    // )";
    QString search = R"(
    SELECT *
    FROM 菜品窗口食堂视图
    WHERE 菜品名称 LIKE :dish
    ORDER BY 菜品评分 DESC
    )";
    query.prepare(search);
    query.bindValue(":dish", "%" + dish + "%");

    if (!query.exec()) {
        qDebug() << "查询失败:" << query.lastError().text();
        return;
    }

    // 使用 QScrollArea 包裹内容
    QScrollArea* scrollArea = ui->dynamic1;
    scrollArea->setWidgetResizable(true);
    QWidget* container = new QWidget();
    scrollArea->setWidget(container);

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);
    qDebug()<<9999999;
    bool hasData = false;
    while (query.next()) {
        qDebug()<<1111199;
        hasData = true;
        QString name = query.value("菜品名称").toString();
        float price = query.value("价格").toFloat();
        QString label = query.value("标签").toString();
        int dish_id = query.value("菜品编号").toInt();
        int windowId = query.value("窗口编号").toInt();
        QString canteenName = query.value("食堂名称").toString();
        int score=query.value("菜品评分").toInt();

        // 创建 QCommandLinkButton
        QCommandLinkButton* button = new QCommandLinkButton(container);
        button->setMaximumHeight(100);
        button->setText(name);  // 主标题（菜品名称）
        button->setDescription(
            "所在食堂: " + canteenName + "  " +
            "所在窗口: " + QString::number(windowId) + "  " +
            "标签: " + label + "  " +
            "价格: " + QString::number(price)+ "  "
            +"评分:"+QString::number(score)
            );  // 副标题（标签、价格等）

        connect(button, &QCommandLinkButton::clicked, this, [this, dish_id]() {
            this->evaluationPage(dish_id);
            this->dishselect = dish_id;
        });
        layout->addWidget(button);
    }

    // 如果没有数据，显示提示信息
    if (!hasData) {
        QLabel* text = new QLabel("暂无菜品", container);
        layout->addWidget(text);
    }
    layout->addStretch();
    }
}




void MainWindow::on_adddish_clicked()
{
    clearcontainer(ui->dynamic1);
    // 创建一个容器 QWidget 用来放置控件
    QWidget* container = new QWidget();  // 可以选择 QScrollArea 或 QWidget，根据需要
    QVBoxLayout* mainLayout = new QVBoxLayout(container);  // 设置主布局

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
        query.bindValue(":name", name);
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
            this->portPage(this->portselect);
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
    // 将主布局的容器添加到 ScrollArea 或 QFrame 中
    QScrollArea* scrollArea = ui->dynamic1;  // 获取 QScrollArea
    scrollArea->setWidget(container);  // 设置内容区域
    scrollArea->setWidgetResizable(true);  // 支持自动调整大小

}






void MainWindow::on_addevaluation_clicked()
{

    clearcontainer(ui->dynamic1);
    // 创建一个容器 QWidget 用来放置控件
    QWidget* container = new QWidget();  // 可以选择 QScrollArea 或 QWidget，根据需要
    QVBoxLayout* mainLayout = new QVBoxLayout(container);  // 设置主布局
    // 将主布局的容器添加到 ScrollArea 或 QFrame 中
    QScrollArea* scrollArea = ui->dynamic1;  // 获取 QScrollArea
    scrollArea->setWidget(container);  // 设置内容区域
    scrollArea->setWidgetResizable(true);  // 支持自动调整大小

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


        int userId = this->userId;
        int dishId = this->dishselect;
        int score = scoreBox->currentText().toInt();// 评分，假设是 QSpinBox
        QString evaluationText = evaluation->toPlainText().trimmed();
        // if(evaluationText=="")
        // {
        //     db.rollback();
        //     QMessageBox::critical(this, "数据库错误", "评价内容不能为空：" + query.lastError().text());
        //     return;
        // }
    //     QString insertSql = R"(
    //     INSERT INTO 评价 (用户编号, 菜品编号, 评分, 评价)
    //     VALUES (:user_id, :dish_id, :score, :evaluation)
    // )";

    //     query.prepare(insertSql);
    //     query.bindValue(":user_id", userId);
    //     query.bindValue(":dish_id", dishId);
    //     query.bindValue(":score", score);
    //     query.bindValue(":evaluation", evaluationText);

    //     if (!query.exec()) {
    //         db.rollback();
    //         QMessageBox::critical(this, "数据库错误", "添加评价失败：" + query.lastError().text());
    //         return;
    //     }

        QString callSql = "CALL InsertEvaluation(:user_id, :dish_id, :score, :evaluation)";

        query.prepare(callSql);
        query.bindValue(":user_id", userId);
        query.bindValue(":dish_id", dishId);
        query.bindValue(":score", score);
        query.bindValue(":evaluation", evaluationText);

        if (!query.exec()) {
            db.rollback();
            QMessageBox::critical(this, "数据库错误", "添加评价失败：评价不能为空");
            return;
        }

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
        float avgScore = 0.0;
        if (query.next()) {
            avgScore = query.value(0).toFloat();
        }
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
        this->recommend();
        this->evaluationPage(this->dishselect);
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
    clearcontainer(ui->dynamic1);
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
    QWidget* container = new QWidget();  // 可以选择 QScrollArea 或 QWidget，根据需要
    QVBoxLayout* layout = new QVBoxLayout(container);  // 设置主布局
    // 将主布局的容器添加到 ScrollArea 或 QFrame 中
    QScrollArea* scrollArea = ui->dynamic1;  // 获取 QScrollArea
    scrollArea->setWidget(container);  // 设置内容区域
    scrollArea->setWidgetResizable(true);  // 支持自动调整大小
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
            if (!db.transaction()) {
                QMessageBox::critical(this, "删除失败", "启动事务失败：" + db.lastError().text());
                return;
            }
            query.prepare("select * from 管理员 WHERE 用户编号 = :userId");
            query.bindValue(":userId", userId);
            query.exec();
            if(query.next())
            {
                // 说明查到了，当前用户是管理员
                QMessageBox::warning(this, "操作禁止", "管理员账号不能删除！");
                db.rollback();
                return;
            }
            query.prepare("DELETE FROM 评价 WHERE 用户编号 = :userId");
            query.bindValue(":userId", userId);
            if (!query.exec()) {
                db.rollback();
                QMessageBox::critical(this, "删除失败", "删除评论失败：" + query.lastError().text());
                return;
            }
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
                db.rollback();
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

void MainWindow::on_deletedish_clicked()
{

    int dishId = this->dishselect;  // 假设菜品 ID 存储在 dishselect 变量中


    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", "您确定要删除该菜品吗？", QMessageBox::Yes | QMessageBox::No);


    if (reply == QMessageBox::Yes) {
        QSqlDatabase &db = Database::getInstance().getDb();
        QSqlQuery query(db);

        // 启动数据库事务
        db.transaction();


        QString deleteCommentsSql = R"(
            DELETE FROM 评价
            WHERE 菜品编号 = :dishId
        )";
        query.prepare(deleteCommentsSql);
        query.bindValue(":dishId", dishId);

        if (!query.exec()) {
            db.rollback();
            QMessageBox::critical(this, "删除失败", "删除评论时发生错误：" + query.lastError().text());
            return;
        }


        QString deleteDishSql = R"(
            DELETE FROM 菜品
            WHERE 菜品编号 = :dishId
        )";
        query.prepare(deleteDishSql);
        query.bindValue(":dishId", dishId);

        if (!query.exec()) {
            db.rollback();
            QMessageBox::critical(this, "删除失败", "删除菜品时发生错误：" + query.lastError().text());
            return;
        }

        db.commit();

        QMessageBox::information(this, "删除成功", "菜品已成功删除！");
        ui->deletedish->hide();
        ui->changedish->hide();
        this->dishPage(this->portselect);
    } else {

        return;
    }
}






void MainWindow::recommend()
{
    // 使用数据库查询，找出评分前十的菜品，并显示所在食堂和窗口信息
    QSqlDatabase& db = Database::getInstance().getDb();
    QSqlQuery query(db);

    QString search = R"(
        SELECT
            菜品.菜品编号,
            菜品.菜品名称,
            菜品.菜品评分,
            菜品.价格,
            食堂.食堂名称,
            窗口.窗口编号
        FROM 菜品
        INNER JOIN 窗口 ON 菜品.窗口编号 = 窗口.窗口编号
        INNER JOIN 食堂 ON 窗口.所属食堂编号 = 食堂.食堂编号
        ORDER BY 菜品.菜品评分 DESC
        LIMIT 10
    )";

    query.prepare(search);

    if (!query.exec()) {
        qDebug() << "查询失败:" << query.lastError().text();
        return;
    }

    // 清除原有推荐内容
    clearcontainer(ui->recommend);

    // 创建一个滚动区域显示推荐菜品
    QScrollArea* scrollArea = ui->recommend;
    scrollArea->setWidgetResizable(true);
    QWidget* container = new QWidget();
    scrollArea->setWidget(container);

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    bool hasData = false;
    while (query.next()) {
        hasData = true;
        QString name = query.value("菜品名称").toString();
        float rating = query.value("菜品评分").toFloat();
        float price = query.value("价格").toFloat();
        int dishId = query.value("菜品编号").toInt();
        QString canteenName = query.value("食堂名称").toString();  // 食堂名称
        QString windowCategory = query.value("窗口编号").toString();  // 窗口分类

        // 创建 QLabel 显示推荐菜品
        QString displayText = QString("菜品名称: %1\n评分: %2  价格: %3\n食堂: %4  窗口: %5")
                                  .arg(name).arg(rating).arg(price).arg(canteenName).arg(windowCategory);

        QLabel* label = new QLabel(displayText, container);
        label->setWordWrap(true);  // 设置文本换行显示
        label->setAlignment(Qt::AlignTop);  // 设置顶部对齐

        layout->addWidget(label);  // 将标签添加到布局中
    }

    // 如果没有数据，显示提示信息
    if (!hasData) {
        QLabel* noDataLabel = new QLabel("暂无推荐菜品", container);
        layout->addWidget(noDataLabel);
    }

    layout->addStretch();

}


