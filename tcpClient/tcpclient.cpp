#include "tcpclient.h"
#include "ui_tcpclient.h"
#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>
#include "protocol.h"
#include "privatechat.h"


TcpClient::TcpClient(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::TcpClient)
{
  ui->setupUi(this);
  ui->name_le->setFocus();
  ui->name_le->setTabOrder(ui->name_le,ui->pwd_le);
  ui->pwd_le->setTabOrder(ui->pwd_le,ui->login_pb);
  ui->login_pb->setDefault(true);
  ui->name_le->setAttribute(Qt::WA_InputMethodEnabled, false);

  loadConfig();

  connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
  connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));

  m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);  //连接服务器
}

TcpClient::~TcpClient()
{
  delete ui;
}
void TcpClient::loadConfig(){
  QFile file(":/client.config");
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

TcpClient &TcpClient::getInstance()
{
  static TcpClient instance;
  return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
  return m_tcpSocket;
}
void TcpClient::showConnect(){
  //QMessageBox::information(this,"连接服务器","连接服务器成功");
  qDebug()<<"连接服务器成功";
}

void TcpClient::recvMsg()
{
  uint uiPDULen=0;
  m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));   //读取总长度，PDU消息单元的前4个字节，即uiPDULen
  uint uiMsgLen=uiPDULen-sizeof(PDU);  //实际消息长度,包含消息类型MsgType、文件名等
  PDU *pdu=mkPDU(uiMsgLen);
  //PDU *resPdu=mkPDU(0);
  m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(int));
  qDebug()<<"消息类型"<<pdu->uiMsgType<<endl<<"消息caData："<<pdu->caData;
  switch (pdu->uiMsgType){   //处理回复信息
    case ENUM_MSG_TYPE_REGIST_RESPOND:{
        if(0==strcmp(pdu->caData,REGIST_OK)){
            QMessageBox::information(this,"注册","注册成功");

          }else if(0==strcmp(pdu->caData,REGIST_FAIL)){
            QMessageBox::warning(this,"注册",REGIST_FAIL);
          }
        break;
      }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:{
        if(0==strcmp(pdu->caData,LOGIN_OK)){
            QMessageBox::information(this,"登录","登录成功");
            m_pCurrentPath=QString("");
            OpeWidget::getInstance().show();
            this->hide();
          }else if(0==strcmp(pdu->caData,LOGIN_FAIL)){
            QMessageBox::warning(this,"登录",LOGIN_FAIL);
          }
        break;
      }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:{

        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);

        break;
      }
    case ENUM_MSG_TYPE_SEARCH_USER_RESPOND:{

        if(0==strcmp(SEARCH_USR_NO,pdu->caData)){
            QMessageBox::information(this,"搜索",QString("%1:no exist").arg(OpeWidget::getInstance().getFriend()->m_strSeacheName));
          }else if(0==strcmp(SEARCH_USR_ONLINE,pdu->caData)){
            QMessageBox::information(this,"搜索",QString("%1:online").arg(OpeWidget::getInstance().getFriend()->m_strSeacheName));
          }else if(0==strcmp(SEARCH_USR_OFFLINE,pdu->caData)){
            QMessageBox::information(this,"搜索",QString("%1:offline").arg(OpeWidget::getInstance().getFriend()->m_strSeacheName));
          }

        break;
      }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{  //pdu='接收方'+'请求方'

        char caName[32]={'\0'};
        strncpy(caName,pdu->caData+32,32);  //请求方名字
        qDebug()<<"来自"<<caName<<"添加好友请求";
        int ret=QMessageBox::information(this,
                                         "好友请求",
                                         QString("%1 want to add you as friend").arg(caName),
                                         QMessageBox::Yes,
                                         QMessageBox::No);
        PDU *resPdu=mkPDU(0);
        memcpy(resPdu->caData,pdu->caData+32,32);  //拷贝接收方为请求方
        memcpy(resPdu->caData+32,pdu->caData,32);
        if(ret==QMessageBox::Yes){
            resPdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGGRE;
            //qDebug()<<"yes";
          }
        else{
            resPdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
            //qDebug()<<"No";
          }
        m_tcpSocket.write((char*)resPdu,resPdu->uiPDULen);
        free(resPdu);
        resPdu=NULL;
        break;
      }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        QMessageBox::information(this,"添加好友",pdu->caData);
        break;
      }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGRE:{
        QMessageBox::information(this,"添加好友","对方已同意你的好友请求");
        break;
      }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:{
        QMessageBox::information(this,"添加好友","对方已拒绝你的好友请求");
        break;
      }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
        break;
      }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:{
        QMessageBox::information(this,"删除好友","删除好友成功");
        break;
      }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:{
        char caName[32]={'\0'};
        memcpy(caName+32,pdu->caData,32);
        QMessageBox::information(this,"删除好友",QString("你已被%1移除好友").arg(caName));

        break;
      }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
        if(PrivateChat::getInstance().isHidden()){
            char caSendName[32]={'\0'};
            memcpy(caSendName,pdu->caData+32,32);
            QString strSendName=caSendName;
            PrivateChat::getInstance().setChatName(caSendName);
            PrivateChat::getInstance().show();
          }
        PrivateChat::getInstance().updateMsg(pdu);
        break;
      }
    case ENUM_MSG_TYPE_PUBLIC_CHAT_REQUEST:{
        OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);
        break;
      }
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:{
        QMessageBox::information(this,"创建文件夹",QString(pdu->caData));
        break;
      }
    default:{
        qDebug()<<"UNKNOWN";
        break;
      }
    }
  free(pdu);
  pdu=NULL;
}

#if 0
void TcpClient::on_send_clicked()
{
  QString strMsg=ui->lineEdit->text();

  if(!strMsg.isEmpty()){
      PDU *pdu=mkPDU(strMsg.size());
      pdu->uiMsgType=8888;  //test
      memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
      m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
      free(pdu);
      pdu=nullptr;
    }else{
      QMessageBox::warning(this,"发送","发送的信息不能为空");
    }
}
#endif//0

void TcpClient::on_login_pb_clicked()
{
  QString strName=ui->name_le->text();
  m_StrLoginName=strName;
  QString strPwd=ui->pwd_le->text();
  if(!strName.isEmpty()&&!strPwd.isEmpty()){
      PDU *pdu=mkPDU(0);
      pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
      strncpy(pdu->caData,strName.toStdString().c_str(),32);   //填充用户名
      strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32); //填充写密码
      m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
      free(pdu);
      pdu=NULL;
    }else{
      QMessageBox::critical(this,"登录","登录失败：用户名或密码为空");
    }
}

void TcpClient::on_regist_pb_clicked()
{
  QString strName=ui->name_le->text();
  QString strPwd=ui->pwd_le->text();
  if(!strName.isEmpty()&&!strPwd.isEmpty()){
      PDU *pdu=mkPDU(0);
      pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
      strncpy(pdu->caData,strName.toStdString().c_str(),32);   //填充用户名
      strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32); //填充写密码
      m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
      free(pdu);
      pdu=NULL;
    }else{
      QMessageBox::critical(this,"注册","注册失败：用户名或密码为空");
    }
}

void TcpClient::on_cancel_pb_clicked()
{

}

QString TcpClient::getPCurrentPath() const
{
  return m_pCurrentPath;
}

QString TcpClient::getStrLoginName() const
{
  return m_StrLoginName;
}
