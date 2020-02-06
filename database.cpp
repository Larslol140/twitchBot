#include "database.h"

Database *Database::instance = nullptr;

Database::Database()
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "MAIN");
  db.setDatabaseName(DB_FILE);
  connectDatabase(db);
}

QSqlDatabase Database::getSqlDatabase() const
{
  return QSqlDatabase::database("MAIN");
}

QSqlQuery Database::getSqlQuery(QString query) const
{
  QSqlQuery q(getSqlDatabase());
  q.prepare(query);
  return q;
}

bool Database::databaseFileExisits(QString fileName) const
{
  return QFile::exists(fileName);
}

void Database::connectDatabase(QSqlDatabase &db)
{
  if (databaseFileExisits(DB_FILE))
    db.open();
  else
    createDatabase(db);
}

void Database::createDatabase(QSqlDatabase &db)
{
  db.open();
  getSqlQuery(DB_CREATE_CLIENTS).exec();
  getSqlQuery(DB_CREATE_COMMANDS).exec();
}

void Database::deleteDatbase()
{
  getSqlDatabase().close();
  QFile::remove(DB_FILE);
}

Database::~Database()
{
  instance = nullptr;
}

QList<QString> Database::getClients() const
{
  QSqlQuery q = getSqlQuery(DB_SELECT_CLIENTS);
  q.exec();

  QList<QString> tmp;

  while (q.next())
    tmp.append(q.value(0).toString());

  return tmp;
}

QString Database::getClientOauth(QString client_name) const
{
  QSqlQuery q = getSqlQuery(DB_SELECT_CLIENT_AUTH);
  q.bindValue(":client_name", client_name);
  q.exec();

  if (q.first())
    return q.value(0).toString();
  return "";
}

void Database::deleteClient(QString client_name) const
{
  QSqlQuery q = getSqlQuery(DB_DEL_CLIENT);
  q.bindValue(":client_name", client_name);
  q.exec();
}

void Database::addClient(QString client_name, QString client_oauth) const
{
  QSqlQuery q = getSqlQuery(DB_ADD_CLIENT);
  q.bindValue(":client_name", client_name);
  q.bindValue(":client_oauth", client_oauth);
  q.exec();
}

QList<int> Database::getCommands() const
{
  QSqlQuery q = getSqlQuery(DB_SELECT_ALL_COMMANDS);
  q.exec();

  QList<int> tmp;

  while (q.next())
    tmp.append(q.value(0).toInt());
  return tmp;
}

QList<int> Database::getCommands(QString command_client, QString command_channel) const
{
  QSqlQuery q = getSqlQuery(DB_SELECT_COMMANDS);
  q.bindValue(":command_channel", command_channel);
  q.bindValue(":command_client", command_client);
  q.exec();

  QList<int> tmp;

  while (q.next())
    tmp.append(q.value(0).toInt());
  return tmp;
}

QString Database::getCommandName(int command_id) const
{
  QSqlQuery q = getSqlQuery(DB_SELECT_CMD_NAME);
  q.bindValue(":command_id", command_id);
  q.exec();

  if (q.first())
    return q.value(0).toString();
  return "";
}

QString Database::getCommandTrigger(int command_id) const
{
  QSqlQuery q = getSqlQuery(DB_SELECT_CMD_TRIGGER);
  q.bindValue(":command_id", command_id);
  q.exec();

  if (q.first())
    return q.value(0).toString();
  return "";
}

QString Database::getCommandResponse(int command_id) const
{
  QSqlQuery q = getSqlQuery(DB_SELECT_CMD_RESPONSE);
  q.bindValue(":command_id", command_id);
  q.exec();

  if (q.first())
    return q.value(0).toString();
  return "";
}

void Database::deleteCommand(int command_id) const
{
  QSqlQuery q = getSqlQuery(DB_DEL_COMMAND);
  q.bindValue(":command_id", command_id);
  q.exec();
}

void Database::addCommand(QString command_client, QString command_channel, QString command_name, QString command_trigger, QString command_response)
{
  QSqlQuery q = getSqlQuery(DB_ADD_COMMAND);
  q.bindValue(":command_client", command_client);
  q.bindValue(":command_channel", command_channel);
  q.bindValue(":command_name", command_name);
  q.bindValue(":command_trigger", command_trigger);
  q.bindValue(":command_response", command_response);
  q.exec();
}

Database *Database::getInstance()
{
  if (instance == nullptr)
    instance = new Database;
  return instance;
}
