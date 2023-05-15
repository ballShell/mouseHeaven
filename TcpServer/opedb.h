#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();
    void init();
    ~OpeDB();

    bool handleRegist(const char* name,const char* pwd);
    bool handleLogin(const char* name,const char* pwd);
    void handleOffline(const char *name);
    int handleSearch(const char* name);
    int handleAddFriendCheck(const char *pername,const char *name);
    void handledAddFriend(const char *name_1,const char *name_2);
    QStringList handleAllOnline();
    QStringList handleFlushFriend(const char *name);
    bool handleDelFriend(const char *name_1,const char *name_2);
    QStringList handleAllFriendOnline(const char* name);
    void allOffLine();

signals:

public slots:
private:
    QSqlDatabase m_db;  //连接数据库
};

#endif // OPEDB_H
