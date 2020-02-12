#ifndef APIHANDLER_H
#define APIHANDLER_H

#include "config.h"
#include <QtNetwork>

class ApiHandler : public QObject
{
    Q_OBJECT

  private:
    static ApiHandler *instance;
    ApiHandler(QObject *parent);

    QHash<QString, QString> query;
    QHash<QString, QNetworkReply *> responses;

    QNetworkAccessManager *nam;

    void  queryApi(QString queryString, QString name);
    void  queryApi(QString queryString, int id);

    void  addToQuery(QString user_name, QNetworkRequest req);

  public:
    static ApiHandler *getInstance(QObject *parent = nullptr);
    ~ApiHandler();

    void  getStreamer(QString user);
    void  getUser(QString user);
    void  getGame(int game_id);

    QString getResponseContent(QString user_name);

    static QJsonObject getData(const QJsonObject &rawObject);
    static QJsonObject convertResponse(const QByteArray &resp);


  signals:
    void  responseReceived(QString);

  private slots:
    void  reqRecv(QNetworkReply *nr);
};

#endif // APIHANDLER_H
