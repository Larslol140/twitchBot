#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include "config.h"

class Database
{
  private:
    static Database   *instance;
                      Database();

    QSqlDatabase      getSqlDatabase() const;
    QSqlQuery         getSqlQuery() const;
    QSqlQuery         getSqlQuery(QString query) const;

    bool              databaseFileExisits(QString fileName) const;
    void              connectDatabase(QSqlDatabase &db);
    void              createDatabase(QSqlDatabase &db);
    void              deleteDatbase();

  public:
    static Database   *getInstance();
                      ~Database();

    QList<QString>    getClients() const;
    QString           getClientOauth(QString client_name) const;
    void              deleteClient(QString client_name) const;
    void              addClient(QString client_name, QString client_oauth) const;

    QList<int>        getCommands() const;
    QList<int>        getCommands(QString command_client, QString command_channel) const;
    QString           getCommandName(int command_id) const;
    QString           getCommandTrigger(int command_id) const;
    QString           getCommandResponse(int command_id) const;
    void              deleteCommand(int command_id) const;
    void              addCommand(QString command_client, QString command_channel, QString command_name, QString command_trigger, QString command_response);

};

#endif // DATABASE_H
