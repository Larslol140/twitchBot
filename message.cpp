#include "message.h"

QTime Message::getTime() const
{
    return time;
}

MESSAGE_TYPE Message::getType() const
{
    return type;
}

QString Message::getStatus() const
{
  return status;
}

QString Message::getChannel() const
{
  return channel;
}

QString Message::getMessage() const
{
  return message;
}

QString Message::getSender() const
{
  return sender;
}

void Message::changeSender(QString sender)
{
  this->sender = sender;
}

void Message::extractChatData(QString rawMessage)
{
  QRegularExpressionMatch match = getRegExpMatch(RE_CHAT, rawMessage);
  sender = match.captured(1);
  channel = match.captured(2);
  message = match.captured(3);
  status = "";
  if (isSlashMe(message))
    removeSlashMe(message);
}

void Message::extractStatusData(QString rawMessage)
{
  QRegularExpressionMatch match = getRegExpMatch(RE_STATUS, rawMessage);

  sender = "SERVER";
  channel = "";
  status = match.captured(1);
  message = match.captured(2);
}

void Message::extractJoinLeaveData(QString rawMessage)
{
  QRegularExpressionMatch match = getRegExpMatch(RE_JOIN_LEAVE, rawMessage);

  sender = "SERVER";
  message = "";
  status = match.captured(1);
  channel = match.captured(2);
}

void Message::extractPingPongData(QString rawMessage)
{
  QRegularExpressionMatch match = getRegExpMatch(RE_PING_PONG, rawMessage);
  sender = "SERVER";
  channel = "";
  status = "";
  message = match.captured(1);
}

void Message::extractUnkownData(QString rawMessage)
{
  message = rawMessage;
  sender = "UNKOWN";
  channel = "UNKOWN";
  status = "UNKOWN";
}

bool Message::isSlashMe(QString message)
{
  return message.contains("\u0001") && message.contains("ACTION");
}

void Message::removeSlashMe(QString &message)
{
  message.replace("\u0001", "").replace("ACTION", "");
}

QRegularExpressionMatch Message::getRegExpMatch(QRegularExpression regex, QString string)
{
  return regex.match(string);
}

Message::Message(QString rawMessage, MESSAGE_TYPE type)
{
  time = QTime::currentTime();
  this->type = type;

  switch (type)
  {
    case CHAT:
      extractChatData(rawMessage);
      break;
    case STATUS:
      extractStatusData(rawMessage);
      break;
    case JOIN_LEAVE:
      extractJoinLeaveData(rawMessage);
      break;
    case PING_PONG:
      extractPingPongData(rawMessage);
      break;
    case UNKOWN:
      extractUnkownData(rawMessage);
      break;
  }
}
