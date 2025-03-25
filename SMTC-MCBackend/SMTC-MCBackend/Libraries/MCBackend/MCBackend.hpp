#ifndef MCBACKEND_HPP
#define MCBACKEND_HPP

#include <crow.h>
#include <RCON.hpp>

struct MCBackendDesc {

	const char* serverAddress = "127.0.0.1";
	int serverPort = 0;
	std::filesystem::path serverFolder;

	bool autoStartServer = true;

	bool autoStopServer = true;
	int autoStopAfter = 10; // 10 seconds

	int webServerPort = 0;

	const char* rconAddr = "127.0.0.1";
	int rconPort = 25575; // Default port
};

class MCBackend {
private:

	MCBackendDesc initDesc;

	RCON rcon;

	crow::SimpleApp webServer;
	time_t nextMessageSendable = 0;

	time_t nextServerStart = 0;
	std::filesystem::path startFile;

	std::unordered_map<std::string, std::function<std::string(const crow::request& req)>> commands;

	std::string handleRCON(const crow::request& req);
	std::string handeReadFile(const crow::request& req);

	void webServerThread();
	bool tpcListener();

	bool startServer();
	bool checkServerStatus(bool* isEmpty);

public:

	MCBackend();
	bool start(MCBackendDesc initDesc);
	void update();

	void createCommand(const char* name, std::function<std::string(const crow::request& req)> fn);

};

#endif MCBACKEND_HPP