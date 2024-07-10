#include "MySQLDB.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QDebug>
#include "func.h"

#pragma execution_character_set("utf-8")

MySQLDB::MySQLDB()
{
}

// ��ȡ�������ݿ��б�
QStringList MySQLDB::getDBList()
{
    qDebug() << "avaliable db drivers:";
    QStringList drivers = QSqlDatabase::drivers();
    foreach(QString driver, drivers)
    {
        qDebug() << drivers;
    }

    return drivers;
}

// �������ݿ�������Ϣ
void MySQLDB::setDBInfo(const QString & host, const quint16 port, const QString & username, const QString & pwd, const QString & dbname)
{
	host_ = host;
	port_ = port;
	username_ = username;
	password_ = pwd;
	dbname_ = dbname;
}

MySQLDB::~MySQLDB()
{
}

// ����MySQL���ݿ�(ͨ��ODBC��ʽ����)
// https://blog.csdn.net/joey_ro/article/details/105411135
bool MySQLDB::connectDB()
{
    //QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db_ = QSqlDatabase::addDatabase("QODBC3");
    db_.setHostName(host_);
    db_.setPort(port_);
    db_.setDatabaseName(dbname_);
    db_.setUserName(username_);
    db_.setPassword(password_);
    bool bConnected = db_.open();
	if (bConnected)
	{
        qDebug() << "connect to mysql db successful!";
		return true;
	}
	else {
        qDebug() << "connect to mysql db falied!" << db_.lastError().text();
		return false;
	}
}

// ��ѯ���ݿ��м�����ݱ� monitordata
bool MySQLDB::queryMonitorData()
{
    //��ѯ���ݿ������б������
    QStringList tables = db_.tables();
    foreach(QString table, tables)
        qDebug()<<table;

    //ODBC��ѯ����
    QSqlQuery result = db_.exec("select * from monitordata");
    while(result.next()){
        qDebug()<<"mn:"<<result.value("mn").toString();
        qDebug()<<"datatime:"<<result.value("datatime").toString();
        qDebug()<<"paramCode:"<<result.value("paramCode").toString();
        qDebug()<<"val:"<<result.value("val").toDouble()<<endl;
        qDebug()<<"mark:"<<result.value("mark").toString()<<endl;
    }

	return true;
}

// �������ݼ�¼��������ݱ� monitordata
bool MySQLDB::insertMonitorData(const MonitorData &record)
{
    // ODBC��������
	String strQNTime = Math::Date::convertfmt(record.qnTime.substr(0, 14).c_str(), "%04d%02d%02d%02d%02d%02d", "%04d-%02d-%02d %02d:%02d:%02d");
	String strDataTime = Math::Date::convertfmt(record.datatime.c_str(), "%04d%02d%02d%02d%02d%02d", "%04d-%02d-%02d %02d:%02d:%02d");
    QString strSql = QString("insert into monitordata(mn,qntime,datatime,paramCode,val,mark) values('%1','%2','%3', '%4', %5,'%6')")
            .arg(QString::fromStdString(record.mn))
			.arg(QString::fromStdString(strQNTime))
            .arg(QString::fromStdString(strDataTime))
            .arg(QString::fromStdString(record.paramCode))
            .arg(record.val)
            .arg(QString::fromStdString(record.mark));   
	QSqlQuery query(db_);
	if (query.exec(strSql))
	{
		qDebug() << "�������ݳɹ���";
		return true;
	}
	else
	{
		qDebug() << "��������ʧ�ܣ�";
		return false;
	}
}
