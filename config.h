#ifndef CONFIG_H
#define CONFIG_H

#include <QtCore>

enum  MESSAGE_TYPE { PING_PONG, JOIN_LEAVE, STATUS, CHAT, UNKOWN };

const QString IRC_HOST      = "irc.chat.twitch.tv";
const quint64 IRC_PORT      = 6667;

const QString MESSAGE_PONG  = "PONG tmi.twitch.tv";
const QString MESSAGE_PING  = "PING";

const QString MESSAGE_PASS  = "PASS :oauth";
const QString MESSAGE_NICK  = "NICK :nick";
const QString MESSAGE_CHAT  = "PRIVMSG #:channel ::message";
const QString MESSAGE_JOIN  = "JOIN #:channel";
const QString MESSAGE_PART  = "PART #:channel";

const QString AUTH_FAILED   = ":tmi.twitch.tv NOTICE * :Improperly formatted auth";
const QString AUTH_INVALID  = ":tmi.twitch.tv NOTICE * :Login authentication failed";

const QString MESSAGE_SKEL_CHAT = "::user!:user@:user.tmi.twitch.tv ";

const QRegularExpression RE_CHAT(":([\\w]+)\\![\\w]+\\@[\\w]+\\.tmi\\.twitch\\.tv PRIVMSG #([\\w]+) :(.+)");
const QRegularExpression RE_STATUS("(?::[\\w]+|)[:\\.]tmi\\.twitch\\.tv ([\\d]+) \\w+ (?::|)(.+)");
const QRegularExpression RE_PING_PONG("(PING|PONG) .+");
const QRegularExpression RE_JOIN_LEAVE(":[\\w]+![\\w]+@[\\w]+\\.tmi\\.twitch\\.tv (JOIN|PART) #([\\w]+)");


#endif // CONFIG_H
