#include "mytcpserver.h"
#include <QtDebug>


MyTcpServer::MyTcpServer()
{

}

MyTcpServer::~MyTcpServer()
{
  OpeDB::getInstance().allOffLine();
}
MyTcpServer &MyTcpServer::getInstance(){
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"new client connected";
    MyTcpSocket *pTcpSocket=new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    M_tcpSocketList.append(pTcpSocket);

    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),
            this,SLOT(deleteSocket(MyTcpSocket*)));
}

void MyTcpServer::send(const char *pername, PDU *pdu)
{
  if(pername==NULL||pdu==NULL){
      return;
    }
  QString strName=pername;
  for(int i=0;i<M_tcpSocketList.size();i++){   //找到正确的接收方
      if(strName==M_tcpSocketList.at(i)->getName()){
          M_tcpSocketList.at(i)->write((char*)pdu,pdu->uiPDULen);
          qDebug()<<"send:"<<"发送成功";
          break;
        }
    }

}

QString MyTcpServer::getFileRootPath() const
{
  return fileRootPath;
}

void MyTcpServer::deleteSocket(MyTcpSocket *socket)
{
  if (!socket) {
        return;
    }

    QList<MyTcpSocket*>::Iterator iter = M_tcpSocketList.begin();
    while (iter != M_tcpSocketList.end()) {   //***存在程序异常退出问题***
        if (*iter == socket) {
            (*iter)->deleteLater();
            *iter = NULL;
            iter = M_tcpSocketList.erase(iter); // 返回下一个有效迭代器
            break;
        } else {
            ++iter;
        }
    }
    if (M_tcpSocketList.isEmpty()) {
        qDebug() << "没有剩余的服务器";
    } else {
        //qDebug() << "剩余服务器：";
        for (int i=0; i<M_tcpSocketList.size(); i++) {
            MyTcpSocket *s = M_tcpSocketList.at(i);
            if (s) {
                qDebug() << s->getName();
            }
        }
    }
   // qDebug() << "执行无误";
}

