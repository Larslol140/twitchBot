#include "apihandler.h"

ApiHandler *ApiHandler::instance = nullptr;

ApiHandler::ApiHandler(QObject *parent ) : QObject (parent)
{
  nam = new QNetworkAccessManager;
  connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(reqRecv(QNetworkReply *)));
}

void ApiHandler::queryApi(QString queryString, int id)
{
  QNetworkRequest nr;
  nr.setUrl(QUrl(TWITCH_API_URL+queryString+QString::number(id)));
  nr.setRawHeader("Client-ID", TWITCH_API_CLIENT_ID.toUtf8());
  nam->get(nr);
  addToQuery(QString::number(id), nr);
}

void ApiHandler::queryApi(QString queryString, QString name)
{
  QNetworkRequest nr;
  nr.setUrl(QUrl(TWITCH_API_URL+queryString+name));
  nr.setRawHeader("Client-ID", TWITCH_API_CLIENT_ID.toUtf8());
  nam->get(nr);
  addToQuery(name, nr);
}

void ApiHandler::addToQuery(QString user_name, QNetworkRequest req)
{
  query[req.url().toString()] = user_name;
}

ApiHandler::~ApiHandler()
{
  instance = nullptr;
}

void ApiHandler::getStreamer(QString user)
{
  queryApi("streams?user_login=", user);
}

void ApiHandler::getUser(QString user)
{
  queryApi("users?login=", user);
}

void ApiHandler::getGame(int game_id)
{
  queryApi("games?=", game_id);
}

QString ApiHandler::getResponseContent(QString user_name)
{
  return responses[user_name]->readAll();
}

QJsonObject ApiHandler::getData(const QJsonObject &rawObject)
{
  return rawObject["data"].toArray().first().toObject();
}

QJsonObject ApiHandler::convertResponse(const QByteArray &resp)
{
  return QJsonDocument(QJsonDocument::fromJson(resp)).object();
}

void ApiHandler::reqRecv(QNetworkReply *nr)
{
  responses[query[nr->url().toString()]] = nr;
  query.remove(nr->url().toString());
  emit responseReceived(query[nr->url().toString()]);
}

ApiHandler *ApiHandler::getInstance(QObject *parent)
{
  if (instance == nullptr)
    instance = new ApiHandler(parent);
  return instance;
}
