#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "protocol.h"

namespace ui {
  class Book;
}

class Book : public QWidget
{
  Q_OBJECT
public:
  explicit Book(QWidget *parent = nullptr);

public slots:

  void createDir();

private:
  QListWidget *m_pBookListW;
  QPushButton *m_pReturnPB;
  QPushButton *m_pCreateDirPB;
  QPushButton *m_pDelDirPB;
  QPushButton *m_pRenamePB;
  QPushButton *m_pFlushFilePB;
  QPushButton *m_pUploadPB;
  QPushButton *m_pDownloadPB;
  QPushButton *m_pDelFilePB;
  QPushButton *m_pSharePB;
};
#endif // BOOK_H
