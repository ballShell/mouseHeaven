#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include<QFile>
#include<QTcpSocket>

#include "opewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    void loadConfig();

    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();

    QString getStrLoginName() const;

    QString getPCurrentPath() const;

public slots:
    void showConnect();
    void recvMsg();

private slots:
 //   void on_send_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    QTcpSocket m_tcpSocket;  //连接服务器
    QString m_StrLoginName;

    QString m_pCurrentPath;

};
#endif // TCPCLIENT_H
