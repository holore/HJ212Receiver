#ifndef HJ212DIALOG_H
#define HJ212DIALOG_H

#include <QDialog>
#include <QTcpServer>      // �����׽���
#include <QTcpSocket>      // ͨ���׽���

#include "MySQLDB.h"

namespace Ui {
class HJ212Dialog;
}

class HJ212Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit HJ212Dialog(QWidget *parent = nullptr);
    ~HJ212Dialog();

private slots:

    void on_btnStop_clicked();

    void on_btnListen_clicked();

    //  ����ͻ��˵���������
    void handleAccept();
    // �����ͻ����ϴ��ı���
    void handleRecevice();

private:
    Ui::HJ212Dialog *ui;

    QTcpServer *tcpServer;      // �����׽���
    QTcpSocket *tcpSocket;    // ͨ���׽���

	// MySQL���ݿ⴦�������
    MySQLDB dbprocess_;
};

#endif // HJ212DIALOG_H
