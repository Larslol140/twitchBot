#include "networkhandler.h"

void NetworkHandler::connectToHost(QString host, quint64 port)
{
  tcpSocket->connectToHost(host, port);
}

void NetworkHandler::userLogin(QString userName, QString userOauth)
{
  sendRawMessage(getReplacedString(MESSAGE_PASS, ":oauth", userOauth));
  sendRawMessage(getReplacedString(MESSAGE_NICK, ":nick", userName));
}

void NetworkHandler::appendEnter(QString &content) const
{
  content.append("\r\n");
}

void NetworkHandler::sendRawMessage(QString content)
{
  appendEnter(content);
  addToQueue(content);
  executeCommand();
}

void NetworkHandler::sendPong()
{
  sendRawMessage(MESSAGE_PONG);
  handleRawSentMessage(MESSAGE_PONG);
}

QString NetworkHandler::getPendingCommand() const
{
  return queue.last();
}

void NetworkHandler::removeLastCommand()
{
  queue.removeLast();
}

void NetworkHandler::addToQueue(QString message)
{
  queue.append(message);
}

QString NetworkHandler::getReplacedString(QString base, QString key, QString val)
{
  return base.replace(key, val);
}

void NetworkHandler::waitForCommmandFinish()
{
  tcpSocket->waitForBytesWritten();
}

void NetworkHandler::executeCommand()
{
  while (commandBeingExecuted)
    waitForCommmandFinish();
  commandBeingExecuted = true;
  tcpSocket->write(getPendingCommand().toUtf8());
  removeLastCommand();
  commandBeingExecuted = false;
}

void NetworkHandler::executeAllCommands()
{
  for (int i = 0; i < queue.length(); i++)
    executeCommand();
}

void NetworkHandler::handleRawMessage(QString message)
{
  Message *m = convertToMessage(message);
  if (isServerPing(m))
    sendPong();
  emit receivedMessage(m);
}

void NetworkHandler::handleRawSentMessage(QString message)
{
  MESSAGE_TYPE type = determineMessageType(message);
  if (type != CHAT)
    return;
  Message *m = makeLikeRecv(message, type);
  emit messageSent(m);
}

MESSAGE_TYPE NetworkHandler::determineMessageType(QString message)
{
  if (message.contains("PRIVMSG"))
    return CHAT;
  else if (message.contains("JOIN") || message.contains("PART"))
    return JOIN_LEAVE;
  else if (message.contains("PING") || message.contains("PONG"))
    return PING_PONG;
  else if (RE_STATUS.match(message).captured(2) != "")
    return STATUS;
  else
    return UNKOWN;
}

Message *NetworkHandler::convertToMessage(QString message)
{
  return new Message(message, determineMessageType(message));
}

Message *NetworkHandler::makeLikeRecv(QString message, MESSAGE_TYPE type)
{
  message = MESSAGE_SKEL_CHAT+message;
  message.replace(":user", "@@SELF@@");
  return new Message(message, type);
}

bool NetworkHandler::isServerPing(Message *m)
{
  return m->getType() == PING_PONG && RE_PING_PONG.match(m->getMessage()).captured(1) == "PING";
}

void NetworkHandler::handleRawMessages()
{
  QString rawMessage = tcpSocket->readAll();
  QList<QString> rawMessages = rawMessage.split("\r\n");

  for (int i = 0; i < rawMessages.length()-1; i++)
    handleRawMessage(rawMessages[i]);
}

NetworkHandler::NetworkHandler(QString userName, QString userOauth, QObject *parent) : QObject (parent)
{
  commandBeingExecuted = false;
  tcpSocket = new QTcpSocket(this);
  connectToHost(IRC_HOST, IRC_PORT);
  userLogin(userName, userOauth);

  connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(handleRawMessages()));
}

NetworkHandler::~NetworkHandler()
{
  executeAllCommands();
  delete tcpSocket;
  tcpSocket = nullptr;
}

void NetworkHandler::sendMessage(QString message, QString channel)
{
  QString msg = getReplacedString(MESSAGE_CHAT, ":channel", channel);
  msg = getReplacedString(msg, ":message", message);
  sendRawMessage(msg);
  handleRawSentMessage(msg);
}

void NetworkHandler::joinChannel(QString channel)
{
  QString msg = getReplacedString(MESSAGE_JOIN, ":channel", channel);
  sendRawMessage(msg);
  handleRawSentMessage(msg);
}

void NetworkHandler::leaveChannel(QString channel)
{
  QString msg = getReplacedString(MESSAGE_PART, ":channel", channel);
  sendRawMessage(msg);
  handleRawSentMessage(msg);
}

void NetworkHandler::sendPing()
{
  sendRawMessage(MESSAGE_PING);
  handleRawSentMessage(MESSAGE_PING);
}
