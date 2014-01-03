#include "toolkit.h"
#include "setting.h"
#include "transferskt.h"
#include "transferform.h"

#include <QShortcut>

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

TransferForm::TransferForm(QWidget *parent, Qt::WindowFlags flags)
: BaseForm(parent, flags)//,m_server(0)
{
	m_ui.setupUi(this);
}

TransferForm::~TransferForm()
{
//	if (m_server && lock(1000))
//	{
//		m_server->disconnect(this);
//		delete m_server;
//		m_server = NULL;

//		unlock();
//	}

    //------davidWu 2013/12/31
    if(lock(1000))
    {
        TransferSkt *skt;
        while(sktList.size() > 0)
        {
            skt = sktList.first();
            if(skt)
            {
                skt->stop();
                skt->disconnect(this);
                delete skt;
                sktList.removeFirst();
            }
        }
        unlock();
    }
    //end

	saveConfig();
}

void TransferForm::initConfig()
{
	QString sst(SET_SEC_TRANS);
    Setting::load(sst+SET_KEY_CMBSA, SET_PFX_CMBITM, *m_ui.cmbSrcAddr, false);
    Setting::load(sst+SET_KEY_CMBDA, SET_PFX_CMBITM, *m_ui.cmbDstAddr);
    Setting::load(sst+SET_KEY_CMBSP, SET_PFX_CMBITM, *m_ui.cmbSrcPort);
    Setting::load(sst+SET_KEY_CMBDP, SET_PFX_CMBITM, *m_ui.cmbDstPort);
    //---------add by daivdWu 2013/12/31---------
    Setting::load();
    QStringList::iterator it = TransferSkt::WhiteIPList.begin();
    for(; it != TransferSkt::WhiteIPList.end(); ++it)
    {
        m_ui.lstWhiteList->addItem(*it);
    }

    QList<TransferInfo>::iterator iter = TransferSkt::TransferMap.begin();
    for(; iter != TransferSkt::TransferMap.end(); ++iter)
    {
        QString item(iter->srcHost.IPAddress);
        item += ": ";
        item += QString::number(iter->srcHost.Port);
        item += "--->";
        item += iter->dstHost.IPAddress;
        item += ": ";
        item += QString::number(iter->dstHost.Port);
        m_ui.lstTransferMaps->addItem(item);
    }
    //---------end---------

	QString skl(SET_SEC_DIR); skl += SET_KEY_LOG;
	skl = Setting::get(skl, SET_KEY_TRANS, SET_VAL_LGTAN);
	setProperty(SET_SEC_DIR, skl);

	TK::initNetworkInterfaces(m_ui.cmbSrcAddr, true);
	TK::initNetworkInterfaces(m_ui.cmbDstAddr);
}

void TransferForm::saveConfig()
{
	QString sst(SET_SEC_TRANS);
    Setting::save(sst+SET_KEY_CMBSA, SET_PFX_CMBITM, *m_ui.cmbSrcAddr, false);
	Setting::save(sst+SET_KEY_CMBDA, SET_PFX_CMBITM, *m_ui.cmbDstAddr);
	Setting::save(sst+SET_KEY_CMBSP, SET_PFX_CMBITM, *m_ui.cmbSrcPort);
	Setting::save(sst+SET_KEY_CMBDP, SET_PFX_CMBITM, *m_ui.cmbDstPort);
    //---------add by daivdWu 2013/12/31---------
    Setting::save();
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
    connect(m_ui.btnAddIPMap, SIGNAL(clicked()), this, SLOT(AddTransferMap()));
    connect(m_ui.btnSelectAllTransferMap, SIGNAL(clicked()), this, SLOT(SelectAllTransferMap()));
    connect(m_ui.btnTransferMapDel, SIGNAL(clicked()), this, SLOT(DeleteTransferMap()));
    connect(m_ui.btnAddWhiteIP, SIGNAL(clicked()), this, SLOT(AddWhiteIP()));
    connect(m_ui.btnSelectAllWhiteList, SIGNAL(clicked()), this, SLOT(SelectAllWhiteIP()));
    connect(m_ui.btnWhiteListIPDel, SIGNAL(clicked()), this, SLOT(DeleteWhiteIP()));

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
//	if (m_server)
//	{
//		while (!list.isEmpty())
//			m_server->kill(list.takeFirst());
//	}

    //-------davidWu 2013/12/31------
    for(int i = 0; i < sktList.size(); ++i)
    {
        if(sktList[i])
            while(!list.isEmpty())
                sktList[i]->kill(list.takeFirst());
    }
    //--------end--------
}

//void TransferForm::trigger(bool start)
//{
//	if (lock(1000))
//	{
//		if (m_server)
//		{
//			m_server->stop();
//			m_server->disconnect(this);
//			delete m_server;
//            m_server = NULL;
//		}

