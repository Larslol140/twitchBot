#ifndef MODEL_H
#define MODEL_H

#include "client.h"

class Model : public QObject
{
    Q_OBJECT

  private:
    static Model *instance;
    Model(QObject *parent);

    QHash<QString, Client *> clients;

    Client    *currentClient;

    bool      isClientInList(QString userName);
    bool      successfullyAuthenticated(QString userName);
    void      addClientToList(QString userName, QString userOauth);
    void      removeClientFromList(QString userName);

    QString   formatMessage(Message *m);

    QString   formatChat(Message *m);
    QString   formatStatus(Message *m);
    QString   formatPingPong(Message *m);
    QString   formatJoinLeave(Message *m);

    void      changeCurrentClient(QString userName);

  public:
    static Model *getInstance(QObject *parent = nullptr);
    ~Model();

    void      addClient(QString userName, QString userOauth);
    void      setCurrentClient(QString userName);
    Client    *getCurrentClient();

  private slots:
    void      messageReceived();
    void      channelChanged();

  signals:
    void      clientChanged(QString userName);
    void      messageReceived(QString message);
    void      channelChanged(QString channel);
};

#endif // MODEL_H
