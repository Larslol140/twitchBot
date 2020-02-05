#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QtNetwork>
#include "message.h"

class NetworkHandler : public QObject
{
    Q_OBJECT

  private:
    QTcpSocket      *tcpSocket;
    QList<QString>  queue;
    bool            commandBeingExecuted;

    void          connectToHost(QString host, quint64 port);
    void          userLogin(QString userName, QString userOauth);
    void          appendEnter(QString &content) const;
    void          sendRawMessage(QString content);
    void          sendPong();

    void          addToQueue(QString message);
    QString       getPendingCommand() const;
    void          removeLastCommand();

    QString       getReplacedString(QString base, QString key, QString val);

    void          waitForCommmandFinish();
    void          executeCommand();

    void          executeAllCommands();

    void          handleRawMessage(QString message);
    void          handleRawSentMessage(QString message);
    MESSAGE_TYPE  determineMessageType(QString message);
    Message       *convertToMessage(QString message);
    Message       *makeLikeRecv(QString message, MESSAGE_TYPE type);

    bool          isServerPing(Message *m);

  private slots:
    void          handleRawMessages();

  public:
                  NetworkHandler(QString userName, QString userOauth, QObject *parent = nullptr);
                  ~NetworkHandler();

    void          sendMessage(QString message, QString channel);
    void          joinChannel(QString channel);
    void          leaveChannel(QString channel);
    void          sendPing();

  signals:
    void          receivedMessage(Message *);
    void          messageSent(Message *);
};

#endif // NETWORKHANDLER_H
