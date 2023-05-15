#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "online.h"
#include "privatechat.h"
#include "protocol.h"


class Friend : public QWidget
{
  Q_OBJECT
public:
  explicit Friend(QWidget *parent = nullptr);
  void showAllOnlineUsr(PDU*);
  void updateFriendList(PDU*);
  void updateGroupMsg(PDU*);

  QString m_strSeacheName;
signals:

public slots:
  void showOnline();

  void searchUsr();
  void flushFriend();
  void delFriend();
  void privateChat();
  void groupChat();

private:
  QTextEdit *m_pShowMsgTE;
  QListWidget *m_pFriendListWidget;
  QLineEdit *m_pInputMsgLE;

  QPushButton *m_pDelFriendPB;
  QPushButton *m_pFlushFriendPB;
  QPushButton *m_pShowOnlineUsrPB;
  QPushButton *m_pSearchUsrPB;
  QPushButton *m_pMsgGroupChatPB;
  QPushButton *m_PrivateChatPB;

  Online *m_pOnline;
  PrivateChat *m_pPrivateChat;

};

#endif // FRIEND_H
