#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <crow.h>
#include <RCON.hpp>

class WebServer {
private:

	RCON rcon;

	crow::SimpleApp server;
	time_t nextMessageSendable = 0;

	std::unordered_map<std::string, std::function<std::string(const crow::request& req)>> commands;

	std::string handleRCON(const crow::request& req);
	std::string handeReadFile(const crow::request& req);

public:

	WebServer();
	void start(int port);

	void createCommand(const char* name, std::function<std::string(const crow::request& req)> fn);

};

#endif WEBSERVER_HPP