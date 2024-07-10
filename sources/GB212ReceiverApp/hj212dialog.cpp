#include "hj212dialog.h"
#include "ui_hj212dialog.h"
#include "gb212.h"
#include <QDebug>
#include "pub.h"
#include "func.h"
#include <QTableWidgetItem>

#pragma execution_character_set("utf-8")

HJ212Dialog::HJ212Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HJ212Dialog)
{
    ui->setupUi(this);

    // QDialog �����󻯡���С����ť�͹رհ�ť������Ҫ������ʾ
    // https://blog.csdn.net/xueyushenzhou/article/details/51291404
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    //�����׽���   ָ���������Ŀ�ģ������Զ����տռ�
    tcpServer = new QTcpServer(this);

    // Ĭ�ϼ����˿�����Ϊ8099
    ui->lineEditPort->setText("8099");

	// �������ݿ�
    dbprocess_.setDBInfo("127.0.0.1", 3306, "root", "1030", "test_hj212_db");
    dbprocess_.connectDB();
    dbprocess_.getDBList();
}

HJ212Dialog::~HJ212Dialog()
{
    delete ui;
}


void HJ212Dialog::on_btnStop_clicked()
{
        if(nullptr == tcpSocket)
        {
            return ;
        }
        //�����Ϳͻ��˶˿ڶϿ�����
        tcpSocket->disconnectFromHost();
        tcpSocket->close();

        tcpSocket = nullptr;

        tcpServer->close();

        ui->btnListen->setChecked(true);
        ui->btnStop->setChecked(false);
}

void HJ212Dialog::on_btnListen_clicked()
{
    if (tcpServer != nullptr)
    {
        qint16 port = ui->lineEditPort->text().toInt();
        tcpServer->listen(QHostAddress::Any, port);

        setWindowTitle(tr("Server start at: %1").arg(port));

        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleAccept()));
        ui->btnListen->setChecked(false);
        ui->btnStop->setChecked(true);
    }
}

//  ����ͻ��˵���������
void HJ212Dialog::handleAccept()
{
    //ȡ�����������ӵĵ��׽���
    tcpSocket = tcpServer->nextPendingConnection();

    //��ȡ�Է���IP�Ͷ˿�
    QString ip = tcpSocket->peerAddress().toString();
    uint16  port = tcpSocket->peerPort();
    QString temp = QString("[%1:%2]:connected successful").arg(ip).arg(port);
    ui->textEditRead->setText(temp);

    /*ע�������connect��λ�ã�һ���ڽ��������Ӻ�
     * ��Ȼ�����Ұָ��Ĵ���ǰ�涨���QTcpSocket  *tcpSocket;
     * ��������ȴӹ��캯��ִ�еģ���û��ִ�е������QTcpSocket  *tcpSocketָ�롣
    */
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(handleRecevice()));
}

