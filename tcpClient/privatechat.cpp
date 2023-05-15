#include "privatechat.h"
#include "ui_privatechat.h"
#include "tcpclient.h"
#include "protocol.h"
#include <QMessageBox>

PrivateChat::PrivateChat(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::PrivateChat)
{
  ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
  delete ui;
}

void PrivateChat::setChatName(const char *strName)
{
  m_strRecName=strName;
  m_strSendName=TcpClient::getInstance().getStrLoginName();
}

void PrivateChat::updateMsg(const PDU *pdu)
{
  if(pdu==NULL){
      return;
    }
  char caSendName[32]={'\0'};
  memcpy(caSendName,(pdu->caData)+32,32);
  QString strMsg=QString("%1 says:%2").arg(caSendName).arg((char*)(pdu->caMsg));
  ui->Chat_TE->append(strMsg);

}

PrivateChat &PrivateChat::getInstance()
{
  static PrivateChat instance;
  return instance;
}

void PrivateChat::on_SendMsgPB_clicked()
{
  QString strMsg=ui->SendMsg_LE->text();
  ui->SendMsg_LE->clear();
  if(!strMsg.isEmpty()){
      PDU *pdu=mkPDU(strMsg.size()+1);
      pdu->uiMsgType=ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;

      memcpy(pdu->caData+32,m_strSendName.toStdString().c_str(),m_strSendName.size());
      memcpy(pdu->caData,m_strRecName.toStdString().c_str(),m_strRecName.size());

      strcpy((char*)(pdu->caMsg),strMsg.toStdString().c_str());

      TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
      free(pdu);
      pdu=NULL;

    }
}
