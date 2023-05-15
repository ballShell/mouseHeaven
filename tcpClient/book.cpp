#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>

Book::Book(QWidget *parent) : QWidget(parent)
{
  m_pBookListW=new QListWidget;
  m_pReturnPB=new QPushButton("返回");
  m_pCreateDirPB=new QPushButton("创建文件夹");
  m_pDelDirPB=new QPushButton("删除文件夹");
  m_pRenamePB=new QPushButton("重命名文件夹");
  m_pFlushFilePB=new QPushButton("刷新文件");

  QVBoxLayout *pDirVBL=new QVBoxLayout;
  pDirVBL->addWidget(m_pReturnPB);
  pDirVBL->addWidget(m_pCreateDirPB);
  pDirVBL->addWidget(m_pDelDirPB);
  pDirVBL->addWidget(m_pRenamePB);
  pDirVBL->addWidget(m_pFlushFilePB);

  m_pUploadPB=new QPushButton("上传文件");
  m_pDownloadPB=new QPushButton("下载文件");
  m_pDelFilePB=new QPushButton("删除文件");
  m_pSharePB=new QPushButton("共享文件");

  QVBoxLayout *pFileVBL=new QVBoxLayout;
  pFileVBL->addWidget(m_pUploadPB);
  pFileVBL->addWidget(m_pDownloadPB);
  pFileVBL->addWidget(m_pDelFilePB);
  pFileVBL->addWidget(m_pSharePB);

  QHBoxLayout *pMain=new QHBoxLayout;

  pMain->addWidget(m_pBookListW);
  pMain->addLayout(pDirVBL);
  pMain->addLayout(pFileVBL);

  setLayout(pMain);

  connect(m_pCreateDirPB,SIGNAL(clicked()),
          this,SLOT(createDir()));
}

void Book::createDir()
{
  QString strNewDirName=QInputDialog::getText(this,"新建文件夹","请输入新文件夹名(不超过32个字符)");
  if(!strNewDirName.isEmpty()&&strNewDirName.size()<=32){
      QString strName=TcpClient::getInstance().getStrLoginName();
      QString strCurrentPath=TcpClient::getInstance().getPCurrentPath();
      PDU* pdu=mkPDU(strCurrentPath.size()+1);
      pdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
      strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
      strncpy(pdu->caData+32,strNewDirName.toStdString().c_str(),strNewDirName.size());
      memcpy(pdu->caMsg,strCurrentPath.toStdString().c_str(),strCurrentPath.size());

      TcpClient::getInstance().getTcpSocket().write((char*)(pdu),pdu->uiPDULen);
      free(pdu);
      pdu=NULL;

    }else{
      QMessageBox::warning(this,"新建文件夹","新建文件夹名不能空或字符数超过32");
    }

}
