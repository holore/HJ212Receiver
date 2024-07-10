#pragma once
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include "pub.h"
#include <QStringList>


class MySQLDB
{
public:
	MySQLDB();

    // ��ȡ�������ݿ��б�
    QStringList getDBList();

	// �������ݿ�������Ϣ
	void setDBInfo(const QString &host, const quint16 port, const QString &username, const QString &pwd, const QString &dbname);

	// ����MySQL���ݿ�
	bool connectDB();

	// ��ѯ���ݿ��м�����ݱ� monitordata
	bool queryMonitorData();

	// �������ݼ�¼��������ݱ� monitordata
	bool insertMonitorData(const MonitorData &record);

	~MySQLDB();

private:
	QString host_;
	quint16 port_;
	QString username_;
	QString password_;
	QString dbname_;

    QSqlDatabase db_;
};