//		IPAddr sa, da;
//		if (start)
//		{
//			start = TK::popIPAddr(m_ui.cmbSrcAddr, m_ui.cmbSrcPort, sa) &&
//					TK::popIPAddr(m_ui.cmbDstAddr, m_ui.cmbDstPort, da);
//		}

//		if (start)
//		{
//			QString type = m_ui.cmbType->currentText();
//			if (type.contains(TK::socketTypeName(true)))
//				m_server = new TransferSktTcp(this);
//			else
//				m_server = new TransferSktUdp(this);

//			if (m_server)
//			{
//				connect(m_server, SIGNAL(connOpen(const QString&)), this, SLOT(listerAdd(const QString&)));
//				connect(m_server, SIGNAL(connClose(const QString&)), this, SLOT(listerRemove(const QString&)));
//				connect(m_server, SIGNAL(message(const QString&)), this, SIGNAL(output(const QString&)));
//				connect(m_server, SIGNAL(dumpbin(const QString&,const char*,quint32)), this, SIGNAL(output(const QString&,const char*,quint32)));
//				connect(m_server, SIGNAL(countRecv(qint32)), this, SLOT(countRecv(qint32)));
//				connect(m_server, SIGNAL(countSend(qint32)), this, SLOT(countSend(qint32)));
//				connect(m_server, SIGNAL(stopped()), this, SLOT(stop()));
				
//				start = m_server->start(sa.ip, sa.port, da.ip, da.port);
//				if (!start)
//				{
//					m_server->disconnect(this);
//					delete m_server;
//					m_server = NULL;
//				}
//			}
//			else
//			{
//				start = false;
//			}
//		}

//		unlock();
//	}

//	m_ui.cmbSrcAddr->setDisabled(start);
//	m_ui.cmbSrcPort->setDisabled(start);
//	m_ui.cmbDstAddr->setDisabled(start);
//	m_ui.cmbDstPort->setDisabled(start);
//	m_ui.cmbType->setDisabled(start);
//    m_ui.btnAddIPMap->setDisabled(start);

//	if (start)
//	{
//		TK::pushIPAddr(m_ui.cmbSrcPort, m_ui.cmbSrcAddr);
//		TK::pushIPAddr(0, m_ui.cmbDstPort);
//        m_ui.btnTrigger->setText("Stop");
//	}
//	else
//	{
//		TK::resetPushBox(m_ui.btnTrigger);
//        m_ui.btnTrigger->setText("Start");
//	}
//}

void TransferForm::stop()
{
	trigger(false);
}

void TransferForm::send(const QString& data, const QString& dir)
{
	bool s2d = dir.startsWith('S');

//	if (m_server &&lock(1000))
//	{
//		QStringList list;
//		listerSelected(list);

//		while (!list.isEmpty())
//			m_server->send(list.takeFirst(), s2d, data);

//		unlock();
//	}

    //-------davidWu 2013/12/31
    if(lock(1000))
    {
        for(int i = 0; i < sktList.size(); ++i)
        {
            if(sktList[i])
            {
                QStringList list;
                listerSelected(list);

                while (!list.isEmpty())
                    sktList[i]->send(list.takeFirst(), s2d, data);
            }
        }
        unlock();
    }
    //--------end----------
}


//---------add by davidWu 2013/12/31---------
void TransferForm::AddWhiteIP()
{
    if(TransferSkt::WhiteIPList.size() >= WHITE_LIST_RANGE)
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
    if(m_ui.lstWhiteList->findItems(IP, Qt::MatchFixedString).size() != 0)
    {
        msgBox.setText(tr("Error: "));
        msgBox.setInformativeText(tr("Please don't enter a same IP!!"));
        msgBox.exec();
        return;
    }
    m_ui.lstWhiteList->addItem(IP);
    TransferSkt::WhiteIPList.push_back(IP);
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
        TransferSkt::WhiteIPList.removeAt(index);
    }
}

