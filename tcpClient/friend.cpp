#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include "privatechat.h"
#include <QInputDialog>

Friend::Friend(QWidget *parent) : QWidget(parent)
{

  m_pDelFriendPB=new QPushButton("删除好友");
  m_pFlushFriendPB=new QPushButton("刷新好友");
  m_pShowOnlineUsrPB=new QPushButton("显示在线用户");
  m_pSearchUsrPB=new QPushButton("搜索用户");
  m_pMsgGroupChatPB=new QPushButton("发送信息");
  m_PrivateChatPB=new QPushButton("私聊");

  QVBoxLayout *pRightPBVBL=new QVBoxLayout;
  pRightPBVBL->addWidget(m_pDelFriendPB);
  pRightPBVBL->addWidget(m_pFlushFriendPB);
  pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
  pRightPBVBL->addWidget(m_pSearchUsrPB);
  pRightPBVBL->addWidget(m_PrivateChatPB);


  m_pShowMsgTE=new QTextEdit;
  m_pFriendListWidget=new QListWidget ;
  m_pInputMsgLE=new QLineEdit;


  QHBoxLayout *pTopHBL=new QHBoxLayout;
  pTopHBL->addWidget(m_pShowMsgTE);
  pTopHBL->addWidget(m_pFriendListWidget);
  pTopHBL->addLayout(pRightPBVBL);

  QHBoxLayout *pMsgHBL=new QHBoxLayout;
  pMsgHBL->addWidget(m_pInputMsgLE);
  pMsgHBL->addWidget(m_pMsgGroupChatPB);

  m_pOnline=new Online;

  QVBoxLayout *pMain=new QVBoxLayout;
  pMain->addLayout(pTopHBL);
  pMain->addLayout(pMsgHBL);
  pMain->addWidget(m_pOnline);
  m_pOnline->hide();

  setLayout(pMain);

  m_pShowMsgTE->setReadOnly(true);

  connect(m_pShowOnlineUsrPB,SIGNAL(clicked()),
          this,SLOT(showOnline()));
  connect(m_pSearchUsrPB,SIGNAL(clicked()),
          this,SLOT(searchUsr()));

  connect(m_pFlushFriendPB,SIGNAL(clicked()),
          this,SLOT(flushFriend()));

  connect(m_pDelFriendPB,SIGNAL(clicked()),
          this,SLOT(delFriend()));

  connect(m_PrivateChatPB,SIGNAL(clicked()),
          this,SLOT(privateChat()));

  connect(m_pMsgGroupChatPB,SIGNAL(clicked()),
          this,SLOT(groupChat()));
}
void Friend::showAllOnlineUsr(PDU *pdu)
{
  if(pdu==NULL){
      return;
    }
  m_pOnline->ShowAllUsr(pdu);

}

void Friend::updateFriendList(PDU *pdu)
{
  m_pFriendListWidget->clear();
  if(pdu==NULL){
      return;
    }
  uint uiSize=pdu->uiMsgLen/32;
  char caName[32]={'\0'};
  for(uint i=0;i<uiSize;i++){
      memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
      m_pFriendListWidget->addItem(caName);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
  QString strMsg=QString("%1 says %2").arg(pdu->caData).arg((char*)(pdu->caMsg));
  m_pShowMsgTE->append(strMsg);
}

void Friend::delFriend()
{
  if(m_pFriendListWidget->currentItem()==NULL) return;
  QString strFriendName=m_pFriendListWidget->currentItem()->text();
  qDebug()<<"Friend::delFriend:将要删除的好友为:"<<strFriendName;
  PDU* pdu=mkPDU(0);
  pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;

  QString strSelfName=TcpClient::getInstance().getStrLoginName();
  memcpy(pdu->caData,strFriendName.toStdString().c_str(),strFriendName.size());
  memcpy(pdu->caData+32,strSelfName.toStdString().c_str(),strSelfName.size());
  TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);

}

void Friend::privateChat()
{
  qDebug()<<"Friend::privateChat:"<<"clicked";
  if(m_pFriendListWidget->currentItem()!=NULL){
      QString strChatName=m_pFriendListWidget->currentItem()->text();
      PrivateChat::getInstance().setChatName(strChatName.toStdString().c_str());
      if (PrivateChat::getInstance().isHidden()){
          PrivateChat::getInstance().show();
        }
    }else{
      //未选中正确的好友的处理
    }
}

void Friend::groupChat()
{
  QString strMsg=m_pInputMsgLE->text();
  m_pInputMsgLE->clear();
  qDebug()<<"群聊消息为:"<<strMsg;
  if(!strMsg.isEmpty()){
      PDU* pdu=mkPDU(strMsg.size()+1);
      pdu->uiMsgType=ENUM_MSG_TYPE_PUBLIC_CHAT_REQUEST;
      QString strName=TcpClient::getInstance().getStrLoginName();
      strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
      strncpy((char*)(pdu->caMsg),strMsg.toStdString().c_str(),strMsg.size());
      TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
      m_pShowMsgTE->append(QString("You say:%1").arg(strMsg));
    }
}

void Friend::showOnline()
{
  if(m_pOnline->isHidden()){
      m_pOnline->show();

      PDU* pdu=mkPDU(0);
      pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
      TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
      free(pdu);
      pdu=NULL;
    }else{
      m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
  m_strSeacheName=QInputDialog::getText(this,"搜索","用户名");
  if(!m_strSeacheName.isEmpty()){
      qDebug()<<m_strSeacheName;
      PDU *pdu=mkPDU(0);
      pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
      memcpy(pdu->caData,m_strSeacheName.toStdString().c_str(),m_strSeacheName.size());

      TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
      free(pdu);
      pdu=NULL;
    }
}

void Friend::flushFriend()
{
  QString strName =TcpClient::getInstance().getStrLoginName();
  PDU* pdu=mkPDU(0);
  pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
  memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
  TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
  free(pdu);
  pdu=NULL;
}
