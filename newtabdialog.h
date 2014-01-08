#ifndef CREATENEWTABDIALOG_H
#define CREATENEWTABDIALOG_H

#include <QDialog>
#include "ui_CreateNewTabDialog.h"

class NewTabDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewTabDialog(QWidget *parent = 0);
    ~NewTabDialog();
    enum ClickedResult
    {
        OK,
        Cancel
    };

signals:

public slots:
    void OKButtonClicked();
    void CancelButtonClicked();

private:
    QString nameOfNewTab;
    Ui::CreateNewTabDialog *dialog;

};

#endif // CREATENEWTABDIALOG_H
