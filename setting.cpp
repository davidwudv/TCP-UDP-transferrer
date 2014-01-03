#include <QComboBox>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

#include "setting.h"
#include "transferskt.h"

Setting::Setting()
{
	QString path(QDir::currentPath());
    if (!QFileInfo(path).isWritable() ||
        path == QDir::homePath ())
	{
		QDir dir(QDir::home());
		dir.mkdir("." SET_APP_NAME);
		if (dir.cd("." SET_APP_NAME))
			path = dir.absolutePath();
	}

	QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, path);
}

QSettings& Setting::storage()
{
	static Setting dummy; // to ensure call QSettings::setPath before create the g_settings
	static QSettings g_settings(QSettings::IniFormat, QSettings::UserScope, SET_APP_NAME);
	return g_settings;
}

QString Setting::path()
{
	return QFileInfo(storage().fileName()).dir().absolutePath();
}

void Setting::flush()
{
	storage().sync();
}

void Setting::set(const QString& section, const QString& key, const QString& val)
{
	storage().setValue(section+key, val);
}

QString Setting::get(const QString& section, const QString& key, const QString& def)
{
	return storage().value(section+key, def).toString();
}

void Setting::save(const QString& section, const QString& prefix, const QComboBox& cmb, bool all)
{
	QSettings& store = storage();

	store.beginGroup(section);

	QString tkey = prefix + SET_PFX_CMBTXT;

	QString tval = cmb.currentText().trimmed();
	if (!tval.isEmpty())
		store.setValue(tkey, tval);

	if (all)
	{
		QStringList keys, vals;

		keys = store.childKeys();
		qint32 n = keys.size();
		if (n > 0)
		{
			keys.sort();

			while (n--)
			{
				QString k = keys[n];
				if ((k!=tkey) && k.startsWith(prefix))
				{
					QString v = store.value(k).toString().trimmed();
					if (!v.isEmpty() && (-1 == cmb.findText(v)))
						vals.prepend(v);

					store.remove(k);
				}
			}
		}

		n = cmb.count();
		if (n > SET_MAX_CMBITM)
			n = SET_MAX_CMBITM;

		qint32 i = 0;
		for (i=0; i<n; ++i)
			store.setValue(prefix+QString::number(i), cmb.itemText(i));

		n = (vals.count() > SET_MAX_CMBITM) ? SET_MAX_CMBITM : vals.count();
		for (qint32 j=0; i<n; ++i,++j)
			store.setValue(prefix+QString::number(i), vals[j]);
	}

	store.endGroup();
}

void Setting::load(const QString& section, const QString& prefix, QComboBox& cmb, bool all)
{
	cmb.clear();

	QSettings& store = storage();

	QStringList keys, vals;

	store.beginGroup(section);

	keys = store.childKeys();
	qint32 n = keys.size();
	if (n > 0)
	{
		QString tval;
		QString tkey = prefix + SET_PFX_CMBTXT;

		keys.sort();

		while (n--)
		{
			QString k = keys[n];
			if (k.startsWith(prefix))
			{
				QString v = store.value(k).toString().trimmed();
				if (k == tkey)
					tval = v;
				else if (all && !v.isEmpty())
					vals.append(v);
			}
		}

		vals.removeDuplicates();

		n = vals.count();
		if (n > 0)
		{
			if (n > SET_MAX_CMBITM)
				n = SET_MAX_CMBITM;

			while (n--)
				cmb.addItem(vals[n]);
		}

		if (!tval.isEmpty())
			cmb.setEditText(tval);
	}

	store.endGroup();
}


//---------add by davidWu 2013/12/31---------
void Setting::save()
{
    QSettings& store = storage();
    store.beginWriteArray("WhiteIPList");
    store.remove("");
    for(int i = 0; i < TransferSkt::WhiteIPList.size(); ++i)
    {
        store.setArrayIndex(i);
        store.setValue("IPAddress", TransferSkt::WhiteIPList[i]);
    }
    store.endArray();

    store.beginWriteArray("TransferMap");
    store.remove("");
    for(int i = 0; i < TransferSkt::TransferMap.size(); ++i)
    {
        store.setArrayIndex(i);
        store.setValue("srcAddress", TransferSkt::TransferMap[i].srcHost.IPAddress);
        store.setValue("srcPort", TransferSkt::TransferMap[i].srcHost.Port);
        store.setValue("dstAddress", TransferSkt::TransferMap[i].dstHost.IPAddress);
        store.setValue("dstPort", TransferSkt::TransferMap[i].dstHost.Port);
    }
    store.endArray();
}

void Setting::load()
{
    QSettings& store = storage();
    int size = store.beginReadArray("WhiteIPList");
    TransferSkt::WhiteIPList.clear();
    for(int i = 0; i < size; ++i)
    {
        store.setArrayIndex(i);
        QString item = store.value("IPAddress").toString();
        TransferSkt::WhiteIPList.push_back(item);
    }
    store.endArray();

    size = store.beginReadArray("TransferMap");
    TransferSkt::TransferMap.clear();
    for(int i = 0; i < size; ++i)
    {
        store.setArrayIndex(i);
        TransferInfo hostInfo;
        hostInfo.srcHost.IPAddress = store.value("srcAddress").toString();
        hostInfo.srcHost.Port = store.value("srcPort").toUInt();
        hostInfo.dstHost.IPAddress = store.value("dstAddress").toString();
        hostInfo.dstHost.Port = store.value("dstPort").toUInt();
        TransferSkt::TransferMap.push_back(hostInfo);
    }
    store.endArray();;
}
//---------end---------
