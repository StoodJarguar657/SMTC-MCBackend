#ifndef MCBACKEND_HPP
#define MCBACKEND_HPP

#include <thread>
#include <crow.h>
#include <RCON.hpp>

enum SERVER_DESC_START : uint8_t {
	START_NONE,
	PROXY_SERVER,
	KEEP_ONLINE
};

enum SERVER_DESC_STOP : uint8_t {
	STOP_NONE,
	AUTO_STOP,
};

struct MCServerDesc {

	std::filesystem::path serverFolder;

	SERVER_DESC_START startLogic = PROXY_SERVER;
	SERVER_DESC_STOP stopLogic = AUTO_STOP;

	uint16_t autoStopAfter = 20;

	bool checkData();
};

enum SERVER_STATE : uint8_t {
	OFFLINE,
	STARTING,
	STOPPING,
	ONLINE_EMPTY,
	ONLINE_USING
};

class MCServer {
public:

	MCServerDesc initDesc;

	std::string name;
	SERVER_STATE state;

	int playerCount;
	time_t lastPlayerActivity;

	RCON rcon;
	
	std::filesystem::path folder;
	std::filesystem::path startFile;

	std::string rawAddress;
	std::string address;

	uint32_t port;
	uint32_t rconPort;

	bool initWithFolder(const std::filesystem::path& serverFolder);
	bool getStatus();

	bool start();
	bool stop();

	bool tpcListener();

	void update();

};

class MCBackend {
private:

	std::unordered_map<std::string, std::pair<std::thread, MCServer>> servers;

	crow::SimpleApp webServer;
	time_t nextMessageSendable = 0;
	bool wantsShutdown = false;

	std::unordered_map<std::string, std::function<std::string(const crow::request& req)>> commands;

	std::string handleRCON(const crow::request& req);
	std::string handleReadFile(const crow::request& req);

	void webServerThread(int webServerThread);

public:

	MCBackend();

	bool addServer(MCServerDesc* serverDesc);

	bool initialize(int webServerThread);
	bool deInitialize();

	void createCommand(const char* name, std::function<std::string(const crow::request& req)> fn);

};

#endif // MCBACKEND_HPP