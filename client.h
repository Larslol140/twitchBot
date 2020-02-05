#ifndef CLIENT_H
#define CLIENT_H

#include "networkhandler.h"

class Client : public QObject
{
    Q_OBJECT

  private:
    QString userName;
    QString currentChannel;
    bool    authenticated;

    QList<Message *> messages;
    NetworkHandler *nh;

    void              disconnectNetworkHandler();
    void              connectNetworkHandler(QString userName, QString userOauth);

  public:
    Client(QString userName, QString userOauth, QObject *parent = nullptr);
    ~Client();

    QList<Message *>  getMessages();
    QList<Message *>  getMessagesByUser(QString user);
    QList<Message *>  getMessagesByChannel(QString channel);

    QString           getCurrentChannel();
    QString           getUserName();
    bool              getAuthenticated();
    bool              isInChannel();

    void              sendMessage(QString message);
    void              joinChannel(QString channel);
    void              leaveChannel();
    void              pingServer();

    void              login(QString userName, QString userOauth);

  private slots:
    void  messageReceived(Message *m);
    void  messageSent(Message *m);
    void  authFailed();
};

#endif // CLIENT_H
