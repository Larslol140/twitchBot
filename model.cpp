#include "model.h"

Model *Model::instance = nullptr;

Model::Model(QObject *parent) : QObject (parent)
{
  currentClient = nullptr;
}

bool Model::isClientInList(QString userName)
{
  return clients.keys().contains(userName);
}

bool Model::successfullyAuthenticated(QString userName)
{
  return clients[userName]->getAuthenticated();
}

void Model::addClientToList(QString userName, QString userOauth)
{
  Client *c = new Client(userName, userOauth);
  clients[userName] = c;
  setCurrentClient(userName);
}

void Model::removeClientFromList(QString userName)
{
  Client *c = clients[userName];

  disconnect(currentClient, SIGNAL(messageReceived()), this, SLOT(messageReceived()));
  disconnect(currentClient, SIGNAL(channelChanged()), this, SLOT(channelChanged()));

  delete c;
  c = nullptr;
  clients.remove(userName);

  emit clientChanged("-- NO CLIENT --");
}

QString Model::formatMessage(Message *m)
{
  if (m->getType() == CHAT)
    return formatChat(m);
  else if (m->getType() == STATUS)
    return formatStatus(m);
  else if (m->getType() == JOIN_LEAVE)
    return formatJoinLeave(m);
  else if (m->getType() == PING_PONG)
    return formatPingPong(m);
  else
    return m->getMessage();
}

QString Model::formatChat(Message *m)
{
  return "["+m->getTime().toString()+"] (CHAT) #"+m->getChannel()+" "+m->getSender()+": "+m->getMessage();
}

QString Model::formatStatus(Message *m)
{
  return "["+m->getTime().toString()+"] (STATUS) "+m->getStatus()+" "+m->getMessage();
}

QString Model::formatPingPong(Message *m)
{
  return "["+m->getTime().toString()+"]"+"("+m->getMessage()+")";
}

QString Model::formatJoinLeave(Message *m)
{
  return "["+m->getTime().toString()+"] ("+m->getStatus()+") #"+m->getChannel();
}

void Model::changeCurrentClient(QString userName)
{
  if (currentClient != nullptr)
  {
    disconnect(currentClient, SIGNAL(messageReceived()), this, SLOT(messageReceived()));
    disconnect(currentClient, SIGNAL(channelChanged()), this, SLOT(channelChanged()));
  }
  currentClient = clients[userName];
  connect(currentClient, SIGNAL(messageReceived()), this, SLOT(messageReceived()));
  connect(currentClient, SIGNAL(channelChanged()), this, SLOT(channelChanged()));
  emit clientChanged(currentClient->getClientName());
}

Model *Model::getInstance(QObject *parent)
{
  if (instance == nullptr)
    instance = new Model(parent);
  return instance;
}

Model::~Model()
{
  instance = nullptr;
}

void Model::addClient(QString userName, QString userOauth)
{
  if (isClientInList(userName))
    changeCurrentClient(userName);
  else
    addClientToList(userName, userOauth);
}

void Model::setCurrentClient(QString userName)
{
  if (isClientInList(userName))
    changeCurrentClient(userName);
}

Client *Model::getCurrentClient()
{
  return currentClient;
}

void Model::messageReceived()
{
  qDebug() << formatMessage(currentClient->getLastMessage());
  emit messageReceived(formatMessage(currentClient->getLastMessage()));
}

void Model::channelChanged()
{
  qDebug() << currentClient->getCurrentChannel();
  emit channelChanged(currentClient->getCurrentChannel());
}
