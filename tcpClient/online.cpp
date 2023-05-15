#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

Online::Online(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Online)
{
  ui->setupUi(this);
}

Online::~Online()
{
  delete ui;
}

void Online::ShowAllUsr(PDU *pdu)
{
  ui->online_lw->clear();
  if(pdu==NULL){
      return;
    }
  uint uiSize=pdu->uiMsgLen/32;
  char tmp[32];
  for(uint i=0;i<uiSize;i++){
      memcpy(tmp,(char*)(pdu->caMsg)+i*32,32);
      ui->online_lw->addItem(tmp);
    }
}



void Online::on_addFriend_pb_clicked()
{
  QListWidgetItem *pItem =ui->online_lw->currentItem();
  QString StrPerUsrName=pItem->text();
  QString strLoginName=TcpClient::getInstance().getStrLoginName();
  PDU *pdu=mkPDU(0);
  pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
  memcpy(pdu->caData,StrPerUsrName.toStdString().c_str(),32);  //请求接收方
  memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),32);  //请求发送方
  TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
  free(pdu);
  pdu=NULL;
}
