#include "tabform.h"
#include "ui_tabform.h"

tabForm::tabForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tabForm)
{
    ui->setupUi(this);
}

tabForm::~tabForm()
{
    delete ui;
}
