#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include<QTcpSocket>
#include "protocol.h"
#include "opedb.h"



class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
private:
    QString m_strName;
public slots:
    void recvMsg();
    void clientOffline();
signals:
       void offline(MyTcpSocket*);
};

#endif // MYTCPSOCKET_H
