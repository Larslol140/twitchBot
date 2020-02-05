CREATE TABLE users (
  user_name VARCHAR(64) NOT NULL PRIMARY KEY,
  user_auth VARCHAR(255) NOT NULL
);

CREATE TABLE commands (
  command_id INTEGER PRIMARY KEY AUTOINCREMENT,
  command_user VARCHAR(64) NOT NULL REFERENCES users(user_name),
  command_channel VARCHAR(64) NOT NULL,
  command_name VARCHAR(255) NOT NULL,
  command_trigger VARCHAR(255) NOT NULL,
  command_response VARCHAR(255) NOT NULL
);
