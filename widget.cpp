#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_openCamera_clicked()
{

}

void Widget::on_detectImage_clicked()
{

}

void Widget::on_detectBarCode_clicked()
{

}

void Widget::on_closeCamera_clicked()
{

}
