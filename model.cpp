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
  emit clientsChanged(getClients());
  emit clientChanged(currentClient->getClientName());
}

void Model::removeClientFromList(QString userName)
{
  Client *c = clients[userName];

  disconnect(currentClient, SIGNAL(messageReceived()), this, SLOT(messageReceived()));
  disconnect(currentClient, SIGNAL(channelChanged()), this, SLOT(channelChanged()));

  delete c;
  c = nullptr;
  clients.remove(userName);

  emit clientsChanged(getClients());

  emit clientChanged("NONE");
}

QString Model::formatMessage(Message *m)
{
  switch (m->getType())
  {
    case CHAT:
      return formatChat(m);
      break;
    case STATUS:
      return formatStatus(m);
      break;
    case JOIN_LEAVE:
      return formatJoinLeave(m);
      break;
    case PING_PONG:
      return formatPingPong(m);
      break;
    default:
      return m->getMessage();
      break;
  }
}

void Model::sendWithAllClients(QString channel, QString message)
{
  QList<QString> clientNames = clients.keys();
  sendWithClients(clientNames, channel, message );
}

void Model::sendWithClients(QList<QString> clientNames, QString channel, QString message)
{
  for (int i = 0; i < clientNames.length(); i++)
    clients[clientNames[i]]->joinChannel(channel);
  for (int i = 0; i < clientNames.length(); i++)
    clients[clientNames[i]]->sendMessage(message);
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
  return "["+m->getTime().toString()+"] "+"("+m->getMessage()+")";
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
  emit channelChanged(currentClient->getCurrentChannel());
}

bool Model::messageIsCommand(QString message)
{
  return message.startsWith(".");
}

void Model::executeCommand(QString message)
{
  QRegularExpressionMatch match = RE_COMMAND.match(message);
  QString command = match.captured(1);
  QString param1 = match.captured(2);
  QString param2 = match.captured(3);

  if (command == "addClient")
    addClient(param1, param2);
  else if (command == "changeClient")
    setCurrentClient(param1);
  else if (command == "removeClient")
    removeClientFromList(param1);
  else if (command == "join")
    currentClient->joinChannel(param1);
  else if (command == "leave")
    currentClient->leaveChannel();
  else if (command == "massSend")
    sendWithAllClients(param1, param2);
  else if (command == "ping")
    currentClient->pingServer();
  else
    qDebug().noquote() << "Unkown Command [" << command << "/" << param1 << "|" << param2 << "]";
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

QList<QString> Model::getClients()
{
  return clients.keys();
}

void Model::messageReceived()
{
  emit messageReceived(formatMessage(currentClient->getLastMessage()));
}

void Model::channelChanged()
{
  emit channelChanged(currentClient->getCurrentChannel());
}

void Model::sendMessage(QString message)
{
  if (messageIsCommand(message))
    executeCommand(message);
  else if (message != "")
    currentClient->sendMessage(message);
}
