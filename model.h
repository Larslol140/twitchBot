#ifndef MODEL_H
#define MODEL_H

#include "client.h"

class Model : public QObject
{
    Q_OBJECT

  private:
    static Model *instance;
    Model(QObject *parent);

    QHash<QString, Client *>  clients;
    QList<QString>            selectedClients;

    Client    *currentClient;

    bool      isClientInList(QString userName);
    bool      successfullyAuthenticated(QString userName);
    void      addClientToList(QString userName, QString userOauth);
    void      removeClientFromList(QString userName);


    QString   formatChat(Message *m);
    QString   formatStatus(Message *m);
    QString   formatPingPong(Message *m);
    QString   formatJoinLeave(Message *m);

    void      changeCurrentClient(QString userName);

    void      selectClient(QString userName);
    void      unselectClient(QString userName);
    void      addClientToSelection(QString userName);
    void      removeClientFromSelection(QString userName);
    bool      isClientSelected(QString userName);

    bool      messageIsCommand(QString message);

    void      executeCommand(QString message);

  public:
    static Model *getInstance(QObject *parent = nullptr);
    ~Model();

    void      addClient(QString userName, QString userOauth);
    void      setCurrentClient(QString userName);
    Client    *getCurrentClient();
    void      sendMessage(QString message);
    QString   formatMessage(Message *m);

    void      sendWithAllClients(QString channel, QString message);
    void      sendWithClients(QList<QString> clientNames, QString channel, QString message);

    QList<QString>  getClients();


  private slots:
    void      messageReceived();
    void      channelChanged();

  signals:
    void      clientsChanged(QList<QString>);
    void      clientChanged(QString userName);
    void      messageReceived(QString message);
    void      channelChanged(QString channel);
};

#endif // MODEL_H
