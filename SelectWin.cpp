#include "SelectWin.h"
#include "ui_SelectWin.h"



SelectWin::SelectWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectWin)
{
    ui->setupUi(this);
    model = new QSqlTableModel;
}

SelectWin::~SelectWin()
{
    delete ui;
}

void SelectWin::on_selectBt_clicked()
{
    if (ui->userRb->isChecked())//员工信息
    {
        model->setTable("userinfo");
    }
    else if (ui->timeRb->isChecked())//考勤信息
    {
        model->setTable("attendance");
    }
    //设置一个过滤器
    //model->setFilter("name='neo'");
    model->select();
    ui->tableView->setModel(model);
}
