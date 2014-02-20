#include "toolkit.h"
#include "setting.h"
#include "transferskt.h"
#include "transferform.h"

#include <QShortcut>
#include <QMessageBox>

#define SET_SEC_TRANS  "transfer"
#define SET_KEY_TRANS  "/transfer"
#define SET_KEY_CMBSA  "/srcip"
#define SET_KEY_CMBDA  "/dstip"
#define SET_KEY_CMBSP  "/srcport"
#define SET_KEY_CMBDP  "/dstport"

#define SET_VAL_LGTAN  "log_transf"

#define PROP_EDIT "edit"
#define PROP_DIRT "dirt"

//---------add by daivdWu 2013/12/31---------
#define WHITE_LIST_RANGE 20  //白名单IP上限数
//---------end---------

TransferForm::TransferForm(QString title, QWidget *parent, Qt::WindowFlags flags)
: BaseForm(parent, flags),m_server(NULL), isRunning(false), tabTitle(title)
{
	m_ui.setupUi(this);
    initConfig();
}

TransferForm::~TransferForm()
{
    if (m_server && lock(1000))
    {
        m_server->disconnect(this);
        m_server->deleteLater();
        m_server = NULL;
        unlock();
    }

    //------davidWu 2013/12/31
//    if(lock(1000))
//    {
//        TransferSkt *skt;
//        while(sktList.size() > 0)
//        {
//            skt = sktList.first();
//            if(skt)
//            {
//                skt->stop();
//                skt->disconnect(this);
//                delete skt;
//                sktList.removeFirst();
//            }
//        }
//        unlock();
//    }
    //end

//	saveConfig();
}

void TransferForm::initConfig()
{
//	QString sst(SET_SEC_TRANS);
//    Setting::load(sst+SET_KEY_CMBSA, SET_PFX_CMBITM, *m_ui.cmbSrcAddr, false);
//    Setting::load(sst+SET_KEY_CMBDA, SET_PFX_CMBITM, *m_ui.cmbDstAddr);
//    Setting::load(sst+SET_KEY_CMBSP, SET_PFX_CMBITM, *m_ui.cmbSrcPort);
//    Setting::load(sst+SET_KEY_CMBDP, SET_PFX_CMBITM, *m_ui.cmbDstPort);
    //---------add by daivdWu 2013/12/31---------
//    Setting::load();
//    QStringList::iterator it = TransferSkt::whiteIPList.begin();
//    for(; it != TransferSkt::whiteIPList.end(); ++it)
//    {
//        m_ui.lstWhiteList->addItem(*it);
//    }
    //---------end---------

//    QString skl(SET_SEC_DIR); skl += SET_KEY_LOG;
//    skl = Setting::get(skl, SET_KEY_TRANS, SET_VAL_LGTAN);
     //---------add by daivdWu 2014/1/7---------
    QString savePath("log/" + tabTitle);
    setProperty(SET_SEC_DIR, savePath);
    //---------end---------

	TK::initNetworkInterfaces(m_ui.cmbSrcAddr, true);
//	TK::initNetworkInterfaces(m_ui.cmbDstAddr);
}

void TransferForm::saveConfig()
{
//	QString sst(SET_SEC_TRANS);
//    Setting::save(sst+SET_KEY_CMBSA, SET_PFX_CMBITM, *m_ui.cmbSrcAddr, false);
//	Setting::save(sst+SET_KEY_CMBDA, SET_PFX_CMBITM, *m_ui.cmbDstAddr);
//	Setting::save(sst+SET_KEY_CMBSP, SET_PFX_CMBITM, *m_ui.cmbSrcPort);
//	Setting::save(sst+SET_KEY_CMBDP, SET_PFX_CMBITM, *m_ui.cmbDstPort);
    //---------add by daivdWu 2013/12/31---------
//    Setting::save();
    //---------end---------

    QString skl(SET_SEC_DIR); skl += SET_KEY_LOG;
	Setting::set(skl, SET_KEY_TRANS, property(SET_SEC_DIR).toString());
}