// �����ͻ����ϴ��ı���
void HJ212Dialog::handleRecevice()
{
    //��ͨ���׽�����ȡ������
    // ��ȡ���ݻ���������
    QByteArray buffArr = tcpSocket->readAll();
    if (!buffArr.isEmpty())
    {
        QString str = ui->textEditRead->toPlainText();
        QString strRecv = QString::fromLocal8Bit(buffArr);
        str += strRecv + "\n";
        // ˢ�½�����ʾ
        ui->textEditRead->setText(str);
        // ui->textEditRead->append(buffArr);
    }


    String strBody = buffArr.toStdString();

    // parse gb212 format data
    GB212::GB212Array arr;
    GB212::Parser(arr, strBody);
    // ʵʱ�����£�����
    for (auto& i : arr) {
        if (i.valid()) {
            // ʵʱ����, ���ûظ����� , ����
            if (i.is_data()) {
               // ��������212���ݱ���
               qDebug() << "***HJ212-2017 data parser successful!***" << "\n";
               qDebug() << "full data=" << i.full.c_str() << "\n";
               qDebug() << "header=" << i.header.c_str() << "\n";
               qDebug() << "ST=" << i.data.ST.c_str() << "\n";
               qDebug() << "CN=" << i.data.CN.c_str() << "\n";
               qDebug() << "PW=" << i.data.PW.c_str() << "\n";
               qDebug() << "MN=" << i.data.MN.c_str() << "\n";
               qDebug() << "Flag=" << i.data.Flag.c_str() << "\n";
               qDebug() << "CP=" << i.data.CP.c_str() << "\n";
               qDebug() << "QN=" << i.data.QN.c_str() << "\n";

			   // ���½����ϵ�HJ212������Ϣ����ʾ���½��յ���HJ212-2017����
			   String strQNTime = Math::Date::convertfmt(i.data.QN.substr(0,14).c_str(), "%04d%02d%02d%02d%02d%02d", "%04d��%02d��%02d�� %02dʱ%02d��%02d��");
               ui->lineEditQN->setText(QString::fromStdString(strQNTime));
			   ui->lineEditMN->setText(QString::fromStdString(i.data.MN));
			   ui->lineEditCN->setText(QString::fromStdString(i.data.CN));
			   ui->lineEditST->setText(QString::fromStdString(i.data.ST));
			   ui->lineEditFlag->setText(QString::fromStdString(i.data.Flag));
			   String strDataTime = Math::Date::convertfmt(i.data.CPs.DataTime.c_str(), "%04d%02d%02d%02d%02d%02d", "%04d��%02d��%02d�� %02dʱ%02d��%02d��");
               ui->lineEditDataTime->setText(QString::fromStdString(strDataTime));

			   
               // ����������ݲ���
               MonitorRow rw;
               for( auto& item : i.data.CPs.Value)
               {
                    if (item.first == "DataTime")
                    {
                        continue;
                    }
                    MonitorData itemData;
					itemData.mn = i.data.MN;
					itemData.qnTime = i.data.QN;
					itemData.datatime = i.data.CPs.DataTime;
					itemData.paramCode = item.first;
                    //String strParamCode = item.first;
                    //double val = 0.0f;
                    //String strMark = "N";
                    for (auto &kitem : item.second)
                    {
                        if (kitem.first == "Avg" || kitem.first == "Rtd")
                        {
                            Math::Tools::to_double(itemData.val, kitem.second);
                        } else if (kitem.first == "Flag")
                        {
                             itemData.mark = kitem.second;
                        }
                    }
                    rw.push_back(itemData);
                    // �˴����Ը�����Ҫ��rw�ļ�����ݽ�����⴦��������⵽MySQL��SqlServer��Sqlite��Qracle��Redis��MongoDB���ݿ�ȣ�
                    dbprocess_.insertMonitorData(itemData);
               }

			   // �������еļ�����ݱ�����������
			   int totalRow = rw.size();
			   int currentRow = 0;
			   ui->tableWidget->clearContents();
			   ui->tableWidget->setRowCount(totalRow);
			   ui->tableWidget->setColumnCount(4);
			   for (auto dataItem : rw)
			   {
				   String strQNTime = Math::Date::convertfmt(dataItem.qnTime.substr(0, 14).c_str(), "%04d%02d%02d%02d%02d%02d", "%04d-%02d-%02d %02d:%02d:%02d");
				   String strDataTime = Math::Date::convertfmt(dataItem.datatime.c_str(), "%04d%02d%02d%02d%02d%02d", "%04d-%02d-%02d %02d:%02d:%02d");

				   QTableWidgetItem *newQNTimeItem = new QTableWidgetItem(QString::fromStdString(strQNTime));
				   ui->tableWidget->setItem(currentRow, 0, newQNTimeItem);

				   QTableWidgetItem *newDataTimeItem = new QTableWidgetItem(QString::fromStdString(strDataTime));
				   ui->tableWidget->setItem(currentRow, 1, newDataTimeItem);

				   QTableWidgetItem *newparamCodeItem = new QTableWidgetItem(QString::fromStdString(dataItem.paramCode));
				   ui->tableWidget->setItem(currentRow, 2, newparamCodeItem);

				   QTableWidgetItem *newValItem = new QTableWidgetItem(tr("%1").arg(dataItem.val));
				   ui->tableWidget->setItem(currentRow, 3, newValItem);

				   QTableWidgetItem *newMarkItem = new QTableWidgetItem(QString::fromStdString(dataItem.mark));
				   ui->tableWidget->setItem(currentRow, 4, newMarkItem);

				   currentRow++;
			   }
            }
        }
    }
}
