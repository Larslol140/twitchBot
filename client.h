#ifndef CLIENT_H
#define CLIENT_H

#include "networkhandler.h"
#include "database.h"

class Client : public QObject
{
    Q_OBJECT

  private:
    QString           userName;
    QString           currentChannel;
    bool              authenticated;

    QList<Message *>  messages;
    QHash<QString, QString> commands;
    NetworkHandler    *nh;
    Database          *db;

    void              disconnectNetworkHandler();
    void              connectNetworkHandler(QString userName, QString userOauth);

    void              loadCommands();

    bool              isCommand(QString command);

    void              sendCommandResponse(QString response);

  public:
    Client(QString userName, QString userOauth, bool database = false, QObject *parent = nullptr);
    ~Client();

    Message           *getLastMessage();
    QList<Message *>  getMessages();
    QList<Message *>  getMessagesByUser(QString user);
    QList<Message *>  getMessagesByChannel(QString channel);

    QString           getCurrentChannel();
    QString           getClientName();
    bool              getAuthenticated();
    bool              isInChannel();

    void              sendMessage(QString message);
    void              joinChannel(QString channel);
    void              leaveChannel();
    void              pingServer();

    void              addCommand(QString command_name, QString command_trigger, QString command_response);
    void              deleteCommand(int command_id);
    void              deleteCommand(QString command_trigger);

    void              appendMessage(QString message);

    void              login(QString userName, QString userOauth, bool database);

  private slots:
    void  messageReceived(Message *m);
    void  messageSent(Message *m);
    void  authFailed();

  signals:
    void  channelChanged();
    void  messageReceived();
};

#endif // CLIENT_H
