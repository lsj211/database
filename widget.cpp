#include "widget.h"
#include "ui_widget.h"  // 确保包含生成的UI头文件


Widget::Widget(QWidget *parent)
    : QWidget{parent}
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("登录");
}


Widget::~Widget()
{
    delete ui;
}






void Widget::on_user_hasfocus()
{
    ui->user->clear();
}

