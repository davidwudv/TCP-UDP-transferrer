#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <QFile>
#include <QTreeView>
#include <QCheckBox>
#include <QMenu>
#include <QTimer>

class QTreeWidget;
class QTreeWidgetItem;
class QPlainTextEdit;
class Logger : public QObject
{
	Q_OBJECT

public:
	Logger(QObject *parent = 0);
	~Logger();

    void init(QTreeWidget* o, QCheckBox* w, QPlainTextEdit* d
              /*---->add a param by davidWu 2014/1/2--->*/, QCheckBox *x);

	void clear();
	void output(const QString& title, const QString& info);
	void output(const char* buf, quint32 len);

signals:
    void clearLog();

public slots:
	void output(const QString& info);
	void output(const QString& title, const char* buf, quint32 len);

private slots:
	void ctxmenu(const QPoint& pos);
	void copy();
	void syncOutput();

private:
	const QString getLogFileName();
	void writeLogFile(const QString& info);
	void pack();
	QTreeWidgetItem* appendLogEntry(QTreeWidgetItem* p, const QString& t);

private:
	QString m_dir;
	QFile m_file;

	QMenu m_cmlog, m_cmtxt;
    QCheckBox* m_chkWrite;
    QTreeWidget* m_treeOut;
    QPlainTextEdit* m_textOut;
    /*----add by davidWu 2014/1/2---*/
    QCheckBox *m_chkOutput;
    /*---2014/2/11---*/
    QStringList logInfor;
    QVector<QPair<QString, QString> > logInforData;
public:
    QTimer *clearLogTimer;//定时检查&清理日志文件
    QTimer *updateTimer;//定时更行输出面板

protected slots:
    void checkAndClearLog();//每天凌晨12点检查&清理日志目录，只保留LOGFILE_MAX天日志
    void updateOutput();//更新输出面板

private:
    void createLogDir();
    /*------end------*/
};

#endif // __LOGGER_H__