bool TransferForm::initForm()
{
	initCounter(m_ui.labRecv, m_ui.labSend);
    initLogger(m_ui.chkLog, m_ui.btnClear, m_ui.treeOutput, m_ui.txtOutput, m_ui.CheckBox_Output);
	initLister(m_ui.btnConnAll, m_ui.btnConnDel, m_ui.lstConn);

//	bindBuffer(1, m_ui.edtBuf1, m_ui.btnSend1, m_ui.cmbDir1);
//	bindBuffer(2, m_ui.edtBuf2, m_ui.btnSend2, m_ui.cmbDir2);
//	bindBuffer(3, m_ui.edtBuf3, m_ui.btnSend3, m_ui.cmbDir3);

//	bindSelect(m_ui.cmbDir1, -1, Qt::Key_F5);
//	bindSelect(m_ui.cmbDir2, -1, Qt::Key_F6);
//	bindSelect(m_ui.cmbDir3, -1, Qt::Key_F7);

	connect(m_ui.btnTrigger, SIGNAL(clicked(bool)), this, SLOT(trigger(bool)));

    //---------add by davidWu 2013/12/31---------
    connect(m_ui.btnAddWhiteIP, SIGNAL(clicked()), this, SLOT(AddWhiteIP()));
    connect(m_ui.btnSelectAllWhiteList, SIGNAL(clicked()), this, SLOT(SelectAllWhiteIP()));
    connect(m_ui.btnWhiteListIPDel, SIGNAL(clicked()), this, SLOT(DeleteWhiteIP()));
    connect(m_ui.checkBox_turnOnWhiteList, SIGNAL(clicked(bool)), this, SLOT(SetWhiteListEnabled(bool)));

    QRegExp regExpIP("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExp regExpNumber("^[0-9]*$");
    m_ui.cmbSrcAddr->setValidator(new QRegExpValidator(regExpIP, this));
    m_ui.cmbSrcPort->setValidator(new QRegExpValidator(regExpNumber, this));
    m_ui.cmbDstAddr->setValidator(new QRegExpValidator(regExpIP, this));
    m_ui.cmbDstPort->setValidator(new QRegExpValidator(regExpNumber, this));
    //---------end---------

	return true;
}

bool TransferForm::initHotkeys()
{
	bindFocus(m_ui.cmbSrcAddr, Qt::Key_Escape);
	bindClick(m_ui.btnTrigger,  Qt::CTRL + Qt::Key_S);
	bindSelect(m_ui.cmbType, 0, Qt::CTRL + Qt::Key_T);
	bindSelect(m_ui.cmbType, 1, Qt::CTRL + Qt::Key_U);

	return true;
}

void TransferForm::kill(QStringList& list)
{
    if (m_server)
    {
        while (!list.isEmpty())
            m_server->kill(list.takeFirst());
    }

    //-------davidWu 2013/12/31------
//    for(int i = 0; i < sktList.size(); ++i)
//    {
//        if(sktList[i])
//            while(!list.isEmpty())
//                sktList[i]->kill(list.takeFirst());
//    }
    //--------end--------
}

void TransferForm::trigger(bool start)
{
    if(m_ui.cmbDstAddr->currentText().isEmpty() || m_ui.cmbDstPort->currentText().isEmpty() ||
            m_ui.cmbSrcAddr->currentText().isEmpty() || m_ui.cmbSrcPort->currentText().isEmpty())
    {
        m_ui.btnTrigger->setChecked(false);
        return;
    }

    isRunning = start;
    if (lock(1000))
    {
        if (m_server)
        {
            m_server->stop();
            m_server->disconnect(this);
            m_server->deleteLater();
            m_server = NULL;
        }

        IPAddr sa, da;
        if (start)
        {
            start = TK::popIPAddr(m_ui.cmbSrcAddr, m_ui.cmbSrcPort, sa) &&
                    TK::popIPAddr(m_ui.cmbDstAddr, m_ui.cmbDstPort, da);
        }

        if (start)
        {
            QString type = m_ui.cmbType->currentText();
            if (type.contains(TK::socketTypeName(true)))
                m_server = new TransferSktTcp(&whiteIPList, this);
            else
                m_server = new TransferSktUdp(&whiteIPList, this);

            if (m_server)
            {
                connect(m_server, SIGNAL(connOpen(const QString&)), this, SLOT(listerAdd(const QString&)));
                connect(m_server, SIGNAL(connClose(const QString&)), this, SLOT(listerRemove(const QString&)));
                connect(m_server, SIGNAL(message(const QString&)), this, SIGNAL(output(const QString&)));
                connect(m_server, SIGNAL(dumpbin(const QString&,const char*,quint32)), this, SIGNAL(output(const QString&,const char*,quint32)));
                connect(m_server, SIGNAL(countRecv(qint32)), this, SLOT(countRecv(qint32)));
                connect(m_server, SIGNAL(countSend(qint32)), this, SLOT(countSend(qint32)));
                connect(m_server, SIGNAL(stopped()), this, SLOT(stop()));

                m_server->whiteIPListEnabled = m_ui.checkBox_turnOnWhiteList->isChecked();
                start = m_server->start(sa.ip, sa.port, da.ip, da.port);
                if (!start)
                {
                    m_server->disconnect(this);
                    m_server->deleteLater();
                    m_server = NULL;
                }
            }
            else
            {
                start = false;
            }
        }

        unlock();
    }

    m_ui.cmbSrcAddr->setDisabled(start);
    m_ui.cmbSrcPort->setDisabled(start);
    m_ui.cmbDstAddr->setDisabled(start);
    m_ui.cmbDstPort->setDisabled(start);
    m_ui.cmbType->setDisabled(start);

    if (start)
    {
        TK::pushIPAddr(m_ui.cmbSrcPort, m_ui.cmbSrcAddr);
        TK::pushIPAddr(m_ui.cmbDstAddr, m_ui.cmbDstPort);
        m_ui.btnTrigger->setText("Stop");
        timer->start(3000);
        m_logger.clearLogTimer->start(86400000 - QTime::currentTime().msecsSinceStartOfDay());
        m_logger.updateTimer->start(3000);
    }
    else
    {
        TK::resetPushBox(m_ui.btnTrigger);
        m_ui.btnTrigger->setText("Start");
        timer->stop();
        m_logger.clearLogTimer->stop();
        m_logger.updateTimer->stop();
    }
}

void TransferForm::stop()
{
	trigger(false);
}

void TransferForm::send(const QString& data, const QString& dir)
{
	bool s2d = dir.startsWith('S');

    if (m_server &&lock(1000))
	{
		QStringList list;
		listerSelected(list);

		while (!list.isEmpty())
			m_server->send(list.takeFirst(), s2d, data);

		unlock();
	}

    //-------davidWu 2013/12/31
//    if(lock(1000))
//    {
//        for(int i = 0; i < sktList.size(); ++i)
//        {
//            if(sktList[i])
//            {
//                QStringList list;
//                listerSelected(list);

//                while (!list.isEmpty())
//                    sktList[i]->send(list.takeFirst(), s2d, data);
//            }
//        }
//        unlock();
//    }
    //--------end----------
}


//---------add by davidWu 2013/12/31---------
void TransferForm::AddWhiteIP()
{
    if(whiteIPList.size() >= WHITE_LIST_RANGE)
    {
        msgBox.setText(tr("Error: "));
        msgBox.setInformativeText(tr("A maximum of IP address is 20."));
        msgBox.exec();
        return;
    }
    QString IP = m_ui.cmbWhiteIPAddr->currentText();

    if(IP.isEmpty())
        return;
    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    if(!regExp.exactMatch(IP))
    {
        msgBox.setText(tr("Error: "));
        msgBox.setInformativeText(tr("Please enter a valid IP address!!"));
        msgBox.exec();
        return;
    }

    if(m_ui.lstWhiteList->findItems(IP, Qt::MatchFixedString).size() != 0 && whiteIPList.contains(IP))
    {
        msgBox.setText(tr("Error: "));
        msgBox.setInformativeText(tr("Please don't enter a same IP!!"));
        msgBox.exec();
        m_ui.cmbWhiteIPAddr->setCurrentText("");
        return;
    }
    m_ui.lstWhiteList->addItem(IP);
    whiteIPList.push_back(IP);
    if(m_ui.cmbWhiteIPAddr->findText(IP) < 0)
        m_ui.cmbWhiteIPAddr->addItem(IP);
    m_ui.cmbWhiteIPAddr->setCurrentText("");
}

void TransferForm::SelectAllWhiteIP()
{
    m_ui.lstWhiteList->selectAll();
}

void TransferForm::DeleteWhiteIP()
{
    QList<QListWidgetItem*> list = m_ui.lstWhiteList->selectedItems();
    const int size = list.size();
    if(size == 0)
        return;
    for(int i = 0; i < size; ++i)
    {
        QListWidgetItem *item = list[i];
        int index = m_ui.lstWhiteList->row(item);
        m_ui.lstWhiteList->takeItem(index);
        whiteIPList.removeAt(index);
    }
}

void TransferForm::SetWhiteListEnabled(bool enable)
{
    m_ui.tab_whiteList->setEnabled(enable);
    if(m_server)
        m_server->whiteIPListEnabled = enable;
}

void TransferForm::Save(QSettings *settings)
{
    QString whiteListString;
    QString currentItemString;
    QString allDstAddrItems;
    QString allSrcPortItems;
    QString allDstPortItems;
    QString allWhiteIPItems;
    int size = whiteIPList.size();
    int srcPortCount = m_ui.cmbSrcPort->count();
    int dstAddrCount = m_ui.cmbDstAddr->count();
    int dstPortCount = m_ui.cmbDstPort->count();
    int whiteIPCount = m_ui.cmbWhiteIPAddr->count();
    int outputEnabled = (int)m_ui.CheckBox_Output->checkState();
    int writeLogEnabled = (int)m_ui.chkLog->checkState();
    int whiteListEnabled = (int)m_ui.checkBox_turnOnWhiteList->checkState();

    for(int i = 0; i < size; ++i)
    {
        if(!whiteIPList[i].isEmpty())
            whiteListString += whiteIPList[i];
        if(i < size - 1)
            whiteListString += "|";
    }

    for(int i = 0; i < srcPortCount; ++i)
    {
        if(!m_ui.cmbSrcPort->itemText(i).isEmpty())
            allSrcPortItems += m_ui.cmbSrcPort->itemText(i);
        if(i < srcPortCount - 1)
            allSrcPortItems += "|";
    }

    for(int i = 0; i < dstAddrCount; ++i)
    {
        if(!m_ui.cmbDstAddr->itemText(i).isEmpty())
            allDstAddrItems += m_ui.cmbDstAddr->itemText(i);
        if(i < dstAddrCount - 1)
            allDstAddrItems += "|";
    }

    for(int i = 0; i < dstPortCount; ++i)
    {
        if(!m_ui.cmbDstPort->itemText(i).isEmpty())
            allDstPortItems += m_ui.cmbDstPort->itemText(i);
        if(i < dstPortCount - 1)
            allDstPortItems += "|";
    }

    for(int i = 0; i < whiteIPCount; ++i)
    {
        if(!m_ui.cmbWhiteIPAddr->itemText(i).isEmpty())
            allWhiteIPItems += m_ui.cmbWhiteIPAddr->itemText(i);
        if(i < whiteIPCount - 1)
            allWhiteIPItems += "|";
    }

    if(!m_ui.cmbSrcAddr->currentText().isEmpty() && !m_ui.cmbSrcPort->currentText().isEmpty()
            && !m_ui.cmbDstAddr->currentText().isEmpty() && !m_ui.cmbDstPort->currentText().isEmpty())
    {
        currentItemString = m_ui.cmbSrcAddr->currentText() + "," + m_ui.cmbSrcPort->currentText();
        currentItemString += "|" + m_ui.cmbDstAddr->currentText() + "," + m_ui.cmbDstPort->currentText();
    }

    settings->beginGroup(tabTitle);
//    settings->remove("");
    settings->setValue("WhiteList", whiteListString);
    settings->setValue("CurrentItem", currentItemString);
    settings->setValue("SrcPortItems", allSrcPortItems);
    settings->setValue("DstAddrItems", allDstAddrItems);
    settings->setValue("DstPortItems", allDstPortItems);
    settings->setValue("WhiteIPItems", allWhiteIPItems);
    settings->setValue("OutputEnabled", outputEnabled);
    settings->setValue("WriteLogEnabled", writeLogEnabled);
    settings->setValue("WhiteListEnabled", whiteListEnabled);
    settings->endGroup();

}

void TransferForm::Load(QSettings *settings)
{
    QString tmpString;
    int index1(-1), index2(-1);

    whiteIPList.clear();
    settings->beginGroup(tabTitle);
    QString whiteListString = settings->value("WhiteList").toString().trimmed();
    if(!whiteListString.isEmpty())
        whiteIPList = whiteListString.split('|');
    tmpString = settings->value("CurrentItem").toString();
    if(!tmpString.isEmpty())
    {
        index1 = tmpString.indexOf(',');
        if(index1 > 0)
        {
            m_ui.cmbSrcAddr->setCurrentText(tmpString.left(index1));
            index2 = tmpString.indexOf('|');
            if(index2 > 0)
                m_ui.cmbSrcPort->setCurrentText(tmpString.mid(index1 + 1, index2 - index1 - 1));
        }
        index1 = tmpString.lastIndexOf(',');
        if(index1 > 0)
        {
            m_ui.cmbDstAddr->setCurrentText(tmpString.mid(index2 + 1, index1 - index2 -1));
            m_ui.cmbDstPort->setCurrentText(tmpString.right(tmpString.length() - index1 - 1));
        }
    }

    if(whiteIPList.size() > 0)
        m_ui.lstWhiteList->addItems(whiteIPList);
    QString SrcPortItems = settings->value("SrcPortItems").toString().trimmed();
    QString DstAddrItems = settings->value("DstAddrItems").toString().trimmed();
    QString DstPortItems = settings->value("DstPortItems").toString().trimmed();
    QString WhiteIPItems = settings->value("WhiteIPItems").toString().trimmed();
    Qt::CheckState outputEnabled = (Qt::CheckState)settings->value("OutputEnabled").toInt();
    Qt::CheckState writeLogEnabled = (Qt::CheckState)settings->value("WriteLogEnabled").toInt();
    Qt::CheckState whiteListEnabled = (Qt::CheckState)settings->value("WhiteListEnabled").toInt();

    if(!SrcPortItems.isEmpty())
        m_ui.cmbSrcPort->addItems(SrcPortItems.split('|'));
    if(!DstAddrItems.isEmpty())
        m_ui.cmbDstAddr->addItems(DstAddrItems.split('|'));
    if(!DstPortItems.isEmpty())
        m_ui.cmbDstPort->addItems(DstPortItems.split('|'));
    if(!WhiteIPItems.isEmpty())
        m_ui.cmbWhiteIPAddr->addItems(WhiteIPItems.split("|"));
    m_ui.CheckBox_Output->setCheckState(outputEnabled);
    m_ui.chkLog->setCheckState(writeLogEnabled);
    m_ui.checkBox_turnOnWhiteList->setCheckState(whiteListEnabled);
    m_ui.tab_whiteList->setEnabled((bool)whiteListEnabled);
    settings->endGroup();
}

//---------end---------
