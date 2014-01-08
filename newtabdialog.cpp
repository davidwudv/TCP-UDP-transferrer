#include "newtabdialog.h"

NewTabDialog::NewTabDialog(QWidget *parent) :
    QDialog(parent)
{
    dialog->setupUi(this);
    connect(dialog->OKButton, SIGNAL(clicked()), this, SLOT(OKButtonClicked()));
    connect(dialog->CancelButton, SIGNAL(clicked()), this, SLOT(CancelButtonClicked()));
}

NewTabDialog::~NewTabDialog()
{
    delete dialog;
}

void NewTabDialog::OKButtonClicked()
{
    nameOfNewTab = dialog->lineEdit->text();
    done(ClickedResult::OK);
}

void NewTabDialog::CancelButtonClicked()
{
    done(ClickedResult::Cancel);
}
