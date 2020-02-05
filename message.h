#ifndef MESSAGE_H
#define MESSAGE_H

#include "config.h"

class Message
{
  private:
    QString       sender;
    QString       message;
    QString       channel;
    QString       status;
    MESSAGE_TYPE  type;
    QTime         time;

    void          extractChatData(QString rawMessage);
    void          extractStatusData(QString rawMessage);
    void          extractJoinLeaveData(QString rawMessage);
    void          extractPingPongData(QString rawMessage);
    void          extractUnkownData(QString rawMessage);

    QRegularExpressionMatch getRegExpMatch(QRegularExpression regex, QString string);

  public:
    Message(QString rawMessage, MESSAGE_TYPE type);

    QTime getTime() const;
    MESSAGE_TYPE getType() const;
    QString getStatus() const;
    QString getChannel() const;
    QString getMessage() const;
    QString getSender() const;
};

#endif // MESSAGE_H
