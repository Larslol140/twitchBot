#include "model.h"

Model *Model::instance = nullptr;

Model::Model(QObject *parent) : QObject (parent)
{
  currentClient = nullptr;
  db = Database::getInstance();
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
  Client *c = new Client(userName, userOauth, true);
  clients[userName] = c;
  setCurrentClient(userName);
  emit clientsChanged(getClients());
  emit clientChanged(currentClient->getClientName());
}

void Model::removeClientFromList(QString userName)
{
  Client *c = clients[userName];

  disconnect(c, SIGNAL(messageReceived()), this, SLOT(messageReceived()));
  disconnect(c, SIGNAL(channelChanged()), this, SLOT(channelChanged()));

  if (currentClient == c)
    currentClient = nullptr;

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

void Model::sendWithClients(QString message)
{
  for (int i = 0; i < selectedClients.length(); i++)
    clients[selectedClients[i]]->sendMessage(message);
}

void Model::sendWithClientsToChannel(QString channel, QString message)
{
  if (channel == "CURRENT")
    sendWithClients(message);
  else
  {
    for (int i = 0; i < selectedClients.length(); i++)
      if (clients[selectedClients[i]]->getCurrentChannel() != channel)
        clients[selectedClients[i]]->joinChannel(channel);
    for (int i = 0; i < selectedClients.length(); i++)
      clients[selectedClients[i]]->sendMessage(message);
  }
}

void Model::addCommand(QString command_trigger, QString command_response)
{
  currentClient->addCommand(command_trigger.remove(0, 1), command_trigger, command_response);
}

void Model::deleteCommand(QString command_trigger)
{
  currentClient->deleteCommand(command_trigger);
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

void Model::selectAllClients()
{
  for (int i = 0; i < clients.keys().length(); i++)
    selectClient(clients.keys()[i]);
}

void Model::selectClient(QString userName)
{
  if (isClientInList(userName))
    addClientToSelection(userName);
  emit clientsChanged(clients.keys());
}

void Model::unselectClient(QString userName)
{
  if (isClientSelected(userName))
    selectedClients.removeOne(userName);
  emit clientsChanged(clients.keys());
}

void Model::addClientToSelection(QString userName)
{
  if (isClientSelected(userName))
    return;
  selectedClients.append(userName);
}

void Model::removeClientFromSelection(QString userName)
{
  if (isClientSelected(userName))
    selectedClients.removeOne(userName);
}

bool Model::isClientSelected(QString userName)
{
  return selectedClients.contains(userName);
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
  else if (command == "loadClient")
    loadClient(param1);
  else if (command == "loadAllClients")
    loadAllClients();
  else if (command == "unloadAllClients")
    unloadAllClients();
  else if (command == "changeClient")
    setCurrentClient(param1);
  else if (command == "unloadClient")
    removeClientFromList(param1);
  else if (command == "join")
    currentClient->joinChannel(param1);
  else if (command == "leave")
    currentClient->leaveChannel();
  else if (command == "selectedSend")
    sendWithClientsToChannel(param1, param2);
  else if (command == "selectedJoin")
    joinWithClients(param1);
  else if (command == "selectedLeave")
    leaveWithClients();
  else if (command == "ping")
    currentClient->pingServer();
  else if (command == "select")
    selectClient(param1);
  else if (command == "unselect")
    unselectClient(param1);
  else if (command == "selectAll")
    selectAllClients();
  else if (command == "unselectAll")
    unselectAllClients();
  else if (command == "help")
    commandHelp();
  else if (command == "exit")
    qApp->exit();
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

void Model::loadClient(QString userName)
{
  if (isClientInDatabase(userName))
    addClient(userName, db->getClientOauth(userName));
}

void Model::loadAllClients()
{
  QList<QString> clients = db->getClients();
  for (int i = 0; i < clients.length(); i++)
    loadClient(clients[i]);
}

void Model::commandHelp()
{
  if (currentClient != nullptr)
    currentClient->appendMessage(COMMAND_HELP);
}

void Model::setCurrentClient(QString userName)
{
  if (isClientInList(userName))
    changeCurrentClient(userName);
}

bool Model::isClientInDatabase(QString userName)
{
  return db->getClients().contains(userName);
}

void Model::joinWithClients(QString channel)
{
  for (int i = 0; i < selectedClients.length(); i++)
    clients[selectedClients[i]]->joinChannel(channel);
}

void Model::leaveWithClients()
{
  for (int i = 0; i < selectedClients.length(); i++)
    clients[selectedClients[i]]->leaveChannel();
}

void Model::unselectAllClients()
{
  int selectedSize = selectedClients.length();
  for (int i = 0; i < selectedSize; i++)
    unselectClient(selectedClients.last());
}

void Model::unloadAllClients()
{
  int clientSize = clients.keys().length();
  for (int i = 0; i < clientSize; i++)
    removeClientFromList(clients.keys().last());
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
