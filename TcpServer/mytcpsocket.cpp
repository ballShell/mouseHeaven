#include "mytcpsocket.h"
#include<QDebug>
#include "protocol.h"
#include "mytcpserver.h"

#include <QDir>
MyTcpSocket::MyTcpSocket()
{
  connect(this,SIGNAL(readyRead()),
          this,SLOT(recvMsg()));
  connect(this,SIGNAL(disconnected()),
          this,SLOT(clientOffline()));
}

QString MyTcpSocket::getName()
{
  return this->m_strName;
}

void MyTcpSocket::recvMsg()
{
  uint uiPDULen=0;
  this->read((char*)&uiPDULen,sizeof(uint));   //读取总长度，PDU消息单元的前4个字节，即uiPDULen
  uint uiMsgLen=uiPDULen-sizeof(PDU);  //实际消息长度,包含消息类型MsgType、文件名等
  PDU *pdu=mkPDU(uiMsgLen);
  PDU *resPdu=NULL;
  bool flag=1;

  this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(int));
  switch (pdu->uiMsgType){   //处理回复信息
    case ENUM_MSG_TYPE_REGIST_REQUEST:{
        resPdu=mkPDU(0);
        resPdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret=OpeDB::getInstance().handleRegist(caName,caPwd);
        qDebug()<<caName<<caPwd;
        if(ret){
            strcpy(resPdu->caData,REGIST_OK);
            const QString usrRootPath=MyTcpServer::getInstance().getFileRootPath();
            QDir dir(usrRootPath);
            dir.mkdir(QString("./%1").arg(caName));
          //  qDebug()<<dir.mkdir(QString("./%1").arg(caName));
          }else{
            strcpy(resPdu->caData,REGIST_FAIL);
          }
        break;
      }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:{
        resPdu=mkPDU(0);
        resPdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret=OpeDB::getInstance().handleLogin(caName,caPwd);
        if(ret){
            strcpy(resPdu->caData,LOGIN_OK);
            m_strName=caName;
          }else{
            strcpy(resPdu->caData,LOGIN_FAIL);
          }
        break;
      }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:{
        QStringList ret=OpeDB::getInstance().handleAllOnline();
        uint uiMsgLen=ret.size()*32;

        resPdu=mkPDU(uiMsgLen);
        resPdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        for(int i=0;i<ret.size();i++){
            memcpy((char*)(resPdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
          }
        break;
      }
    case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:{
        int ret=OpeDB::getInstance().handleSearch(pdu->caData);
        resPdu=mkPDU(0);
        resPdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
        if(ret==-1){
            strcpy(resPdu->caData,SEARCH_USR_NO);
          }else if(1==ret){
            strcpy(resPdu->caData,SEARCH_USR_ONLINE);
          }else{
            strcpy(resPdu->caData,SEARCH_USR_OFFLINE);
          }
        break;
      }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        char caPerName[32]={'\0'};
        char caName[32]={'\0'};
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        int ret=OpeDB::getInstance().handleAddFriendCheck(caPerName,caName);

        resPdu=mkPDU(0);
        resPdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        if(ret==-1){
            strcpy(resPdu->caData,UNKNOWERRO);
          }else if(ret==0){
            strcpy(resPdu->caData,EXISTED_FRIEND);
          }else if(ret==1){
            //strcpy(resPdu->caData,REQUEST_SENT);
            qDebug()<<"服务器：添加好友任务处理："<<caName<<"添加"<<caPerName<<"的请求";
            MyTcpServer::getInstance().send(caPerName,pdu);
          }else if(ret==2){
            strcpy(resPdu->caData,ADD_FRIEND_OFFLINE);
          }
        break;
      }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGRE:{
        char rec[32]={'\0'};
        char sender[32]={'\0'};
        strncpy(rec,pdu->caData,32);
        strncpy(sender,(pdu->caData)+32,32);
        OpeDB::getInstance().handledAddFriend(rec,sender);
        MyTcpServer::getInstance().send(rec,pdu);
        flag=0;
        break;
      }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:{
        char rec[32]={'\0'};
        char sender[32]={'\0'};
        strncpy(rec,pdu->caData,32);
        strncpy(sender,pdu->caData+32,32);
        MyTcpServer::getInstance().send(rec,pdu);
        flag=0;
        break;
      }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        QStringList friendList = OpeDB::getInstance().handleFlushFriend(caName);
        int uiMsgLen=friendList.size()*32;
        resPdu=mkPDU(uiMsgLen);
        resPdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
        for(int i=0;i<friendList.size();i++){
            memcpy((char*)(resPdu->caMsg)+i*32,
                   friendList.at(i).toStdString().c_str(),friendList.at(i).size());
          }
        break;
      }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:{
        char caPerName[32]={'\0'};
        char caName[32]={'\0'};
        strncpy(caPerName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        OpeDB::getInstance().handleDelFriend(caName,caPerName);

        resPdu=mkPDU(0);
        resPdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
        strcpy(resPdu->caData,DELETE_FRIEND_OK);

        MyTcpServer::getInstance().send(caPerName,pdu);  //发送给被删好友的提示

        break;
      }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
        char caPerName[32]={'\0'};
        memcpy(caPerName,pdu->caData,32);
        qDebug()<<caPerName;
        MyTcpServer::getInstance().send(caPerName,pdu);
        flag=0;
        break;
      }
    case ENUM_MSG_TYPE_PUBLIC_CHAT_REQUEST:{
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        qDebug()<<"ENUM_MSG_TYPE_PUBLIC_CHAT_REQUEST";
        QStringList OnlineFriendList=OpeDB::getInstance().handleAllFriendOnline(caName);
        for(int i=0;i<OnlineFriendList.size();i++){
            MyTcpServer::getInstance().send(OnlineFriendList.at(i).toStdString().c_str(),pdu);
          }
        flag=0;
        break;
      }
    case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:{
        QString FileRootPath=MyTcpServer::getInstance().getFileRootPath();
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        QString curUsrRootPath=FileRootPath+"\\"+caName;
        qDebug()<<"revMsg::curUsrRootPath"<<curUsrRootPath;
        QDir dir(curUsrRootPath);   //根地址初始化文件夹，保证其作用范围仅限于本用户的所属文件

        QString strCurPath=QString("./%1").arg((char*)(pdu->caMsg));   //用户当前路径
        qDebug()<<"revMsg::strCurPath"<<strCurPath;

        bool ret=dir.exists(strCurPath);
        qDebug()<<"revMsg::dir.exists:"<<ret;

        resPdu=mkPDU(0);
        resPdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;

        if(ret){  //存在
            char caNewDir[32]={'\0'};
            memcpy(caNewDir,pdu->caData+32,32);
            QString strNewPath=strCurPath+caNewDir;
            ret=dir.exists(strNewPath);
            if(ret){  //想创建的文件夹名已存在
                strcpy(resPdu->caData,FILENAME_OR_DIR_EXIST);
              }else{   //想创建的文件夹名不存在
                dir.mkdir(strNewPath);
                strcpy(resPdu->caData,CREATE_DIR_OK);
              }
          }else{  //不存在
            strcpy(resPdu->caData,DIR_NOT_EXISTS);
          }
        break;

      }
    default:{
        break;
      }
    }
  //发送
  if(flag){
      qDebug()<<"revMsg::flag:resPdu开始发送";
       write((char*)resPdu,resPdu->uiPDULen);
       free(resPdu);
    }
  free(pdu);
  pdu=NULL;

  resPdu=NULL;
}

void MyTcpSocket::clientOffline()
{
  OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());  //数据内用户状态更新
  emit offline(this);

}

