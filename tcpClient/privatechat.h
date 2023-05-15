#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>

#include "protocol.h"

namespace Ui {
  class PrivateChat;
}

class PrivateChat : public QWidget
{
  Q_OBJECT

public:
  explicit PrivateChat(QWidget *parent = nullptr);
  ~PrivateChat();
  void setChatName(const char*);
  void updateMsg(const PDU *pdu);
  static PrivateChat &getInstance();
private slots:
  void on_SendMsgPB_clicked();

private:
  Ui::PrivateChat *ui;
  QString m_strRecName;
  QString m_strSendName;
};

#endif // PRIVATECHAT_H
