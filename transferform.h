#ifndef __TRANSFERFORM_H__
#define __TRANSFERFORM_H__

#include "ui_transferform.h"
#include "baseform.h"
#include <QMessageBox>

class TransferSkt;

class TransferForm : public BaseForm
{
	Q_OBJECT

public:
	TransferForm(QWidget* p=0, Qt::WindowFlags f=0);
	~TransferForm();

protected:
	virtual bool initForm();
	virtual bool initHotkeys();
	virtual void initConfig();
	virtual void saveConfig();
	virtual void send(const QString& data, const QString& dir);
	virtual void kill(QStringList& list);

private slots:
	void trigger(bool start);
	void stop();

    //---------add by davidWu 2013/12/30---------
    //void triggerEx(bool start);
    void AddWhiteIP();
    void SelectAllWhiteIP();
    void DeleteWhiteIP();
    void AddTransferMap();
    void SelectAllTransferMap();
    void DeleteTransferMap();
    //---------end---------

    //void on_btnTrigger_clicked();

private:
    //TransferSkt* m_server;
	Ui::TransferForm m_ui;
    //---------add by davidWu 2013/12/30---------
    QMessageBox msgBox;
    QList<TransferSkt*> sktList;
    //---------end---------

};

#endif // __TRANSFERFORM_H__



