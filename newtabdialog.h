#ifndef CREATENEWTABDIALOG_H
#define CREATENEWTABDIALOG_H

#include <QDialog>
#include "ui_NewTabDialog.h"

class NewTabDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewTabDialog(QWidget *parent = 0);
    ~NewTabDialog();
    QString &GetNewTabName() { return newTabName; }

signals:

public slots:
    void OKButtonClicked();
    void CancelButtonClicked();

public:
    enum ClickedResult { OK, Cancel };
private:
    QString newTabName;
    Ui::NewTabDialog *dialog;

};

#endif // CREATENEWTABDIALOG_H
