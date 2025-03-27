#ifndef MCBACKEND_HPP
#define MCBACKEND_HPP

#include <crow.h>
#include <RCON.hpp>

struct MCServerDesc {

	std::filesystem::path serverFolder;

	bool autoStart = true;

	bool autoStop = true;
	uint16_t autoStopAfter = 10; // Seconds

	bool checkData();
};

enum SERVER_STATE : uint8_t {
	OFFLINE,
	STARTING,
	ONLINE
};

class MCServer {
public:

	RCON rcon;

	std::string name;
	std::filesystem::path folder;

	std::filesystem::path startFile;
	time_t nextServerStart;

	std::string address;
	uint32_t port;

	uint32_t rconPort;

	SERVER_STATE state;

	bool initWithFolder(const std::filesystem::path& serverFolder);
	bool getStatus(bool* isEmpty);
	bool start();

};

class MCBackend {
private:

	std::vector<MCServer> servers;

	crow::SimpleApp webServer;
	time_t nextMessageSendable = 0;

	std::unordered_map<std::string, std::function<std::string(const crow::request& req)>> commands;

	std::string handleRCON(const crow::request& req);
	std::string handeReadFile(const crow::request& req);

	void webServerThread(int webServerThread);
	bool tpcListener();

public:

	MCBackend();

	bool addServer(MCServerDesc serverDesc);

	bool initialize(int webServerThread);
	bool deInitialize();

	void update();

	void createCommand(const char* name, std::function<std::string(const crow::request& req)> fn);

};

#endif MCBACKEND_HPP