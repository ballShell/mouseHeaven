#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include "friend.h"
#include "book.h"

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>

namespace ui{
  class OpeWidget;
}

class OpeWidget : public QWidget
{
  Q_OBJECT
public:
  explicit OpeWidget(QWidget *parent = nullptr);
  static OpeWidget &getInstance();
  Friend* getFriend();
signals:

public slots:

private:
  QListWidget *m_pListW;
  Friend *m_pFriend;
  Book *m_pBook;

  QStackedWidget *m_pSW;
};

#endif // OPEWIDGET_H
