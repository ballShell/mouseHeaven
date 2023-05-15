#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"
#include<QFile>
#include<QHostAddress>
#include<QDebug>
#include<QMessageBox>


TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();

    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}


void TcpServer::loadConfig(){
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData=baData.toStdString().c_str();
 //       qDebug()<<strData;
        file.close();

        strData.replace("\r\n"," ");
 //       qDebug()<<strData;
        QStringList strList = strData.split(" ");
//        for(int i=0;i<strList.size();i++){
//            qDebug()<<strList[i];
//        }
        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUInt();
        qDebug()<<"IP:"<<m_strIP<<"\t"<<"Port:"<<m_usPort;

    }else{
        QMessageBox::critical(this,"OPEN CONFIG","open config fail");
    }
}
