#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    settings = new QSettings(CONFIG_FILE, QSettings::IniFormat);
}

MainWindow::~MainWindow()
{
    SaveConfig();
    delete ui;
    delete settings;
}

void MainWindow::AddNewTab()
{
    QString name;
    if(!GetNameForTab(name))
        return;
    TransferForm* newTab = new TransferForm(name);
    newTab->init();
    tabWidget->addTab(newTab, name);
    tabWidget->setCurrentWidget((QWidget*)newTab);
    currentTabs.push_back(newTab);
}

void MainWindow::CloseTab(int tabIndex)
{
    TransferForm *tab = (TransferForm*)tabWidget->widget(tabIndex);
    if(tab->IsRunning())
    {
        msgBox.setInformativeText(tr("Are you sure?"));
        if(msgBox.exec() == QMessageBox::No)
            return;
    }
    currentTabs.removeAt(currentTabs.indexOf(tab));
    tab->deleteLater();
    tabWidget->removeTab(tabIndex);
    currentTabs.removeOne(tab);
}

void MainWindow::CloseCurrentTab()
{
    TransferForm *tab = (TransferForm*)tabWidget->currentWidget();
    if(tab->IsRunning())
    {
        msgBox.setInformativeText(tr("Are you sure?"));
        if(msgBox.exec() == QMessageBox::No)
            return;
    }
    currentTabs.removeAt(currentTabs.indexOf(tab));
    tab->deleteLater();
    tabWidget->removeTab(tabWidget->currentIndex());
    currentTabs.removeOne(tab);
}

bool MainWindow::GetNameForTab(QString &name)
{
    NewTabDialog *dialog = new NewTabDialog(this);
    NewTabDialog::ClickedResult res;
    QMessageBox msgBox;
    QString newTabName;

    res = (NewTabDialog::ClickedResult)dialog->exec();
    msgBox.setText(tr("Error: "));
    if(res == NewTabDialog::Cancel)
    {
        delete dialog;
        return false;
    }
    else if(res == NewTabDialog::OK)
    {
        newTabName = dialog->GetNewTabName();

        if(newTabName.isEmpty())
        {
            msgBox.setInformativeText(tr("Tab name is not allowed to be empty!!"));
            msgBox.exec();
            delete dialog;
            return false;
        }

        for(int i = 0; i < currentTabs.size(); ++i)
        {
            if(currentTabs[i]->tabTitle == newTabName)
            {
                msgBox.setInformativeText(tr("Tab name is not allowed to same!!"));
                msgBox.exec();
                delete dialog;
                return false;
            }
        }
    }
    name = newTabName;
    delete dialog;
    return true;
}

void MainWindow::RenameTab(int tabIndex)
{
    QString name;
    if(!GetNameForTab(name))
        return;
    tabWidget->setTabText(tabIndex, name);
    TransferForm *tab = (TransferForm*)tabWidget->widget(tabIndex);
    tab->tabTitle = name;
}

void MainWindow::InitTab()
{
    setWindowTitle(tr("Transferer", "transferer -- F1 for help"));
    setWindowIcon(QIcon(":/transfer.png"));
    QWidget* pnl = new QWidget(this);
    setCentralWidget(pnl);
    LoadConfig();
    tabWidget = new QTabWidget(pnl);
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    if(currentTabs.size() == 0)
    {
        TransferForm *transf = new TransferForm("Transfer");
        transf->init();
        currentTabs.push_back(transf);
        tabWidget->addTab(transf, transf->windowTitle());
    }
    else
    {
        for(int i = 0; i < currentTabs.size(); ++i)
        {
            TransferForm *tab = currentTabs[i];
            tabWidget->addTab(tab, tab->tabTitle);
        }
    }
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));
    connect(tabWidget, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(RenameTab(int)));
    connect(ui->actionClose_tab, SIGNAL(triggered()), this, SLOT(CloseCurrentTab()));
    connect(ui->actionNew_tab, SIGNAL(triggered()), this, SLOT(AddNewTab()));

    QLayout* lay = new QVBoxLayout(pnl);
    lay->setSpacing(5);
    lay->setContentsMargins(5, 5, 5, 5);
    lay->addWidget(tabWidget);
}

void MainWindow::SaveConfig()
{
    settings->remove("");
    settings->beginWriteArray("Tabs");
    for(int i = 0; i < currentTabs.size(); ++i)
    {
        settings->setArrayIndex(i);
        settings->setValue("TabTitle", currentTabs[i]->tabTitle);
    }
    settings->endArray();
    for(int i = 0; i < currentTabs.size(); ++i)
    {
        currentTabs[i]->Save(settings);
    }
}

void MainWindow::LoadConfig()
{
    currentTabs.clear();
    int size = settings->beginReadArray("Tabs");
    for(int i = 0; i < size; ++i)
    {
        settings->setArrayIndex(i);
        QString tabTitle = settings->value("TabTitle").toString();
        TransferForm *tab = new TransferForm(tabTitle);
        tab->init();
        currentTabs.push_back(tab);
    }
    settings->endArray();
    for(int i = 0; i < currentTabs.size(); ++i)
    {
        currentTabs[i]->Load(settings);
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    bool flag(false);
    for(int i = 0; i < currentTabs.size(); ++i)
    {
        if(currentTabs[i]->IsRunning())
        {
            flag = true;
            break;
        }
    }
    if(flag)
    {
        msgBox.setInformativeText(tr("Are you sure to exit ?"));
        if(msgBox.exec() == QMessageBox::No)
            event->ignore();
    }
}
