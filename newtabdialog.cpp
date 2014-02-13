#include "newtabdialog.h"

NewTabDialog::NewTabDialog(QWidget *parent) :
    QDialog(parent), dialog(new Ui::NewTabDialog)
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
    newTabName = dialog->lineEdit->text();
    done(ClickedResult::OK);
}

void NewTabDialog::CancelButtonClicked()
{
    done(ClickedResult::Cancel);
}
