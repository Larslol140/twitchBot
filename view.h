#ifndef VIEW_H
#define VIEW_H

#include <QtWidgets>
#include "model.h"

class View : public QWidget
{
    Q_OBJECT

  private:
    Model *m;
    QListWidget *lwUsers;
    QLabel      *lChannel;
    QLabel      *lClient;
    QTextEdit   *teChat;
    QLineEdit   *leMessage;
    QLineEdit   *leCmdTrigger;
    QLineEdit   *leCmdResponse;
    QPushButton *pbAddCommand;
    QPushButton *pbDelCommand;


    void        setUpGui();
    void        connectComponents();
    void        loadLastMessages();

    int         getStartMessage(QList<Message *> messages);
    int         getEndMessage(QList<Message *> messages);

  public:
    View(QWidget *parent = nullptr);

  private slots:
    void  changeClient(QListWidgetItem *item);
    void  updateChannel(QString channel);
    void  updateClients(QList<QString> clients);
    void  updateCurrentClient(QString userName);
    void  updateMessages(QString message);
    void  executeCommand();

    void  addCommand();
    void  delCommand();

};

#endif // VIEW_H
