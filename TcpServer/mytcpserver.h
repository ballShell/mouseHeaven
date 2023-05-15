#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include<QTcpServer>
#include<QList>
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();
    ~MyTcpServer();
    static MyTcpServer &getInstance();
    void incomingConnection(qintptr socketDescriptor);
    void send(const char *pername, PDU *pdu);

    QString getFileRootPath() const;

private:
    QList<MyTcpSocket*> M_tcpSocketList;
    QString fileRootPath=QString("D:\\software\\qt\\projects\\TcpServer\\usersFiles");
public slots:
    void deleteSocket(MyTcpSocket *socket);
};

#endif // MYTCPSERVER_H
