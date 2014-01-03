#ifndef TABFORM_H
#define TABFORM_H

#include <QWidget>

namespace Ui {
class tabForm;
}

class tabForm : public QWidget
{
    Q_OBJECT

public:
    explicit tabForm(QWidget *parent = 0);
    ~tabForm();

private:
    Ui::tabForm *ui;
};

#endif // TABFORM_H
