#include "adminmangager.h"
#include "ui_adminmangager.h"

adminMangager::adminMangager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::adminMangager)
{
    ui->setupUi(this);
}

adminMangager::~adminMangager()
{
    delete ui;
}