void TransferForm::AddTransferMap()
{
    QString srcIP = m_ui.cmbSrcAddr->currentText();
    QString dstIP = m_ui.cmbDstAddr->currentText();
    QString srcPortStr = m_ui.cmbSrcPort->currentText();
    QString dstPortStr = m_ui.cmbDstPort->currentText();

    if(srcIP.isEmpty() || dstIP.isEmpty() || srcPortStr.isEmpty() || dstPortStr.isEmpty())
        return;
    QRegExp regExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    if(!regExp.exactMatch(srcIP) || !regExp.exactMatch(dstIP))
    {
        msgBox.setText(tr("Error: "));
        msgBox.setInformativeText(tr("Please enter a valid IP address!!"));
        msgBox.exec();
        return;
    }
    QString str = srcIP + ": " + srcPortStr + "--->" + dstIP + ": " + dstPortStr;
//    if(m_ui.cmbType->currentIndex() == 0)
//        str += tr("(TCP)");
//    else if(m_ui.cmbType->currentIndex() == 1)
//        str += tr("(UDP)");
    if(m_ui.lstTransferMaps->findItems(str, Qt::MatchFixedString).size() != 0)
    {
        msgBox.setText(tr("Error: "));
        msgBox.setInformativeText(tr("Please don't enter a same item!!"));
        msgBox.exec();
        return;
    }
    m_ui.lstTransferMaps->addItem(str);
    TransferInfo hostInfo;
    hostInfo.dstHost.IPAddress = m_ui.cmbDstAddr->currentText();
    hostInfo.dstHost.Port = m_ui.cmbDstPort->currentText().toUShort();
    hostInfo.srcHost.IPAddress = m_ui.cmbSrcAddr->currentText();
    hostInfo.srcHost.Port = m_ui.cmbSrcPort->currentText().toUShort();
    TransferSkt::TransferMap.push_back(hostInfo);

    if(m_ui.cmbSrcAddr->findText(srcIP) < 0)
        m_ui.cmbSrcAddr->addItem(srcIP);
    if(m_ui.cmbDstAddr->findText(dstIP) < 0)
        m_ui.cmbDstAddr->addItem(dstIP);
    if(m_ui.cmbDstPort->findText(dstPortStr) < 0)
        m_ui.cmbDstPort->addItem(dstPortStr);
    if(m_ui.cmbSrcPort->findText(srcPortStr) < 0)
        m_ui.cmbSrcPort->addItem(srcPortStr);
    m_ui.cmbSrcAddr->setCurrentText("");
    m_ui.cmbDstAddr->setCurrentText("");
    m_ui.cmbDstPort->setCurrentText("");
    m_ui.cmbSrcPort->setCurrentText("");

    TK::pushIPAddr(m_ui.cmbSrcPort, m_ui.cmbSrcAddr);
    TK::pushIPAddr(m_ui.cmbDstAddr, m_ui.cmbDstPort);
}

void TransferForm::SelectAllTransferMap()
{
    m_ui.lstTransferMaps->selectAll();
}

void TransferForm::DeleteTransferMap()
{
    QList<QListWidgetItem*> list = m_ui.lstTransferMaps->selectedItems();
    const int size = list.size();
    if(size == 0)
        return;
    for(int i = 0; i < size; ++i)
    {
        QListWidgetItem *item = list[i];
        int index = m_ui.lstTransferMaps->row(item);
        m_ui.lstTransferMaps->takeItem(index);
        TransferSkt::TransferMap.removeAt(index);
    }
}

void TransferForm::trigger(bool start)
{
    if (lock(1000))
    {
        TransferSkt *skt = NULL;
        while(sktList.size() > 0)
        {
            skt = sktList.first();
            if(skt)
            {
                skt->stop();
                skt->disconnect(this);
                delete skt;
                sktList.removeFirst();
            }
        }

        IPAddr sa, da;
        QList<TransferInfo>::iterator it = TransferSkt::TransferMap.begin();
        for(; it != TransferSkt::TransferMap.end(); ++it)
        {
            if (start)
            {
                sa.ip.setAddress(it->srcHost.IPAddress);
                sa.port = it->srcHost.Port;
                da.ip.setAddress(it->dstHost.IPAddress);
                da.port = it->dstHost.Port;

                QString type = m_ui.cmbType->currentText();
                TransferSkt *server;
                if (type.contains(TK::socketTypeName(true)))
                    server = new TransferSktTcp(this);
                else
                    server = new TransferSktUdp(this);

                if (server)
                {
                    connect(server, SIGNAL(connOpen(const QString&)), this, SLOT(listerAdd(const QString&)));
                    connect(server, SIGNAL(connClose(const QString&)), this, SLOT(listerRemove(const QString&)));
                    connect(server, SIGNAL(message(const QString&)), this, SIGNAL(output(const QString&)));
                    connect(server, SIGNAL(dumpbin(const QString&,const char*,quint32)), this, SIGNAL(output(const QString&,const char*,quint32)));
                    connect(server, SIGNAL(countRecv(qint32)), this, SLOT(countRecv(qint32)));
                    connect(server, SIGNAL(countSend(qint32)), this, SLOT(countSend(qint32)));
                    connect(server, SIGNAL(stopped()), this, SLOT(stop()));

                    start = server->start(sa.ip, sa.port, da.ip, da.port);
                    if (!start)
                    {
                        server->disconnect(this);
                        delete server;
                    }
                    sktList.push_back(server);
                }
                else
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
    m_ui.btnAddIPMap->setDisabled(start);
    m_ui.lstTransferMaps->setDisabled(start);
    m_ui.btnAddIPMap->setDisabled(start);
    m_ui.btnSelectAllTransferMap->setDisabled(start);
    m_ui.btnTransferMapDel->setDisabled(start);

    if (start)
    {
        m_ui.btnTrigger->setText("Stop");
    }
    else
    {
        TK::resetPushBox(m_ui.btnTrigger);
        m_ui.btnTrigger->setText("Start");
    }
}

//---------end---------
