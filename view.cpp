#include "view.h"

void View::setUpGui()
{
  QLabel *title = new QLabel("Twitch Bot", this);
  teChat = new QTextEdit(this);
  leMessage = new QLineEdit(this);
  lClient = new QLabel(this);
  lChannel = new QLabel(this);
  lwUsers = new QListWidget(this);

  leCmdTrigger = new QLineEdit(this);
  leCmdResponse = new QLineEdit(this);
  pbAddCommand = new QPushButton("Add Command", this);
  pbDelCommand = new QPushButton("Delete Command", this);


  teChat->setReadOnly(true);

  lClient->setFont(QFont("Arial", 12));
  lChannel->setFont(QFont("Arial", 12));
  title->setFont(QFont("Arial", 12));
  title->setAlignment(Qt::AlignCenter);

  QVBoxLayout *layout = new QVBoxLayout(this);
  QHBoxLayout *userChannel = new QHBoxLayout;
  QVBoxLayout *currentUserList = new QVBoxLayout;
  QHBoxLayout *usersChat = new QHBoxLayout;
  QVBoxLayout *messageChat = new QVBoxLayout;
  QHBoxLayout *commandInputs = new QHBoxLayout;
  QHBoxLayout *commandOutputs = new QHBoxLayout;


  commandInputs->addWidget(leCmdTrigger);
  commandInputs->addWidget(leCmdResponse);

  commandOutputs->addWidget(pbAddCommand);
  commandOutputs->addWidget(pbDelCommand);

  userChannel->addWidget(lClient);
  userChannel->addWidget(lChannel);

  messageChat->addWidget(teChat);
  messageChat->addWidget(leMessage);

  currentUserList->addLayout(userChannel);
  currentUserList->addWidget(lwUsers);
  currentUserList->addLayout(commandInputs);
  currentUserList->addLayout(commandOutputs);

  usersChat->addLayout(messageChat);
  usersChat->addLayout(currentUserList);

  layout->addWidget(title);
  layout->addLayout(usersChat);

  setLayout(layout);

}

void View::connectComponents()
{
  connect(m, SIGNAL(messageReceived(QString)), this, SLOT(updateMessages(QString)));
  connect(m, SIGNAL(channelChanged(QString)), this, SLOT(updateChannel(QString)));
  connect(m, SIGNAL(clientChanged(QString)), this, SLOT(updateCurrentClient(QString)));
  connect(m, SIGNAL(clientsChanged(QList<QString>)), this, SLOT(updateClients(QList<QString>)));

  connect(leMessage, SIGNAL(returnPressed()), this, SLOT(executeCommand()));
  connect(lwUsers, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(changeClient(QListWidgetItem *)));
  connect(pbAddCommand, SIGNAL(clicked()), this, SLOT(addCommand()));
  connect(pbDelCommand, SIGNAL(clicked()), this, SLOT(delCommand()));
}

void View::loadLastMessages()
{
  QList<Message *> messages = m->getCurrentClient()->getMessages();
  teChat->clear();

  int end = getEndMessage(messages);
  int start = getStartMessage(messages);
  while(start < end)
  {
    updateMessages(m->formatMessage(messages[start]));
    start++;
  }
}

int View::getStartMessage(QList<Message *> messages)
{
  return messages.length()-LAST_MESSAGE_COUNT < 0 ? 0 : messages.length()-LAST_MESSAGE_COUNT;
}

int View::getEndMessage(QList<Message *> messages)
{
  return messages.length();
}

View::View(QWidget *parent) : QWidget (parent)
{
  m = Model::getInstance(this);
  setUpGui();
  setWindowTitle("Twitch Bot");
  connectComponents();
}

void View::changeClient(QListWidgetItem *item)
{
  m->setCurrentClient(item->text());
}

void View::updateChannel(QString channel)
{
  lChannel->setText("Channel: "+channel);
}

void View::updateClients(QList<QString> clients)
{
  lwUsers->clear();

  for (int i = 0; i < clients.length(); i++)
    lwUsers->addItem(clients[i]);
}

void View::updateCurrentClient(QString userName)
{
  lClient->setText("User: "+userName);
  if (userName == "NONE")
  {
    teChat->clear();
    updateChannel("NONE");
  }
  else
  {
    updateChannel(m->getCurrentClient()->getCurrentChannel());
    loadLastMessages();
  }
}

void View::updateMessages(QString message)
{
  QList<QString> chat = teChat->toPlainText().split("\n");
  while ( chat.length() > LAST_MESSAGE_COUNT )
    chat.removeFirst();

  chat.append(message);

  QString newChat = "";

  for (int i = 0; i < chat.length(); i++ )
  {
    if (chat[i] != "")
      newChat.append(chat[i].append("\n"));
  }
  teChat->setText(newChat);
  teChat->verticalScrollBar()->setValue(teChat->verticalScrollBar()->maximum());
}

void View::executeCommand()
{
  m->sendMessage(leMessage->text());
  leMessage->clear();
}

void View::addCommand()
{
  m->addCommand(leCmdTrigger->text(), leCmdResponse->text());
  leCmdTrigger->clear();
  leCmdResponse->clear();
}

void View::delCommand()
{
  m->deleteCommand(leCmdTrigger->text());
  leCmdTrigger->clear();
  leCmdResponse->clear();
}
