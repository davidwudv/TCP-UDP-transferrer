#ifndef __MAIN_H__
#define __MAIN_H__

#include <QTranslator>
#include <QApplication>
#include <QMainWindow>
#include "mainwindow.h"

class Sokit :public QApplication
{
	Q_OBJECT

public:
	Sokit(int& argc, char** argv);
	~Sokit();

	bool initTranslator();
	bool initUI();
	void show();
	void close();

private slots:
	void ontop();

private:
	void initFont();
	void initDefaultActionsName();

private:
    MainWindow m_wnd;
	QTranslator m_trans;
};

#endif //__MAIN_H__
