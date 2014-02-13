//----------davidWu 2014/1/3-----------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define CONFIG_FILE "config.ini"

#include <QMainWindow>
#include <QTabWidget>
#include <QMessageBox>
#include <QSettings>
#include "ui_MainWindow.h"
#include "transferform.h"
#include "newtabdialog.h"

class TransferForm;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitTab();
    void SaveConfig();
protected:
    void LoadConfig();
    void closeEvent(QCloseEvent * event);
private:
    bool GetNameForTab(QString &name);

signals:

public slots:
    void AddNewTab();
    void CloseTab(int tabIndex);
    void CloseCurrentTab();
    void RenameTab(int tabIndex);

private:
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
    QMessageBox msgBox;
    QSettings *settings;
public:
    QList<TransferForm*> currentTabs;
};

#endif // MAINWINDOW_H
