#ifndef MCBACKEND_HPP
#define MCBACKEND_HPP

#include <crow.h>
#include <RCON.hpp>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

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
	ONLINE_EMPTY,
	ONLINE_USING
};

class MCServer {
public:

	MCServerDesc initDesc;
	RCON rcon;

	std::string name;
	std::filesystem::path folder;

	std::filesystem::path startFile;
	time_t nextServerStart;

	time_t serverEmptyTime;

	std::string address;
	uint32_t port;

	uint32_t rconPort;

	SERVER_STATE state;

	int socketHandle;
	bool listeningActive;

	bool initWithFolder(const std::filesystem::path& serverFolder);
	bool getStatus();

	bool start();
	bool stop();

	void tpcListener();

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

public:

	MCBackend();

	bool addServer(MCServerDesc serverDesc);

	bool initialize(int webServerThread);
	bool deInitialize();

	void update();

	void createCommand(const char* name, std::function<std::string(const crow::request& req)> fn);

};

#endif // MCBACKEND_HPP