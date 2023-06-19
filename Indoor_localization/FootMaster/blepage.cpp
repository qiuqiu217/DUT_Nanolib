#include "blepage.h"
#include "ui_blepage.h"





BlePage::BlePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlePage)
{
    ui->setupUi(this);
    //蓝牙对象定义









}

BlePage::~BlePage()
{
    delete ui;
}
