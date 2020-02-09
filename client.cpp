#include "client.h"

void Client::disconnectNetworkHandler()
{
  disconnect(nh, SIGNAL(messageReceived(Message *)), this, SLOT(messageReceived(Message *)));
  disconnect(nh, SIGNAL(messageSent(Message *)), this, SLOT(messageSent(Message *)));
  disconnect(nh, SIGNAL(authFailed()), this, SLOT(authFailed()));
  delete nh;
  nh = nullptr;
}

void Client::connectNetworkHandler(QString userName, QString userOauth)
{
  nh = new NetworkHandler(userName, userOauth);
  connect(nh, SIGNAL(messageReceived(Message *)), this, SLOT(messageReceived(Message *)));
  connect(nh, SIGNAL(messageSent(Message *)), this, SLOT(messageSent(Message *)));
  connect(nh, SIGNAL(authFailed()), this, SLOT(authFailed()));
}

void Client::loadCommands()
{
  commands.clear();
  QList<int> commandIds = db->getCommands(userName, currentChannel);

  for (int i = 0; i < commandIds.length(); i++)
    commands[db->getCommandTrigger(commandIds[i])] = db->getCommandResponse(commandIds[i]);
}

bool Client::isCommand(QString command)
{
  return commands.contains(command);
}

Client::Client(QString userName, QString userOauth, bool database, QObject *parent) : QObject (parent)
{
  this->userName = userName;
  nh = nullptr;
  db = Database::getInstance();
  currentChannel = "";
  login(userName, userOauth, database);
}

Client::~Client()
{
  delete nh;
  nh = nullptr;
  for (int i = 0; i < messages.length(); i++)
  {
    delete messages[i];
    messages[i] = nullptr;
  }
}

Message *Client::getLastMessage()
{
  return messages.last();
}

QList<Message *> Client::getMessages()
{
  return messages;
}

QList<Message *> Client::getMessagesByUser(QString user)
{
  QList<Message *> tmp;

  for (int i = 0; i < messages.length(); i++)
    if (messages[i]->getSender() == user)
      tmp.append(messages[i]);
  return tmp;
}

QList<Message *> Client::getMessagesByChannel(QString channel)
{
  QList<Message *> tmp;

  for (int i = 0; i < messages.length(); i++)
    if (messages[i]->getChannel() == channel)
      tmp.append(messages[i]);
  return tmp;
}

QString Client::getCurrentChannel()
{
  return currentChannel;
}

QString Client::getClientName()
{
  return userName;
}

bool Client::getAuthenticated()
{
  return authenticated;
}

bool Client::isInChannel()
{
  return currentChannel != "";
}

void Client::sendMessage(QString message)
{
  if (isInChannel() && authenticated)
    nh->sendMessage(message, currentChannel);
}

void Client::joinChannel(QString channel)
{
  if (isInChannel())
    leaveChannel();
  nh->joinChannel(channel);
  currentChannel = channel;
  loadCommands();
  emit channelChanged();
}

void Client::leaveChannel()
{
  if (!isInChannel())
    return;
  nh->leaveChannel(currentChannel);
  currentChannel = "";
  emit channelChanged();
}

void Client::pingServer()
{
  nh->sendPing();
}

void Client::addCommand(QString command_name, QString command_trigger, QString command_response)
{
  db->addCommand(userName, currentChannel, command_name, command_trigger, command_response);
  loadCommands();
}

void Client::deleteCommand(int command_id)
{
  db->deleteCommand(command_id);
  loadCommands();
}

void Client::deleteCommand(QString command_trigger)
{
  QList<int> commandIds = db->getCommands(userName, currentChannel);

  for (int i = 0; i < commandIds.length(); i++)
    if (db->getCommandTrigger(commandIds[i]) == command_trigger)
      db->deleteCommand(commandIds[i]);
  loadCommands();
}

void Client::appendMessage(QString message)
{
  Message *m = new Message(message, UNKOWN);
  messages.append(m);
  emit messageReceived();
}

void Client::login(QString userName, QString userOauth, bool database)
{
  if (nh != nullptr)
    disconnectNetworkHandler();
  authenticated = true;
  if (database)
    db->addClient(userName, userOauth);
  connectNetworkHandler(userName, userOauth);
}

void Client::messageReceived(Message *m)
{
  if (isInChannel() && isCommand(m->getMessage()))
    sendMessage(commands[m->getMessage()]);
  messages.append(m);
  emit messageReceived();
}

void Client::messageSent(Message *m)
{
  m->changeSender(userName);
  if (isInChannel() && isCommand(m->getMessage()))
    sendMessage(commands[m->getMessage()]);
  messages.append(m);
  emit messageReceived();
}

void Client::authFailed()
{
  authenticated = false;
}
