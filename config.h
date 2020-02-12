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

const QString COMMAND_HELP = ".addClient, .loadClient, .loadAllCLients, .unloadAllClients, "
                             ".changeClient, .unloadClient, .join, .leave, .selectedSend, "
                             ".selectedJoin, .selectedLeave, .ping, .select, .unselect, "
                             ".selectAll, .unselectAll, .exit, .help";

const QRegularExpression RE_CHAT(":([\\w]+)\\![\\w]+\\@[\\w]+\\.tmi\\.twitch\\.tv PRIVMSG #([\\w]+) :(.+)");
const QRegularExpression RE_STATUS("(?::[\\w]+|)[:\\.]tmi\\.twitch\\.tv ([\\d]+) \\w+ (?::|)(.+)");
const QRegularExpression RE_PING_PONG("(PING|PONG) .+");
const QRegularExpression RE_JOIN_LEAVE(":[\\w]+![\\w]+@[\\w]+\\.tmi\\.twitch\\.tv (JOIN|PART) #([\\w]+)");
const QRegularExpression RE_COMMAND("\\.([\\w]+)(?: |)([\\w]*)(?: |)(.*)");
const QRegularExpression TWITCH_API_REGEX("https:\\/\\/api\\.twitch\\.tv\\/helix\\/\\w+\?\\w+=(.+)");
const QRegularExpression RE_ONLY_NUMBER("[\\d+]");

const int LAST_MESSAGE_COUNT = 100;

const QString DB_FILE                  = "data.db";
const QString DB_CREATE_COMMANDS       = "CREATE TABLE commands ("
                                         "  command_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                         "  command_client VARCHAR(64) NOT NULL REFERENCES users(user_name),"
                                         "  command_channel VARCHAR(64) NOT NULL,"
                                         "  command_name VARCHAR(255) NOT NULL,"
                                         "  command_trigger VARCHAR(255) NOT NULL,"
                                         "  command_response VARCHAR(255) NOT NULL"
                                         ");";
const QString DB_CREATE_CLIENTS         = "CREATE TABLE clients ("
                                        "  client_name VARCHAR(64) NOT NULL PRIMARY KEY,"
                                        "  client_oauth VARCHAR(255) NOT NULL"
                                        ");";

const QString DB_SELECT_ALL_COMMANDS  = "SELECT command_id FROM commands;";
const QString DB_SELECT_COMMANDS      = "SELECT command_id FROM commands "
                                        "WHERE command_client = :command_client AND command_channel = :command_channel;";
const QString DB_SELECT_CMD_TRIGGER   = "SELECT command_trigger FROM commands "
                                        "WHERE command_id = :command_id;";
const QString DB_SELECT_CMD_RESPONSE  = "SELECT command_response FROM commands "
                                        "WHERE command_id = :command_id";
const QString DB_SELECT_CMD_NAME      = "SELECT command_name FROM commands "
                                        "WHERE command_id = :command_id;";
const QString DB_ADD_COMMAND          = "INSERT INTO commands(command_client, command_channel, command_name, command_trigger, command_response) "
                                        "VALUES (:command_client, :command_channel, :command_name, :command_trigger, :command_response);";
const QString DB_DEL_COMMAND          = "DELETE FROM commands "
                                        "WHERE command_id = :command_id;";

const QString DB_SELECT_CLIENTS       = "SELECT client_name FROM clients;";
const QString DB_SELECT_CLIENT_AUTH   = "SELECT client_oauth FROM clients "
                                        "WHERE client_name = :client_name;";
const QString DB_ADD_CLIENT           = "INSERT INTO clients(client_name, client_oauth) "
                                        "VALUES (:client_name, :client_oauth);";
const QString DB_DEL_CLIENT           = "DELETE FROM clients "
                                        "WHERE client_name = :client_name";

const QString TWITCH_API_URL          = "https://api.twitch.tv/helix/";
const QString TWITCH_API_CLIENT_ID    = "";

#endif // CONFIG_H
