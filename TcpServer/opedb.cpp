#include "opedb.h"
#include<QMessageBox>
#include<QDebug>
#include<QString>
#include<QSqlQuery>

OpeDB::OpeDB(QObject *parent) : QObject(parent)
{
  m_db=QSqlDatabase::addDatabase("QSQLITE");
  init();
}

OpeDB &OpeDB::getInstance()
{
  static OpeDB instance;
  return instance;
}

void OpeDB::init()
{
  m_db.setHostName("localhost");
  m_db.setDatabaseName("D:\\software\\qt\\projects\\tcpServer\\cloud.db");
  if(m_db.open()){
      QSqlQuery query;
      query.exec("select * from usrInfo");
      while(query.next()){
          QString data=QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
          //ui->textEdit->append(data);
          qDebug()<<data;
        }
    }else{
      QMessageBox::critical(NULL,"打开数据库","打开失败");
    }
}

OpeDB::~OpeDB()
{
  m_db.close();
}

bool OpeDB::handleRegist(const char *name, const char *pwd)
{
  if(name==NULL||pwd==NULL){
      return false;
    }
  QString order=QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
  QSqlQuery query;
  return query.exec(order);
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
  if(name==NULL||pwd==NULL){
      return false;
    }
  QString order=QString("select * from usrinfo where name=\'%1\' and pwd='\%2' and online=0").arg(name).arg(pwd);
  QSqlQuery query;
  query.exec(order);
  if(query.next()){
      order=QString("update usrinfo set online=1 where name=\'%1\'").arg(name);
      query.exec(order);
      return true;
    }else{
      return false;
    }
}

void OpeDB::handleOffline(const char *name)
{
  if(NULL==name){
      //qDebug()<<"name is null";
      return;
    }
  qDebug()<<"下线操作"<<name;
  QString order=QString("update usrinfo set online=0 where name=\'%1\'").arg(name);
  QSqlQuery query;
  query.exec(order);

}

int OpeDB::handleSearch(const char *name)
{
  if(name==NULL){
      return -1;
    }

  QString order=QString("select online from usrInfo where name=\'%1\'").arg(name);
  QSqlQuery query;
  query.exec(order);
  if(query.next()){
      int ret=query.value(0).toInt();
      if(ret==1){
          return 1;
        }else{
          return 0;
        }
    }else{
      return -1;
    }
}

int OpeDB::handleAddFriendCheck(const char *pername, const char *name)
{
  if(pername==NULL||name==NULL){
      return -1;
    }
  QString order =QString("select * from friend where (id=(select id from usrinfo where name=\'%1\') and friendid=(select id from usrinfo where name=\'%2\')) or (id=(select id from usrinfo where name=\'%3\') and friendid=(select id from usrinfo where name=\'%4\'))").arg(name).arg(pername).arg(pername).arg(name);
  qDebug()<<order;
  QSqlQuery query;
  query.exec(order);
  if(query.next()){
      return 0;  //已经是好友
    }else{
      order=QString("select online from usrInfo where name=\'%1\'").arg(pername);

      query.exec(order);
      query.next();
      int ret=query.value(0).toInt();
      qDebug()<<order<<ret;
      if(ret==1){
          return 1;   //在线
        }else{
          return 2;  //不在线
        }
    }
}

void OpeDB::handledAddFriend(const char *name_1, const char *name_2)
{
  qDebug()<<"添加好友"<<name_1<<name_2;
  QSqlQuery query;
  QString order=QString("select id from usrinfo where name=\'%1\' "
                        "union all "
                        "select id from usrinfo where name=\'%2\'").arg(name_1).arg(name_2);
  qDebug()<<"order:"<<order;
  query.exec(order);
  query.next();
  QString id_1=query.value(0).toString();
  query.next();
  QString id_2=query.value(0).toString();
  if(id_1==id_2)
    order=QString("insert into friend values(%1,%2)").arg(id_1).arg(id_2);
  else
    order=QString("insert into friend values(%1,%2),values(%3,%4)").arg(id_1).arg(id_2).arg(id_2).arg(id_1);
  query.exec(order);
  qDebug()<<"order:"<<order;
}

QStringList OpeDB::handleAllOnline()
{
  QString order =QString("select name from usrinfo where online=1");
  QSqlQuery query;
  query.exec(order);

  QStringList res;
  res.clear();

  while(query.next()){
      res.append(query.value(0).toString());
    }

  return res;
}

QStringList OpeDB::handleFlushFriend(const char *name)
{
  QStringList strFriendList;
  strFriendList.clear();
  if(name==NULL){
      return strFriendList;
    }
  QString order=QString("select name from usrinfo where name in (select friend from usrWithFriends where name=\'%1\')").arg(name);
  QSqlQuery query;
  query.exec(order);
  qDebug()<<order;
  while(query.next()){
      strFriendList.append(query.value(0).toString());
      qDebug()<<"OpeDB::handleFlushFriend:friend"<<query.value(0).toString();
    }
  return strFriendList;
}

bool OpeDB::handleDelFriend(const char *name_1, const char *name_2)
{
  if(name_1==NULL||name_2==NULL){
      return false;
    }
  QString order=QString("delete from friend where id=(select id from usrinfo where name=\'%1\') and friendid=(select id from usrinfo where name=\'%2\')").arg(name_1).arg(name_2);
  QSqlQuery query;
  qDebug()<<"OpeDB::handleDelFriend:"<<order;
  query.exec(order);
  order=QString("delete from friend where id=(select id from usrinfo where name=\'%1\') and friendid=(select id from usrinfo where name=\'%2\')").arg(name_2).arg(name_1);
  query.exec(order);
  return true;
}

QStringList OpeDB::handleAllFriendOnline(const char *name)
{
  QStringList strFriendList;
  strFriendList.clear();
  if(name==NULL){
      return strFriendList;
    }
  QString order=QString("select name from usrinfo where online=1 and name in (select friend from usrWithFriends where name=\'%1\')").arg(name);
  QSqlQuery query;
  query.exec(order);
  qDebug()<<order;
  while(query.next()){
      strFriendList.append(query.value(0).toString());
      qDebug()<<"OpeDB::handleAllFriendOnline:online friend:"<<query.value(0).toString();
    }
  return strFriendList;
}

void OpeDB::allOffLine()
{
  QString order=QString("update usrinfo set online=0");
  QSqlQuery query;
  query.exec(order);

}
