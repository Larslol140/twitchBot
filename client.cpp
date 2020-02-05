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

Client::Client(QString userName, QString userOauth, QObject *parent) : QObject (parent)
{
  this->userName = userName;
  nh = nullptr;
  currentChannel = "";
  login(userName, userOauth);
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

QString Client::getUserName()
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
}

void Client::leaveChannel()
{
  if (!isInChannel())
    return;
  nh->leaveChannel(currentChannel);
  currentChannel = "";
}

void Client::pingServer()
{
  nh->sendPing();
}

void Client::login(QString userName, QString userOauth)
{
  if (nh != nullptr)
    disconnectNetworkHandler();
  authenticated = true;
  connectNetworkHandler(userName, userOauth);
}

void Client::messageReceived(Message *m)
{
  messages.append(m);
}

void Client::messageSent(Message *m)
{
  m->changeSender(userName);
  messages.append(m);
}

void Client::authFailed()
{
  authenticated = false;
}
